/* Hunter Kitts
 * Lab 3 B_Tree
 * juses jdisk as disk system
 * adds keys and reocords in btree 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "b_tree.h"

typedef struct tnode {
	unsigned char bytes[JDISK_SECTOR_SIZE+256]; /* This holds the sector for reading and writing.  
												   It has extra room because your internal representation  
												   will hold an extra key. */
	unsigned char nkeys;						/* Number of keys in the node */
	unsigned char flush;						/* Should I flush this to disk at the end of b_tree_insert()? */
	unsigned char internal;						/* Internal or external node */
	unsigned int lba;							/* LBA when the node is flushed */
	unsigned char **keys;						/* Pointers to the keys.  Size = MAXKEY+1 */
	unsigned int *lbas;							/* Pointer to the array of LBA's.  Size = MAXKEY+2 */
	struct tnode *parent;						/* Pointer to my parent -- useful for splitting */
	int parent_index;							/* My index in my parent */
	struct tnode *ptr;							/* Free list link */
} Tree_Node;

typedef struct {
	/* First 12/16 bytes in sector 0 */
	int key_size;								/* bytes 0-3 */
	unsigned int root_lba;						/* bytes 4-7 */	
	unsigned long first_free_block;				/* bytes 8-15 or 8-11 */

	void *disk;									/* The jdisk */
	unsigned long size;							/* The jdisk's size */
	unsigned long num_lbas;						/* size/JDISK_SECTOR_SIZE */
	int max_key;								/* MAXKEY */
	int lbas_per_block;							/* MAXKEY+1 */
	Tree_Node *free_list;						/* Free list of nodes */  
	Tree_Node *tmp_e;							/* When find() fails, this is a pointer to the external node */
	int tmp_e_index;							/* and the index where the key should have gone */

	int flush;									/* Should I flush sector[0] to disk after b_tree_insert() */
} B_Tree;


/* puts the tree_node on the free list
 * and flushs it to the jdisk if it has been changed */
void free_and_flush(void *b_tree, void *tree_node)
{

	B_Tree *bt;
	Tree_Node *tn;
	unsigned char buf[JDISK_SECTOR_SIZE] = {0};
	int i;
	int offset = 0;
	//Tree_Node *free_tmp;
	//Tree_Node *tmp;

	tn = (Tree_Node *)tree_node;
	bt = (B_Tree *)b_tree;

	//this is so I can flush just sector 0
	if(tree_node != NULL){

		//flush the node if needed
		if(tn->flush == 1){
			memcpy(buf, (unsigned char*)&tn->internal, sizeof(unsigned char));
			offset += 1;
			memcpy(buf+offset, (unsigned char*)&tn->nkeys, sizeof(unsigned char));
			offset += 1;
			for(i=0; i < (bt->max_key); i++){
				memcpy(buf+offset, tn->keys[i], bt->key_size);
				offset += bt->key_size;
			}
			memcpy(buf+(JDISK_SECTOR_SIZE - bt->lbas_per_block), (unsigned int*)tn->lbas, bt->lbas_per_block);
			//write the buf to the disk 
			jdisk_write(bt->disk, tn->lba, buf);

			//set the node on the free list reset some values
			tn->flush = 0;
			tn->ptr = bt->free_list;
			bt->free_list = tn;
			tn->parent = NULL;
		} 

		/* if flush is not set then add the node to the free list */
		else{
			tn->ptr = bt->free_list;
			bt->free_list = tn;
		}
	}

	/* write the first 16/12 bytes to sector 0 */	
	if(bt->flush == 1){
		printf("flush sector 0\n");
		memcpy(buf, (int *)&bt->key_size, 4);
		memcpy(buf+4, (unsigned int*)&bt->root_lba, 4);
		memcpy(buf+8, (unsigned long*)&bt->first_free_block, sizeof(unsigned long));
		jdisk_write(bt->disk, 0, buf);
		bt->flush = 0;
	}
}

/* insert the key/lba in the Tree_Node already
 * check if node needed to be split
 * Recursivly split node */
int split_node(void *b_tree, void *t_node, unsigned long first_free_block)
{
	B_Tree *bt;
	/* The split needs 3 nodes */
	Tree_Node *original;						//node that needs to be split
	Tree_Node *new_node;						//node that goes to right of original
	Tree_Node *parent;							//parent of both new and original
	unsigned long new_first_free_block;		    //as it splits the first free block moves
	unsigned int blocks_needed;					//blocks needed for this split
	unsigned int key_up_index;					//key that needs to go up
	unsigned char **new_node_keys;				//new nodes keys copied from original
	unsigned int *new_node_lbas;				//new nodes lbas copied from original
	unsigned char **parent_keys;				//parent keys added the key going up
	unsigned int *parent_lbas;					//parent lbas added the lba of original and new node
	int new_root = 0;							//if new root needed
	int first_split = 0;						//if it needs to split again
	int i;
	//these are all for memcpy so the correct size is copied
	unsigned long parent_keys_size;
	unsigned long parent_lbas_size;
	unsigned long new_node_keys_size;
	unsigned long new_node_lbas_size;
	unsigned long bytes_offset;

	//have to set these first to get teh key size
	original = (Tree_Node *)t_node;
	bt = (B_Tree *)b_tree;

	unsigned char key_going_up[bt->key_size];	//key thats going up to parent

	/* if this is not the first call to split node
	 * it could need 1 or 2 blocks depeneding on if its the root
	 * If it is the first call it could need
	 * 2 or 3 because it needs a block for record */
	if(bt->first_free_block != first_free_block){
		if(original->parent == NULL){
			blocks_needed = 2;
			new_root = 1;
		} else{
			blocks_needed = 1;
		}
	} else{
		first_split = 1;
		if(original->parent == NULL){
			blocks_needed = 3;
			new_root = 1;
		} else{
			blocks_needed = 2;
		}
	}

	//check if enough space is available
	if(first_free_block+blocks_needed > bt->num_lbas){
		return 0;
	} else{
		new_first_free_block = first_free_block+blocks_needed;
	}



	//need new node
	if(bt->free_list != NULL){
		//get it off the free list
		new_node = bt->free_list;
		bt->free_list = new_node->ptr;
	} else{
		//malloc a new node
		new_node = (Tree_Node *)malloc(sizeof(Tree_Node));
		new_node->keys = (unsigned char **) malloc(sizeof(char*)*(bt->max_key+1));
		new_node->lbas = (unsigned int *)malloc(sizeof(unsigned int) * (bt->max_key+2));
		for(i=0; i < bt->max_key+1; i++){
			new_node->keys[i] = (unsigned char *) &new_node->bytes+2+(i*bt->key_size);
		}
	}

	//this is to set up the blocks for split
	if(first_split == 1){
		new_node->lba = first_free_block+1;
	} else{
		new_node->lba = first_free_block;
	}

	//need new root
	if(new_root == 1){
		if(bt->free_list != NULL){
			//get it off the free list
			parent = bt->free_list;
			bt->free_list = parent->ptr;
		} else{
			//malloc for a parent
			parent = (Tree_Node *)malloc(sizeof(Tree_Node));
			parent->keys = (unsigned char **) malloc(sizeof(char *)*(bt->max_key+1));
			parent->lbas = (unsigned int *)malloc(sizeof(unsigned int) * (bt->max_key+2));
			for(i=0; i < (bt->max_key+1); i++){
				parent->keys[i] = (unsigned char *) &parent->bytes+2+(i*bt->key_size);	
			}
		}
		//set up new root
		parent->internal = 1;
		parent->lba = new_node->lba+1;
		parent->parent = NULL;
		parent->parent_index = -1;
		parent->nkeys = 0;
		original->parent_index = 0;
	} 

	//parent of node already exists
	else{
		parent = original->parent;
	}

	/* Get the key and index in the node that is going up */
	key_up_index = ceil(original->nkeys/2);
	memcpy(key_going_up, original->bytes+(2+(key_up_index*bt->key_size)), bt->key_size);

	//new node set up --------------------------------------------------------------------------
	/* Set up to copy the keys and lbas in orignal to the new node */
	new_node_keys_size = bt->key_size * (original->nkeys- key_up_index - 1);
	new_node_lbas_size = (original->nkeys - key_up_index) * sizeof(unsigned int);
	if(original->nkeys%2 == 0){
		bytes_offset = 2 + (bt->key_size *(original->nkeys- key_up_index + 1));
	} else{
		bytes_offset = 2 + (bt->key_size *(original->nkeys- key_up_index));
	}

	//copy the data from original into the new_node
	for(i=0; i < (original->nkeys - key_up_index-1); i++){
		memcpy(new_node->bytes+(2+(bt->key_size*i)), original->bytes+bytes_offset, bt->key_size);
		bytes_offset += bt->key_size;
	}
	memcpy(new_node->lbas, original->lbas+(key_up_index+1), new_node_lbas_size);
	new_node->nkeys = (original->nkeys - key_up_index - 1);

	//make it internal or not depending if the orignal is interanl
	if(original->internal == 1){
		new_node->internal = 1;
	} else{
		new_node->internal = 0;
	}
	new_node->parent = parent;

	//parent node set up ------------------------------------------------------------------------
	//add the key to the end if the index is the same as the number of keys
	if(original->parent_index == parent->nkeys){
		memcpy(parent->bytes+(2+(bt->key_size*original->parent_index)), key_going_up, bt->key_size);
		if(new_root == 1){
			memcpy(parent->lbas+(original->parent_index), &original->lba, sizeof(unsigned int));
		}
		memcpy(parent->lbas+(original->parent_index+1), &new_node->lba, sizeof(unsigned int));
		parent->nkeys++;
	}

	/* values must be shifted to the right to insert */
	else{
		parent_keys = malloc((bt->max_key) * bt->key_size);
		parent_lbas = (unsigned int *)malloc(sizeof(unsigned int) * (bt->max_key+1));
		unsigned char keys[(bt->max_key+1) * bt->key_size];
		unsigned long offset = 0;
		parent_keys_size = bt->key_size * (parent->nkeys - original->parent_index);
		parent_lbas_size = sizeof(unsigned int) * (parent->nkeys-original->parent_index);
		memcpy(keys, parent->bytes+(2+(bt->key_size*original->parent_index)), parent_keys_size);
		memcpy(parent_lbas, parent->lbas+(original->parent_index+1), parent_lbas_size);
		offset = (2+(original->parent_index*bt->key_size));
		memcpy(parent->bytes+offset, key_going_up, bt->key_size);
		offset+= bt->key_size;
		memcpy(parent->bytes+offset, keys, parent_keys_size);
		offset = original->parent_index+1;
		memcpy(parent->lbas+offset, (unsigned int*)&new_node->lba, sizeof(unsigned int));
		offset += 1;
		memcpy(parent->lbas+offset, parent_lbas, parent_lbas_size);

		parent->nkeys++;
		free(parent_lbas);
	}

	//oringal set up -----------------------------------------------------------------------------
	original->parent = parent;
	if(original->nkeys%2 == 0){
		original->nkeys = original->nkeys - key_up_index;
	} else{
		original->nkeys = original->nkeys - key_up_index - 1;
	}

	//determine if another split is needed
	if(parent->nkeys > bt->max_key){
		//if there is not enough space for the split return 0
		if(split_node(bt, parent, new_first_free_block) == 0){
			return 0;
		} else{
			//otherwise the spliting is done flush the nodes
			//insert will flush sector 0
			parent->flush = 1;
			new_node->flush = 1;
			original->flush = 1;
			bt->first_free_block += blocks_needed;
			free_and_flush(bt, new_node);
			free_and_flush(bt, original);

			return 1;
		}
	} else{
		//this happens if another split was not needed
		parent->flush = 1;
		new_node->flush = 1;
		original->flush = 1;
		bt->first_free_block += blocks_needed;
		if(new_root == 1){
			bt->root_lba = parent->lba;
		}
		free_and_flush(bt, new_node);
		free_and_flush(bt, original);
		free_and_flush(bt, parent);
		return 1;
	}

	return 0;
}

/* reads a tree_node from a disk 
 * returns the tree_node read */
void *read_tree_node(void *b_tree, unsigned long lba)
{
	B_Tree *bt;
	Tree_Node *tn;
	//char bytes[JDISK_SECTOR_SIZE]; 
	int i;

	bt = (B_Tree *)b_tree;

	/* If there is a free node then don't malloc a new one */
	if(bt->free_list != NULL){
		tn = bt->free_list;
		bt->free_list = tn->ptr;
	}

	/* Otherwise malloc a new tree node */
	else{
		tn = (Tree_Node *)malloc(sizeof(Tree_Node));	
		tn->keys = (unsigned char **)malloc(sizeof(char *)*(bt->max_key+1));
		tn->lbas = (unsigned int *)malloc(sizeof(unsigned int) * (bt->max_key+2));
		for(i=0; i < (bt->max_key+1); i++){
			tn->keys[i] = (unsigned char *) &tn->bytes+2+(i*bt->key_size);
		}
	}

	//read the bytes form the disk
	jdisk_read(bt->disk, lba, tn->bytes);

	//set up the node to return it
	memcpy(&tn->nkeys, tn->bytes+1, 1);
	tn->flush = 0;
	memcpy((unsigned char*)&tn->internal, tn->bytes, 1);
	memcpy(tn->lbas, tn->bytes+(JDISK_SECTOR_SIZE - bt->lbas_per_block),bt->lbas_per_block);
	tn->parent = NULL;
	tn->parent_index = -1;
	tn->ptr = NULL;

	return tn;
}

/* Creates a b_tree
 * steps
 * 1 - creates jdisk
 * 2 - set up B_Tree struct / data for    sector 0
 * 3 - sets up the root node at           sector 1
 * 4 - flush sector 1 and 0 to disk
 */
void *b_tree_create(char *filename, long size, int key_size)
{
	B_Tree *bt;
	Tree_Node *root;
	void *disk = NULL;
	int i;

	//create jdisk
	disk = jdisk_create(filename, size);

	//malloc space for tree info
	bt = (B_Tree *)malloc(sizeof(B_Tree));

	//set up 
	bt->key_size = key_size;
	bt->root_lba = 1;
	bt->first_free_block = 2;
	bt->disk = disk;
	bt->size = size;
	bt->num_lbas = size/JDISK_SECTOR_SIZE;
	bt->max_key = (JDISK_SECTOR_SIZE - 6) / (key_size + 4);
	bt->lbas_per_block = (bt->max_key + 1)*4;
	bt->free_list = NULL;
	bt->flush = 1;
	bt->tmp_e = NULL;
	bt->tmp_e_index = -1;

	/* setup memory for root node 
	 * needs to be external and has no keys */
	root = (Tree_Node *)malloc(sizeof(Tree_Node));	
	root->keys = (unsigned char **) malloc(sizeof(char *)*(bt->max_key+1));
	root->lbas = (unsigned int *)malloc(sizeof(unsigned int) * (bt->max_key+2));
	for(i=0; i < (bt->max_key+1); i++){
		root->keys[i] = (unsigned char *) &root->bytes+2+(i*bt->key_size);
	}
	root->internal = 0;
	root->lba = 1;
	root->nkeys = 0;
	root->flush = 1;
	root->parent = NULL;
	root->parent_index = -1;
	root->ptr = NULL;

	//flush root and sector 0
	free_and_flush(bt, root);

	return bt;
}

/* Attach to already existing b_tree
 * only have to set up b_tree struct
 */
void *b_tree_attach(char *filename)
{
	B_Tree *bt;
	void *disk;
	int r;
	char buf[JDISK_SECTOR_SIZE];
	size_t offset = 0;

	/* malloc the B_Tre */
	bt = (B_Tree *)malloc(sizeof(B_Tree));

	/* Attach to disk jdisk */
	disk = jdisk_attach(filename);
	bt->disk = disk;

	/* Read the first sector into the buf to get the disk info */
	r = jdisk_read(disk, 0, buf);

	/* Get the first 16/12 bytes */
	memcpy((int*)&bt->key_size, buf, 4);
	offset += 4;
	memcpy((unsigned int*)&bt->root_lba, buf+offset, 4);
	offset += 4;
	memcpy((unsigned long*)&bt->first_free_block, buf+offset, sizeof(unsigned long));
	offset += sizeof(unsigned long);

	//set by data
	bt->size = jdisk_size(disk);
	bt->num_lbas = bt->size/JDISK_SECTOR_SIZE;
	bt->max_key = (JDISK_SECTOR_SIZE - 6) / (bt->key_size + 4);
	bt->lbas_per_block = (bt->max_key + 1)*4;
	bt->free_list = NULL;
	bt->tmp_e = NULL;
	bt->tmp_e_index = -1;
	bt->flush = 0;

	return bt;
}

/* Insert record into b_tree at key */
unsigned int b_tree_insert(void *b_tree, void *key, void *record)
{
	B_Tree *bt;
	Tree_Node *tn;

	//the bt->first_free_block cannot change until right before its flushed
	//this is so if a split cannot happen the original values are presereved
	unsigned long next_free_block;
	unsigned long og_free_block;

	unsigned int cur_last_lba;			//the last lba (its 1+tn->nkeys)
	int can_split = 0;					//if the split can happen
	int find_lba = -2;					//lba of the tree_node whey the key is
	//if its -1 the tree_node is in the B_Tree struct

	//used to shift keys and lbas over
	unsigned long current_keys_size;
	unsigned long current_lbas_size;
	unsigned char **current_keys;
	unsigned int *current_lbas;
	int i;

	bt = (B_Tree *)b_tree;
	next_free_block = bt->first_free_block+1;
	og_free_block = bt->first_free_block;

	/* Need to search for the key first */
	find_lba = b_tree_find(b_tree, key);

	/* Not in tree need to insert */
	if(find_lba == -1){
		tn = bt->tmp_e;
		cur_last_lba = tn->lbas[tn->nkeys];

		/* check if enough space is available */
		if(next_free_block > bt->num_lbas){
			return 0;
		} 

		//if the node will have to split determine if it can
		else if(tn->nkeys == bt->max_key){
			//will need space for new node and jdisk sector for record
			if(tn->parent != NULL && next_free_block+2 > bt->num_lbas){
				return 0;
			} else if(tn->parent == NULL && next_free_block+3 > bt->num_lbas){
				return 0;
			}
		}

		/* need to insert the key and record into the node */

		//the key can be the last inside the tree_node
		if(bt->tmp_e_index == tn->nkeys){
			memcpy(tn->bytes+2+(bt->key_size * bt->tmp_e_index), key, bt->key_size);

			//make sure previous last lba is still the last lba
			memcpy(tn->lbas+(bt->tmp_e_index), (unsigned int*)&bt->first_free_block, sizeof(unsigned int));
			memcpy(tn->lbas+(bt->tmp_e_index+1), (unsigned int*)&cur_last_lba, sizeof(unsigned int));
			tn->nkeys++;
		}

		/* values must be shifted to the right to insert */
		else{
			//this should probably change originally my set up was incorrect making this necessary
			current_keys = malloc((bt->max_key) * bt->key_size);
			current_lbas = (unsigned int *)malloc(sizeof(unsigned int) * (bt->max_key+1));
			unsigned char keys[tn->nkeys*bt->key_size];

			//size of values to shift
			current_keys_size = bt->key_size * (tn->nkeys - bt->tmp_e_index);
			current_lbas_size = sizeof(unsigned int) * (tn->nkeys+1 - bt->tmp_e_index);
			//copy values to be shifted
			memcpy(keys, tn->bytes+(2+(bt->key_size*bt->tmp_e_index)), current_keys_size);
			memcpy(current_lbas, tn->lbas+(bt->tmp_e_index), current_lbas_size);

			//write the new key and then the values that were previously saved so the could shift
			memcpy(tn->bytes+(2+(bt->tmp_e_index*bt->key_size)), key, bt->key_size);
			memcpy(tn->lbas+(bt->tmp_e_index), (unsigned int*)&bt->first_free_block, sizeof(unsigned int));
			memcpy(tn->bytes+(2+((bt->tmp_e_index+1)*bt->key_size)), keys, current_keys_size);
			memcpy(tn->lbas+(bt->tmp_e_index+1), current_lbas, current_lbas_size);

			tn->nkeys++;	
			free(current_keys);
			free(current_lbas);
		}

		//check if the node must be split
		if(tn->nkeys > bt->max_key){
			//split the node
			can_split = split_node(bt, tn, bt->first_free_block);

			//split worked write the record and flush sector 0
			if(can_split == 1){
				jdisk_write(bt->disk, og_free_block, record);
				bt->flush = 1;
				find_lba = og_free_block;
				free_and_flush(bt, NULL);
			}
			//split didn't have enough space in the jdisk (failed)
			else{
				return 0;
			}
		}
		//the node doesnt need to split so just flush it to disk
		else{
			jdisk_write(bt->disk, bt->first_free_block, record);
			tn->flush = 1;
			find_lba = bt->first_free_block;
			bt->first_free_block++;
			bt->flush = 1;
			free_and_flush(bt, tn);
		}
	}

	/* The key was already in the tree node
	 * -1 was not return when it tried to find
	 *  Just replace the record */
	else{
		jdisk_write(bt->disk, find_lba, record);
	}

	//the lba the insert was done at
	return find_lba;
}

/* Looks through a b_tree and tries to find the key given */
unsigned int b_tree_find(void *b_tree, void *key)
{
	B_Tree *bt;

	//search will go from root to leaf
	//it sets up the parent nodes so it needs the prev node to set cur->parent
	Tree_Node *cur = NULL;		//current Tree_Node that is being seached
	Tree_Node *prev = NULL;		//previous Tree_Node also cur->parent
	size_t key_len;				//used to memcmp the key and the Tree_Node->key
	int found_internal = 0;		//the node was found internaly if 1
	int r, i;
	int lba_found = -2;			//lba the key should go in
	int parent_index = -1;		//cur->parent_index 
	int tmp_e_index = -1;		
	unsigned int lba = -1;		//the current Tree_Nodes lba (determined by find)

	bt = (B_Tree *)b_tree;

	//read the root node
	cur = read_tree_node(bt, bt->root_lba);
	cur->lba = bt->root_lba;
	cur->parent = NULL;
	key_len = sizeof(key);

	//read through the nodes the key is found
	while(cur->internal == 1 ||( cur->internal == 0 && lba_found == -2)){
		//loop through the current Tree_Node keys and compare them to the key trying to find
		for(i=0; i < cur->nkeys; i++){
			r = memcmp(cur->keys[i], key, bt->key_size);

			//if the keys match
			if(r == 0){
				//if the current node is internal continue but set found_internal = 1
				//key will be in leaf node
				if(cur->internal == 1){
					//go same index as key
					found_internal = 1;
					prev = cur;
					parent_index = i;
					lba = *(cur->lbas+i);
					cur = read_tree_node(bt, *(cur->lbas+i));
					cur->parent = prev;
					cur->parent_index = parent_index;
					cur->lba = lba;
					break;
				}
				//the key has been found in a leaf node
				else{
					lba_found = *(cur->lbas+i);
					break;
				}
			}

			//if the key was already found internally follow a path to the leaf
			else if(found_internal == 1){
				//go far right not external node
				if(cur->internal == 1){
					prev = cur;
					parent_index = cur->nkeys;
					lba = *(cur->lbas+(cur->nkeys));
					cur = read_tree_node(bt, *(cur->lbas+(cur->nkeys)));
					cur->parent = prev;
					cur->parent_index = parent_index;
					cur->lba = lba;
					break;
				}
				//key previously found interanlly return its lba value
				else{
					lba_found = *(cur->lbas+(cur->nkeys));
					cur->parent_index = parent_index;
					break;
				}
			}
			//if the lba needed is to the left of the current key
			else if(r > 0){
				//if its interanl then go left
				if(cur->internal == 1){
					prev = cur;
					parent_index = i;
					lba = *(cur->lbas+i);
					cur = read_tree_node(bt, *(cur->lbas+i));
					cur->parent = prev;
					cur->parent_index = parent_index;
					cur->lba = lba;
					break;
				}
				//if this is hit the key is not in the tree
				//this would mean the find is in an external node
				//and the key should have gone in this location
				else{
					parent_index = i;
					lba_found = -1;
					tmp_e_index = i;
					break;
				}
			}
			//check last key in the current node
			else if(i == cur->nkeys - 1){
				/* only have to check right if its the last key of an interanl node
				 * this is cause there are 1+nkeys for each tree_node */
				if(r < 0 && cur->internal == 1){
					prev = cur;
					parent_index = i+1;
					lba = *(cur->lbas+i+1);
					cur = read_tree_node(bt, *(cur->lbas+i+1));
					cur->parent = prev;
					cur->parent_index = parent_index;
					cur->lba = lba;
					break;
				}
				//the key was not found
				//the key in the exteranl node was checked
				//it goes to the right of that
				else{
					parent_index = i+1;
					lba_found = -1;
					tmp_e_index = i + 1;
					break;
				}
			}
		} //end of for loop

		//this should only happen when a new tree is made
		//and the root is empty
		if(cur->nkeys == 0){
			tmp_e_index = 0;
			lba_found = -1;
		}

	} //end of while

	//if the key was not found set the values in bt
	//do not free the nodes on the path
	//need there parent info to insert new key
	if(lba_found == -1){
		bt->tmp_e = cur;
		bt->tmp_e_index = tmp_e_index;
	}

	//if the value was in the tree free the list
	else{
		Tree_Node *flushing = cur;
		while(flushing != NULL){
			free_and_flush(bt, flushing);
			flushing = flushing->parent;
		}
	}
	
	return lba_found;
}

//returns the disk of the current b_tree
void *b_tree_disk(void *b_tree)
{
	B_Tree *bt;
	bt = (B_Tree *)b_tree;
	return bt->disk;
}

//returns the size of the keys in the current b_tree
int b_tree_key_size(void *b_tree)
{
	B_Tree *bt;
	bt = (B_Tree *)b_tree;
	return bt->key_size;	
}

//I didn't implement this
void b_tree_print_tree(void *b_tree)
{
	printf("This does nothing yet\n");	
}

