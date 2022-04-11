#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "b_tree.h"

typedef struct tnode {
	unsigned char bytes[JDISK_SECTOR_SIZE+256]; /* This holds the sector for reading and writing.  
												   It has extra room because your internal representation  
												   will hold an extra key. */
	unsigned char nkeys;                      /* Number of keys in the node */
	unsigned char flush;                      /* Should I flush this to disk at the end of b_tree_insert()? */
	unsigned char internal;                   /* Internal or external node */
	unsigned int lba;                         /* LBA when the node is flushed */
	unsigned char **keys;                     /* Pointers to the keys.  Size = MAXKEY+1 */
	unsigned int *lbas;                       /* Pointer to the array of LBA's.  Size = MAXKEY+2 */
	struct tnode *parent;                     /* Pointer to my parent -- useful for splitting */
	int parent_index;                         /* My index in my parent */
	struct tnode *ptr;                        /* Free list link */
} Tree_Node;

typedef struct {
	/* First 12/16 bytes in sector 0 */
	int key_size;						/* bytes 0-3 */
	unsigned int root_lba;			/* bytes 4-7 */	
	unsigned long first_free_block;	/* bytes 8-15 or 8-11 */

	void *disk;						/* The jdisk */
	unsigned long size;				/* The jdisk's size */
	unsigned long num_lbas;			/* size/JDISK_SECTOR_SIZE */
	int max_key;						/* MAXKEY */
	int lbas_per_block;				/* MAXKEY+1 */
	Tree_Node *free_list;				/* Free list of nodes */  
	Tree_Node *tmp_e;					/* When find() fails, this is a pointer to the external node */
	int tmp_e_index;					/* and the index where the key should have gone */

	int flush;						/* Should I flush sector[0] to disk after b_tree_insert() */
} B_Tree;

void free_and_flush(void *b_tree, void *tree_node)
{

	B_Tree *bt;
	Tree_Node *tn;
	Tree_Node *free_tmp;
	Tree_Node *tmp;
	tn = (Tree_Node *)tree_node;
	bt = (B_Tree *)b_tree;
	unsigned char buf[JDISK_SECTOR_SIZE] = {0};
	int i;
	int offset = 0;

	if(tree_node != NULL){

	//flush the node if needed
	if(tn->flush == 1){

		printf("flush: %d\n", tn->lba);
		memcpy(buf, (unsigned char*)&tn->internal, sizeof(unsigned char));
		offset += 1;
		memcpy(buf+offset, (unsigned char*)&tn->nkeys, sizeof(unsigned char));
		offset += 1;
		for(i=0; i < (bt->max_key); i++){
			//printf("%d\n", i);
			memcpy(buf+offset, tn->keys[i], bt->key_size);
			offset += bt->key_size;
		}
		memcpy(buf+(JDISK_SECTOR_SIZE - bt->lbas_per_block), (unsigned int*)tn->lbas, bt->lbas_per_block);

		//printf("buf+2: %c\n", buf[2]);
		int test;
		test = jdisk_write(bt->disk, tn->lba, buf);

		//printf("test: %d\n", test);
		tn->flush = 0;
		tn->ptr = bt->free_list;
		bt->free_list = tn;
		tn->parent = NULL;
	} 

	/* if flush is not set then add the node to the free list */
	else{
		printf("dont flush: %d\n", tn->lba);
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
	printf("\ninside split node\n");
	B_Tree *bt;
	Tree_Node *original;
	Tree_Node *new_node;
	Tree_Node *parent;
	unsigned long new_first_free_block;
	unsigned int blocks_needed;
	unsigned int key_up_index;
	//unsigned char **key_going_up;
	unsigned char **new_node_keys;
	unsigned int *new_node_lbas;
	unsigned char **parent_keys;
	unsigned int *parent_lbas;
	int new_root = 0;
	int first_split = 0;
	int i;
	unsigned long parent_keys_size;
	unsigned long parent_lbas_size;


	original = (Tree_Node *)t_node;
	bt = (B_Tree *)b_tree;

	unsigned char key_going_up[bt->key_size];

	printf("original->lba: %d\n", original->lba);
	if(original->parent != NULL) printf("original->parent->lba: %d\n", original->parent->lba);

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
		printf("split node not enough free space\n");
		return 0;
	} else{
		new_first_free_block = first_free_block+blocks_needed;
	}



	//need new node
	if(bt->free_list != NULL){
		printf("new node of free list\n");
		new_node = bt->free_list;
		bt->free_list = new_node->ptr;
	} else{
		printf("alloc new node\n");
		new_node = (Tree_Node *)malloc(sizeof(Tree_Node));
		new_node->keys = (unsigned char **) malloc(sizeof(char*)*(bt->max_key+1));
		//new_node->keys = malloc((bt->max_key+1) * bt->key_size);
		new_node->lbas = (unsigned int *)malloc(sizeof(unsigned int) * (bt->max_key+2));
		for(i=0; i < bt->max_key+1; i++){
			new_node->keys[i] = (unsigned char *) &new_node->bytes+2+(i*bt->key_size);
		}
		/*
		   for(i=0; i < (bt->max_key+1); i++){
		   new_node->keys[i] = &new_node->bytes[2+(bt->key_size * i)];
		   }
		   */
	}

	//printf("got new_node\n");
	if(first_split == 1){
		new_node->lba = first_free_block+1;
	} else{
		new_node->lba = first_free_block;
	}
	//printf("new_node->lba: %d\n\n\n", new_node->lba);

	//need new root
	if(new_root == 1){
		if(bt->free_list != NULL){
			printf("got root off free list\n");
			parent = bt->free_list;
			bt->free_list = parent->ptr;
		} else{
			printf("malloc new parent\n");
			parent = (Tree_Node *)malloc(sizeof(Tree_Node));
			parent->keys = (unsigned char **) malloc(sizeof(char *)*(bt->max_key+1));
			parent->lbas = (unsigned int *)malloc(sizeof(unsigned int) * (bt->max_key+2));
			for(i=0; i < (bt->max_key+1); i++){
				parent->keys[i] = (unsigned char *) &parent->bytes+2+(i*bt->key_size);	
				//parent->keys[i] = &parent->bytes[2+(bt->key_size * i)];
			}
		}
		printf("got new root\n");
		parent->internal = 1;
		parent->lba = new_node->lba+1;
		parent->parent = NULL;
		parent->parent_index = -1;
		parent->nkeys = 0;

		original->parent_index = 0;
		printf("new_root_lba: %d\n", parent->lba);
	} 
	//parent of node already exists
	else{
		printf("\ntestagain\n");
		printf("original->lba: %d\n", original->lba);
		//printf("parent->lba: %d\n", parent->lba);
		parent = original->parent;
	}
	printf("original->parent->lba: %d\n" ,parent->lba);
	printf("new_node->lba: %d\n", new_node->lba);
	if(original->parent != NULL){
		printf("original->lba: %d\n", original->lba);
		printf("original->parent->lba: %d\n", original->parent->lba);
		printf("yup changed\n");
	}

	/* Do actual splitting */
	/*	
		for(i=0; i < bt->max_key; i++){
		printf("original->keys[%d]: %s\n", i, original->keys[i]);
		printf("original->lbas[%d]: %d\n", i, original->lbas[i]);
		}
		printf("original->lbas[%d]: %d\n", i, original->lbas[i]);
		*/	
	key_up_index = ceil(original->nkeys/2);
	//printf("key_up_index: %d\n", key_up_index);
	//key_going_up = &original->keys[key_up_index];
	memcpy(key_going_up, original->bytes+(2+(key_up_index*bt->key_size)), bt->key_size);
	unsigned long new_node_keys_size;
	unsigned long new_node_lbas_size;
	unsigned long bytes_offset;



	new_node_keys_size = bt->key_size * (original->nkeys- key_up_index - 1);
	new_node_lbas_size = (original->nkeys - key_up_index) * sizeof(unsigned int);
	//bytes_offset = 2 + (bt->key_size *(original->nkeys- key_up_index + 1));
	if(original->nkeys%2 == 0){
		bytes_offset = 2 + (bt->key_size *(original->nkeys- key_up_index + 1));
	} else{
		bytes_offset = 2 + (bt->key_size *(original->nkeys- key_up_index));
	}

	printf("original->nkeys: %d\n", original->nkeys);
	printf("bt->key_size: %d\n", bt->key_size);
	printf("key_up_index: %d\n", key_up_index);
	printf("bytes_offset: %d\n", bytes_offset);
	printf("new_node_keys_size: %d\n", new_node_keys_size);
	printf("new_node_lbas_size: %d\n", new_node_lbas_size);
	printf("bytes: %s\n", original->bytes+bytes_offset);


	printf("original\n");
	printf("lba: %d\n", original->lba);
	for(i=0; i < original->nkeys; i++){
		printf("original->keys[%d]: %s\n", i, original->keys[i]);
		printf("original->lbas[%d]: %d\n", i, original->lbas[i]);
	}
	//printf("original->keys[%d]: %s\n", i, original->keys[i]);
	printf("original->lbas[%d]: %d\n", i, original->lbas[i]);
	printf("\n");


	//set up the new node
	printf("original->nkeys - key_up_index-1: %d\n", original->nkeys - key_up_index-1);	
	for(i=0; i < (original->nkeys - key_up_index-1); i++){
		/*
		   printf("i: %d\n", i);
		   printf("(2+(bt->key_size*i)): %d\n", (2+(bt->key_size*i)));
		   printf("bytes_offset: %d\n", bytes_offset);
		   printf("key: %s\n", original->bytes+bytes_offset);
		   */
		memcpy(new_node->bytes+(2+(bt->key_size*i)), original->bytes+bytes_offset, bt->key_size);
		bytes_offset += bt->key_size;
	}


	//memcpy(new_node->bytes+2, original->bytes+2+bytes_offset, new_node_keys_size);
	memcpy(new_node->lbas, original->lbas+(key_up_index+1), new_node_lbas_size);
	new_node->nkeys = (original->nkeys - key_up_index - 1);

	if(original->internal == 1){
		new_node->internal = 1;
	} else{
		new_node->internal = 0;
	}
	new_node->parent = parent;

	//free(new_node_keys);
	//free(new_node_lbas);

	//set up parent
	//parent->nkeys++;

	//add the key_goind_up to its correct position in the parent node

	printf("original->parent_index: %d\n", original->parent_index);
	printf("parent->nkeys: %d\n", parent->nkeys);
	if(original->parent_index == parent->nkeys){
		printf("add key to end\n");
		printf("key_going_up: %s\n", key_going_up);
		//printf("(unsigned int*)&new_node->lba: %d\n", &new_node->lba);
		//add the key to the end
		memcpy(parent->bytes+(2+(bt->key_size*original->parent_index)), key_going_up, bt->key_size);
		//memcpy(parent->keys+(original->parent_index), key_going_up, bt->key_size);
		if(new_root == 1){
			memcpy(parent->lbas+(original->parent_index), &original->lba, sizeof(unsigned int));
		}
		memcpy(parent->lbas+(original->parent_index+1), &new_node->lba, sizeof(unsigned int));
		printf("child lba: %d\n", parent->lbas[original->parent_index]);

		parent->nkeys++;

		/*
		   printf("first key:    %s\n", parent->keys[0]);
		   printf("new last key: %s\n", parent->keys[1]);
		   printf("first lba:    %d\n", parent->lbas[0]);
		   printf("last lba:     %d\n", parent->lbas[parent->nkeys]);
		   printf("2nd last lba: %d\n", parent->lbas[parent->nkeys-1]);
		   printf("check byte of new last: %c\n", parent->bytes[2+(bt->key_size*1)]);
		   */	
	}

	/* values must be shifted to the right to insert */
	else{
		parent_keys = malloc((bt->max_key) * bt->key_size);
		parent_lbas = (unsigned int *)malloc(sizeof(unsigned int) * (bt->max_key+1));
		unsigned char keys[(bt->max_key+1) * bt->key_size];
		unsigned long offset = 0;
		parent_keys_size = bt->key_size * (parent->nkeys - original->parent_index);
		parent_lbas_size = sizeof(unsigned int) * (parent->nkeys-original->parent_index);
		//printf("parent_keys_size: %d\n", parent_keys_size);
		//printf("keys size: %d\n\n\n", (bt->max_key+1) * bt->key_size);
		memcpy(keys, parent->bytes+(2+(bt->key_size*original->parent_index)), parent_keys_size);
		//printf("keys[0]: %c\n", keys[0]);
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

		/*
		   for(i=0; i < parent->nkeys; i++){
		   printf("parent->keys[%d]: %s\n", i, parent->keys[i]);
		   printf("parent->lbas[%d]: %d\n", i, parent->lbas[i]);
		   }
		   printf("parent->lbas[%d]: %d\n", i, parent->lbas[i]);
		   */

		//free(parent_keys);
		//free(parent_lbas);
	}

	printf("key_going_up: %s\n", key_going_up);
	//printf("original: -----------\n");
	original->parent = parent;
	if(original->nkeys%2 == 0){
		original->nkeys = original->nkeys - key_up_index;
		printf("original->nkeys: %d\n", original->nkeys);
	} else{
		printf("original keys odd\n\n");
		original->nkeys = original->nkeys - key_up_index - 1;
		printf("original->nkeys: %d\n", original->nkeys);
	}

	printf("\nprint split nodes\n");

	if(new_root == 1){
		printf("new root\n");
		printf("lba: %d\n", parent->lba);
		for(i=0; i < parent->nkeys; i++){
			printf("parent->keys[%d]: %s\n", i, parent->keys[i]);
			printf("parent->lbas[%d]: %d\n", i, parent->lbas[i]);
		}
		printf("parent->keys[%d]: %s\n", i, parent->keys[i]);
		printf("parent->lbas[%d]: %d\n", i, parent->lbas[i]);
	} else{
		printf("old root\n");
		printf("lba: %d\n", parent->lba);
		for(i=0; i < parent->nkeys; i++){
			printf("parent->keys[%d]: %s\n", i, parent->keys[i]);
			printf("parent->lbas[%d]: %d\n", i, parent->lbas[i]);
		}
		//printf("parent->keys[%d]: %s\n", i, parent->keys[i]);
		printf("parent->lbas[%d]: %d\n", i, parent->lbas[i]);
	}


	printf("\n");
	printf("original\n");
	printf("lba: %d\n", original->lba);
	for(i=0; i < original->nkeys; i++){
		printf("original->keys[%d]: %s\n", i, original->keys[i]);
		printf("original->lbas[%d]: %d\n", i, original->lbas[i]);
	}
	printf("original->keys[%d]: %s\n", i, original->keys[i]);
	printf("original->lbas[%d]: %d\n", i, original->lbas[i]);
	printf("\n");
	printf("new_node\n");
	printf("lba: %d\n", new_node->lba);
	for(i=0; i < new_node->nkeys; i++){
		printf("new_node->keys[%d]: %s\n", i, new_node->keys[i]);
		printf("new_node->lbas[%d]: %d\n", i, new_node->lbas[i]);
	}
	printf("new_node->keys[%d]: %s\n", i, new_node->keys[i]);
	printf("new_node->lbas[%d]: %d\n", i, new_node->lbas[i]);
	printf("\n\n");

	//printf("parent->nkeys: %d\n", parent->nkeys);
	if(parent->nkeys > bt->max_key){
		if(split_node(bt, parent, new_first_free_block) == 0){
			return 0;
		} else{
			parent->flush = 1;
			new_node->flush = 1;
			original->flush = 1;
			bt->first_free_block += blocks_needed;
			/*If its parent split then the parent cannot be freed here */
			printf("another split flush new\n");
			free_and_flush(bt, new_node);
			printf("another split flush original\n");
			free_and_flush(bt, original);

			return 1;
		}
	} else{
		parent->flush = 1;
		new_node->flush = 1;
		original->flush = 1;
		bt->first_free_block += blocks_needed;
		if(new_root == 1){
			bt->root_lba = parent->lba;
			printf("parent->lba: %d\n", parent->lba);
		}
		//printf("blocks_needed: %d\n", blocks_needed);
		printf("flush new node-------\n");
		free_and_flush(bt, new_node);
		printf("flush original-------\n");
		free_and_flush(bt, original);
		printf("flush parent---------\n");
		free_and_flush(bt, parent);
		return 1;
	}

	return 0;
}

/* reads a tree_node from a disk */
void *read_tree_node(void *b_tree, unsigned long lba)
{
	B_Tree *bt;
	Tree_Node *tn;
	int r;
	char bytes[JDISK_SECTOR_SIZE];
	int i;

	bt = (B_Tree *)b_tree;

	printf("\nRead Tree Start\n");
	Tree_Node *tmp;
	tmp = bt->free_list;
	//while(tmp != NULL){
	//	printf("tmp->lba: %d\n", tmp->lba);
	//	tmp = tmp->ptr;
	//}

	/* If there is a free node then don't malloc a new one */
	if(bt->free_list != NULL){
		printf("there is a free node\n");
		tn = bt->free_list;
		bt->free_list = tn->ptr;
		//printf("tn->ptr: %s\n", tn->ptr);
		printf("tn->lba: %d\n", tn->lba);
		//printf("\n");
		//printf("tn->key: %s\n", tn->keys[0]);
	}

	/* Otherwise malloc a new tree node */
	else{
		//printf("malloc new tree node\n");
		tn = (Tree_Node *)malloc(sizeof(Tree_Node));	
		tn->keys = (unsigned char **)malloc(sizeof(char *)*(bt->max_key+1));
		//tn->keys = malloc((bt->max_key+1) * bt->key_size);
		tn->lbas = (unsigned int *)malloc(sizeof(unsigned int) * (bt->max_key+2));
		for(i=0; i < (bt->max_key+1); i++){
			tn->keys[i] = (unsigned char *) &tn->bytes+2+(i*bt->key_size);
			//tn->keys[i] = &tn->bytes[2+(bt->key_size * i)];
		}
	}

	r = jdisk_read(bt->disk, lba, tn->bytes);
	memcpy(&tn->nkeys, tn->bytes+1, 1);
	tn->flush = 0;
	//for(i=0; i < (bt->max_key+1); i++){
	//	tn->keys[i] = &tn->bytes[2+(bt->key_size * i)];
	//}
	memcpy((unsigned char*)&tn->internal, tn->bytes, 1);
	memcpy(tn->lbas, tn->bytes+(JDISK_SECTOR_SIZE - bt->lbas_per_block),bt->lbas_per_block);
	tn->parent = NULL;
	tn->parent_index = -1;
	tn->ptr = NULL;

	//printf("tn->lbas+0: %d\n", *(tn->lbas));
	//printf("tn->lbas+1: %d\n", *(tn->lbas+1));
	printf("\nRead Tree End\n");

	return tn;
}


void *b_tree_create(char *filename, long size, int key_size)
{
	/*
	   printf("Inside create\n");	
	   printf("file_size: %d\n", size);
	   printf("key_size: %d\n", key_size);
	   printf("filename: %s\n", filename);
	   printf("JDISK_SECTOR_SIZE: %d\n", JDISK_SECTOR_SIZE);
	   */
	//FILE *disk;
	B_Tree *bt;
	Tree_Node *root;
	void *disk = NULL;
	char first_bytes[JDISK_SECTOR_SIZE];
	int r, i;

	disk = jdisk_create(filename, size);
	if(disk == NULL){
		printf("attach dont create!\n");
		exit(1);
	}
	bt = (B_Tree *)malloc(sizeof(B_Tree));

	/* Setup b_tree struct */
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
	//root->keys = malloc((bt->max_key+1) * bt->key_size);
	root->lbas = (unsigned int *)malloc(sizeof(unsigned int) * (bt->max_key+2));

	for(i=0; i < (bt->max_key+1); i++){
		root->keys[i] = (unsigned char *) &root->bytes+2+(i*bt->key_size);
		//root->keys[i] = &root->bytes[2+(bt->key_size * i)];
	}

	root->internal = 0;
	root->lba = 1;
	root->nkeys = 0;
	root->flush = 1;
	root->parent = NULL;
	root->parent_index = -1;
	root->ptr = NULL;
	printf("create flush root\n");
	free_and_flush(bt, root);

	//printf("done with create\n");

	return bt;
}

void *b_tree_attach(char *filename)
{
	B_Tree *bt;
	void *disk;
	int r;
	char buf[JDISK_SECTOR_SIZE];
	size_t offset = 0;

	/* malloc the B_Tre */
	bt = (B_Tree *)malloc(sizeof(B_Tree));

	/* Attach to disk */
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

	bt->size = jdisk_size(disk);
	bt->num_lbas = bt->size/JDISK_SECTOR_SIZE;
	bt->max_key = (JDISK_SECTOR_SIZE - 6) / (bt->key_size + 4);
	bt->lbas_per_block = (bt->max_key + 1)*4;
	bt->free_list = NULL;
	bt->tmp_e = NULL;
	bt->tmp_e_index = -1;
	bt->flush = 0;

	/*
	   printf("bt->key_size: %d\n", bt->key_size);
	   printf("bt->root_lba: %d\n", bt->root_lba);
	   printf("bt->first_free_block: %d\n", bt->first_free_block);
	   printf("bt->size: %d\n", bt->size);
	   printf("bt->num_lbas: %d\n", bt->num_lbas);
	   printf("bt->keys_per_block: %d\n", bt->max_key);
	   printf("bt->lbas_per_block: %d\n", bt->lbas_per_block);

	   if(r == 0){
	   printf("have the first sector\n");
	   }
	   */

	return bt;

}

unsigned int b_tree_insert(void *b_tree, void *key, void *record)
{

	B_Tree *bt;
	Tree_Node *tn;

	unsigned long next_free_block;
	unsigned long og_free_block;
	unsigned int cur_last_lba;
	unsigned int lba_to_shift;
	unsigned long current_keys_size;
	unsigned long current_lbas_size;
	int can_split = 0;
	int find_lba = -2;
	int i;
	unsigned char **current_keys;
	unsigned int *current_lbas;

	printf("\ninside insert\n");
	//printf("key: %s\n", key);
	//printf("record: %s\n", record);

	bt = (B_Tree *)b_tree;
	next_free_block = bt->first_free_block+1;
	og_free_block = bt->first_free_block;

	/* Need to search for the key first */
	find_lba = b_tree_find(b_tree, key);

	/* Not in tree need to insert */
	if(find_lba == -1){
		/*
		   printf("not in tree\n");
		   printf("index should have gone: %d\n", bt->tmp_e_index);
		   printf("tmp_e->keys[0]: %s\n", bt->tmp_e->keys[0]);
		   */
		tn = bt->tmp_e;
		cur_last_lba = tn->lbas[tn->nkeys];

		/* check if enough space is available */
		if(next_free_block > bt->num_lbas){
			printf("cant insert not enough space\n");
			return 0;
		} else if(tn->nkeys == bt->max_key){

			//will need space for new node and jdisk sector for record
			if(tn->parent != NULL && next_free_block+2 > bt->num_lbas){
				printf("non root cant insert\n");
				return 0;
			} else if(tn->parent == NULL && next_free_block+3 > bt->num_lbas){
				printf("not enough space for root node\n");
				return 0;
			}
		}

		/* need to insert the key and record into the node */

		/* if it can go at end */
		if(bt->tmp_e_index == tn->nkeys){
			printf("insert at end of node\n");	
			//add the key to the end
			memcpy(tn->bytes+2+(bt->key_size * bt->tmp_e_index), key, bt->key_size);

			//have to swap the cur_last_lba to the new index
			//cur_last_lba = tn->lbas[tn->nkeys];
			/*
			   printf("last lba val: %d\n", tn->lbas[tn->nkeys]);
			   printf("bt->tmp_e_index: %d\n", bt->tmp_e_index);
			   */
			memcpy(tn->lbas+(bt->tmp_e_index), (unsigned int*)&bt->first_free_block, sizeof(unsigned int));
			memcpy(tn->lbas+(bt->tmp_e_index+1), (unsigned int*)&cur_last_lba, sizeof(unsigned int));
			tn->nkeys++;
			/*
			   printf("last lba val: %d\n", tn->lbas[tn->nkeys]);
			   printf("new lba: %d\n", tn->lbas[tn->nkeys-1]);
			   printf("key: %s\n", tn->keys[tn->nkeys-1]);
			   printf("lba: %d\n", tn->lba);
			   */
		}

		/* values must be shifted to the right to insert */
		else{
			printf("insert shift values right\n");
			current_keys = malloc((bt->max_key) * bt->key_size);
			current_lbas = (unsigned int *)malloc(sizeof(unsigned int) * (bt->max_key+1));
			unsigned char keys[tn->nkeys*bt->key_size];

			current_keys_size = bt->key_size * (tn->nkeys - bt->tmp_e_index);
			current_lbas_size = sizeof(unsigned int) * (tn->nkeys+1 - bt->tmp_e_index);
			memcpy(keys, tn->bytes+(2+(bt->key_size*bt->tmp_e_index)), current_keys_size);
			memcpy(current_lbas, tn->lbas+(bt->tmp_e_index), current_lbas_size);

			memcpy(tn->bytes+(2+(bt->tmp_e_index*bt->key_size)), key, bt->key_size);
			memcpy(tn->lbas+(bt->tmp_e_index), (unsigned int*)&bt->first_free_block, sizeof(unsigned int));
			memcpy(tn->bytes+(2+((bt->tmp_e_index+1)*bt->key_size)), keys, current_keys_size);
			memcpy(tn->lbas+(bt->tmp_e_index+1), current_lbas, current_lbas_size);

			tn->nkeys++;	

			free(current_keys);
			free(current_lbas);
		}

		printf("tn->nkeys: %d\n", tn->nkeys);
		printf("tn->lba: %d\n", tn->lba);
		if(tn->parent != NULL) printf("tn->parent->lba: %d\n", tn->parent->lba); 
		if(tn->nkeys > bt->max_key){
			//printf("have to split node\n");
			can_split = split_node(bt, tn, bt->first_free_block);

			if(can_split == 1){
				printf("split not write record\n");
				jdisk_write(bt->disk, og_free_block, record);
				bt->flush = 1;
				find_lba = og_free_block;

				printf("flush sector 0 with NULL tn\n");
				free_and_flush(bt, NULL);
			} else{
				printf("insert cannot split\n");
				return 0;
			}
			//tn->nkeys++;bt->first_free_block++;

		}else{
			printf("writing record no split\n");
			jdisk_write(bt->disk, bt->first_free_block, record);
			tn->flush = 1;
			find_lba = bt->first_free_block;
			bt->first_free_block++;
			bt->flush = 1;
			printf("flush no split insert node & 0\n");
			free_and_flush(bt, tn);
		}
		/*	
		printf("tn->nkeys: %d\n", tn->nkeys);
		printf("bt->first_free_block: %d\n", bt->first_free_block);
		printf("no split just insert\n");
		printf("free and flush up\n");
		Tree_Node *flushing = tn;
		while(flushing != NULL){
			free_and_flush(bt, flushing);
			flushing = flushing->parent;
		}
		*/
	}

	/* Key in tree need to replace record */
	else{
		printf("need to replace record\n");
		printf("lba: %d\n", find_lba);	
		jdisk_write(bt->disk, find_lba, record);
	}

	return find_lba;

}

unsigned int b_tree_find(void *b_tree, void *key)
{
	B_Tree *bt;
	Tree_Node *cur = NULL;
	Tree_Node *prev = NULL;
	Tree_Node *tn;
	Tree_Node *root;
	size_t key_len;
	int found_internal = 0;
	int r, i;
	int lba_found = -2;
	int parent_index = -1;
	int tmp_e_index = -1;
	unsigned int lba = -1;

	bt = (B_Tree *)b_tree;
	//memcpy(k, key, bt->key_size);
	printf("\nfind start\n");
	//read root
	//printf("bt->root_lba: %d\n", bt->root_lba);
	cur = read_tree_node(bt, bt->root_lba);
	root = cur;
	//printf("\ninsert find end\n");
	cur->parent = NULL;
	key_len = sizeof(key);
	printf("n->keys: %d\n", cur->nkeys);
	printf("key_len: %d\n", key_len);
	//printf("key: %s\n", key);
	//printf("root lba: %d\n", cur->lba);

	while(cur->internal == 1 ||( cur->internal == 0 && lba_found == -2)){
		//printf("inside while\n");
		for(i=0; i < cur->nkeys; i++){

			//check if value is less than or equal
			r = memcmp(cur->keys[i], key, bt->key_size);
			printf("cur->keys[%d]: %s\n", i, cur->keys[i]);
			//printf("cur->lba: %d\n", cur->lba);
			printf("r: %d\n", r);
			if(r == 0){
				if(cur->internal == 1){
					found_internal = 1;
					printf("found internal\n");
					//go left
					prev = cur;
					parent_index = i;
					lba = *(cur->lbas+i);
					cur = read_tree_node(bt, *(cur->lbas+i));
					cur->parent = prev;
					cur->parent_index = parent_index;
					cur->lba = lba;
					printf("lba: %d\n", lba);
					//printf("pi: %d\n", parent_index);
					break;
				} else{
					//printf("found external\n");
					lba_found = *(cur->lbas+i);
					//cur->lba = *(cur->lbas+i);
					break;
					//found val
				}
			} 
			else if(found_internal == 1){
				//go far right not external node
				if(cur->internal == 1){
					printf("continue found right\n");	
					prev = cur;
					parent_index = cur->nkeys;
					lba = *(cur->lbas+(cur->nkeys));
					cur = read_tree_node(bt, *(cur->lbas+(cur->nkeys)));
					cur->parent = prev;
					cur->parent_index = parent_index;
					cur->lba = lba;
					printf("lba: %d\n", lba);
					break;
				}

				//now in external node
				//return lba
				else{
					printf("found earlier\n");
					lba_found = *(cur->lbas+(cur->nkeys));
					cur->parent_index = parent_index;
					//cur->lba = *(cur->lbas+(cur->nkeys));
					break;

				}

			}
			else if(r > 0){
				if(cur->internal == 1){
					//go left
					printf("go left of key\n");
					prev = cur;
					printf("prev->lba: %d\n", prev->lba);
					parent_index = i;
					lba = *(cur->lbas+i);
					//printf("read: %d\n", *(cur->lbas+i));
					cur = read_tree_node(bt, *(cur->lbas+i));
					//printf("cur->nkeys: %d\n", cur->nkeys);
					//printf("cur->keys[0]: %s\n", cur->keys[0]);
					cur->parent = prev;
					printf("lba: %d\n", lba);
					printf("prev->lba: %d\n", prev->lba);
					cur->parent_index = parent_index;
					cur->lba = lba;
					break;
				}

				else{
					//printf("key not found left\n");
					parent_index = i;
					lba_found = -1;
					tmp_e_index = i;
					//cur->lba = *(cur->lbas+i);
					break;
				}
			}
			else if(i == cur->nkeys - 1){
				/* Only have to check right 
				 * if its the last key of the internal node */
				if(r < 0 && cur->internal == 1){
					//printf("key: %s\n", cur->keys[0]);
					//printf("cur->internal: %d\n", cur->internal);
					//printf("HERE\n");
					//printf("i: %d\n", i);
					//go right
					prev = cur;
					parent_index = i+1;
					lba = *(cur->lbas+i+1);
					//printf("lba: %d\n", lba);
					cur = read_tree_node(bt, *(cur->lbas+i+1));
					cur->parent = prev;
					cur->parent_index = parent_index;
					cur->lba = lba;
					break;

				}

				//the last lba to the right is
				//only set if it has a parents value
				//if this is hit the key is not int the tree
				else{
					printf("key not found---------------\n");
					//printf("cur->lba: %d\n", cur->lba);
					printf("lba: %d\n", lba);
					parent_index = i+1;
					lba_found = -1;
					tmp_e_index = i + 1;
					//cur->lba = *(cur->lbas+i+1);
					break;
				}
				//}
		}
	}

	//this should only happen when a new tree is made
	//and the root is empty
	if(cur->nkeys == 0){
		printf("empty node no keys\n");
		tmp_e_index = 0;
		lba_found = -1;
	}

	printf("\nFind End\n");

}

root->lba = bt->root_lba;

if(lba_found == -1){
	printf("cur->lba: %d\n", cur->lba);
	if( cur->parent != NULL){
		printf("cur->parent->lba: %d\n", cur->parent->lba);
	}
	//printf("key not found\n");
	printf("index should have gone: %d\n", tmp_e_index);
	//printf("cur->keys: %s\n", cur->keys[0]);
	bt->tmp_e = cur;
	bt->tmp_e_index = tmp_e_index;
	//printf("tmp_e->keys[0]: %s\n", bt->tmp_e->keys[0]);

}

else{
	//free the searched list by following the current tree_nodes parents
		printf("flush find\n");
		Tree_Node *flushing = cur;
		while(flushing != NULL){
			free_and_flush(bt, flushing);
			flushing = flushing->parent;
		}
	//free_and_flush(bt, cur);
}




printf("lba return: %d\n", lba_found);
/*
   printf("cur->nkeys: %d\n", cur->nkeys);
   printf("cur->flush: %d\n", cur->flush);
   printf("cur->internal: %d\n", cur->internal);
   printf("cur->keys: %s\n", cur->keys[0]);
   */

return lba_found;
}


void *b_tree_disk(void *b_tree)
{
	B_Tree *bt;
	bt = (B_Tree *)b_tree;
	return bt->disk;
}

int b_tree_key_size(void *b_tree)
{
	B_Tree *bt;
	bt = (B_Tree *)b_tree;
	return bt->key_size;	
}

void b_tree_print_tree(void *b_tree)
{
	printf("This does nothing yet\n");	
}

