/* Hunter Kitts
 * LabB avltree_lab.cpp
 */ 

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include "avltree.hpp"
using namespace std;
using CS140::AVLTree;
using CS140::AVLNode;


AVLTree& AVLTree::operator= (const AVLTree &t)        
{
	Clear();					//clears the AVLTree just in case its not empty
	size = t.size;				//sets the size to match t's size
	//recursivly copys t to the other AVLTree and then the trees parent is set as the sentinel
	sentinel->right = AVLTree::recursive_postorder_copy(t.sentinel->right);		
	sentinel->right->parent = sentinel;

	return *this;
}

bool imbalance(const AVLNode *n)
{
	int height_difference;	//used to check height difference of n's childs

	//if n's childs are greater than 2 it returns true otherwise returns false
	height_difference = abs((int)n->left->height - (int)n->right->height);
	if(height_difference >= 2) return true;
	return false;
}


void rotate(AVLNode *n)
{
	//n will become the new parent

	AVLNode *parent;				//the parent of n's node, will be switched with n
	AVLNode *grandparent;			//the grandparent of n's node (parent of parent), not switch just saved to make sure the left or right pointer is correct
	AVLNode *middle;				//either left or right of n, to be moved to left or right of the old parent node, 

	//the parent can not be the sentinel, if it does it returns before the parent is set
	if(n->parent->height == 0){
		return;
	}

	//both the parent and grandparent are set
	parent = n->parent;
	grandparent = parent->parent;
	
	//this determins from which side the new parent will branch off
	//it should be the same side as the original parent
	//sets all pointers correctly to the grandparent after rotation
	if(grandparent->height == 0) grandparent->right = n;
	else if(grandparent->right->key == parent->key) grandparent->right = n;
	else grandparent->left = n;
	n->parent = grandparent;

	//n's location determins where all the other pointers go
	//in total n and parent will swap
	//the middle will swap its location from left to right 
	//the other child n will not move its pointers stay
	//the other child of the parent will not change
	//the only thing that happens with them is their parents move, thus moving them on the tree
	if(n->key == parent->right->key){
		middle = n->left;
		n->left = parent;
		parent->parent = n;
		parent->right = middle;
		middle->parent = parent;
	}
	else{
		middle = n->right;
		n->right = parent;
		parent->parent = n;
		parent->left = middle;
		middle->parent = parent;
	}
}

void fix_height(AVLNode *n)
{
	size_t max;						//used to find the maximum of the childern

	//goes up as until it hits the sentinel and checks the height of n's childern
	while(n->height != 0){
		//if the height of n is not (the height of the greatest child +1) then it is changed to be that
		if(n->right->height >= n->left->height) max = n->right->height;
		else max = n->left->height;
		if(n->height != max+1) n->height = max+1;
		n = n->parent;
	}
}

void fix_imbalance(AVLNode *n)
{
	AVLNode *tmp;			//used to fix the height after the rotation
							//the nodes to fix height depends on the rotation(s) dirrection

	//every statement is looking for the child node that is the greatest after n
	//it follows the pointers down 2 times
	//if a zig-zig is found 1 rotation and fix height is done
	//if a zig-zag is found 2 rotations are done and 2 fix heights are done
	
	//if the first pointer off n to the left has greater height than the right
	if(n->left->height > n->right->height){
		//zig-zig
		//if the max height follows left left
		if(n->left->left->height >= n->left->right->height){
			rotate(n->left);
			fix_height(n);
		}
		//zig-zag
		//if the max height follows left right
		else if (n->left->right->height >= n->left->left->height){
			//1st rotation and fix height
			rotate(n->left->right);
			tmp = n->left;
			fix_height(tmp);
			//2nd rotation and fix height
			rotate(n->left);
			tmp = n->parent->left;
			fix_height(tmp);
		}
	}

	//if the first pointer off n to the right has greater height than the left
	else if(n->right->height > n->left->height){
		if(n->right->right->height >= n->right->left->height){
			rotate(n->right);
			fix_height(n);
		}
		//zig-zag
		//if the max height follows right left
		else if(n->right->left->height >= n->right->right->height){
			//1st rotation and fix height
			rotate(n->right->left);
			tmp = n->right;
			fix_height(tmp);
			//2nd rotation and fix height
			rotate(n->right);
			tmp = n->parent->right;
			fix_height(tmp);
		}
	}
	//this was not necessary but I used this to error test
	else cout << "Imbalance error" << endl;
}


/* I simply took Insert and Delete from their binary search tree
   implementations.  They aren't correct for AVL trees, but they are
   good starting points.  */

bool AVLTree::Insert(const string &key, void *val)
{
	AVLNode *parent;
	AVLNode *n;
	AVLNode *h;			//used to fix check the heights and fix the imbalance if neccesary

	parent = sentinel;
	n = sentinel->right;

	/* Find where the key should go.  If you find the key, return false. */

	while (n != sentinel) {
		if (n->key == key) return false;
		parent = n;
		n = (key < n->key) ? n->left : n->right;
	}

	/* At this point, parent is the node that will be the parent of the new node.
	   Create the new node, and hook it in. */

	n = new AVLNode;
	n->key = key;
	n->val = val;
	n->parent = parent;
	n->height = 1;
	n->left = sentinel;
	n->right = sentinel;

	/* Use the correct pointer in the parent to point to the new node. */

	if (parent == sentinel) {
		sentinel->right = n;
	} else if (key < parent->key) {
		parent->left = n;
	} else {
		parent->right = n;
	}

	//fixes the height after a node is inserted

	fix_height(n->parent);

	/* Looks through nodes going up to check if their is an imbalance after the height change
	 * If an imbalance is found it is fixed through the rotation(s) in fix imbalance
	 */
	
	h = n->parent;
	while(h != sentinel){
		if(imbalance(h)) fix_imbalance(h);
		h = h->parent;
	}

	/* Increment the size */

	size++;
	return true;
}

bool AVLTree::Delete(const string &key)
{
	AVLNode *n, *parent, *mlc, *tmp;				//tmp is used to fix check the heights and fix the imbalance if neccesary
	string tmpkey;
	void *tmpval;

	/* Try to find the key -- if you can't return false. */

	n = sentinel->right;
	while (n != sentinel && key != n->key) {
		n = (key < n->key) ? n->left : n->right;
	}
	if (n == sentinel) return false;

	/* We go through the three cases for deletion, although it's a little
	   different from the canonical explanation. */

	parent = n->parent;

	/* Case 1 - I have no left child.  Replace me with my right child.
	   Note that this handles the case of having no children, too. */

	if (n->left == sentinel) {
		if (n == parent->left) {
			parent->left = n->right;
		} else {
			parent->right = n->right;
		}
		if (n->right != sentinel) n->right->parent = parent;

		delete n;
		size--;

		/* Case 2 - I have no right child.  Replace me with my left child. */

	} else if (n->right == sentinel) {
		if (n == parent->left) {
			parent->left = n->left;
		} else {
			parent->right = n->left;
		}
		n->left->parent = parent;
		delete n;
		size--;

		/* If I have two children, then find the node "before" me in the tree.
		   That node will have no right child, so I can recursively delete it.
		   When I'm done, I'll replace the key and val of n with the key and
		   val of the deleted node.  You'll note that the recursive call 
		   updates the size, so you don't have to do it here. */

	} else {
		for (mlc = n->left; mlc->right != sentinel; mlc = mlc->right) ;
		tmpkey = mlc->key;
		tmpval = mlc->val;
		Delete(tmpkey);
		n->key = tmpkey;
		n->val = tmpval;
		return true;
	}

	//fixes the height after a node is deleted
	fix_height(parent);

	/* Looks through nodes going up to check if their is an imbalance after the height change
	 * If an imbalance is found it is fixed through the rotation(s) in fix imbalance
	 */
	tmp = parent;
	while(tmp != sentinel){
		if(imbalance(tmp)) fix_imbalance(tmp);
		tmp = tmp->parent;
	}
	
	return true;
}

/* These are lifted verbatim from the binary search tree lab. */

//recursivly creates vector of keys from the AVLTree
vector <string> AVLTree::Ordered_Keys() const
{
	vector <string> rv;
	make_key_vector(sentinel->right, rv);
	return rv;
}

void AVLTree::make_key_vector(const AVLNode *n, vector<string> &v) const
{
	if( n == sentinel) return;
	make_key_vector(n->left, v);
	v.push_back(n->key);
	make_key_vector(n->right, v);
}

//returns maximum height which is the height of the node to the right of the sentinel, the root

size_t AVLTree::Height() const
{	
	return sentinel->right->height;
}

/* Helper for the recursive call
 * n is the root node of the tree that is being passed in
 */


AVLNode *AVLTree::recursive_postorder_copy(const AVLNode *n) const
{
	//this will create a total seperate tree that is the same
	//the parent to the root node is set in the assignment overload

	AVLNode *c;			//nodes that are a copy of n pointers and all

	//base case
	if(n->height == 0) return sentinel;

	//creates the new node for the copy and sets all values except the pointers
	c = new AVLNode;
	c->key = n->key;
	c->val = n->val;
	c->height = n->height;

	//recusivlly make calls to set the left pointer
	c->left = AVLTree::recursive_postorder_copy(n->left);
	c->left->parent = c;

	//recursivlly make calls to set the right pointer
	c->right = AVLTree::recursive_postorder_copy(n->right);
	c->right->parent = c;

	return c;
}

//To all of the TA's. You all have helped me so much and I feel I will never be albe to pay it back.
//From the bottom of my heart, thank you.
