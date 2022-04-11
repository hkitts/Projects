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
	Clear();
/*
	cout << "size                   : " << size << endl;
	cout << "t.size                 : " << t.size << endl;
	cout << "sentinel->right->key   : " << sentinel->right->key << endl;
	cout << "t.sentinel->right->key : " << t.sentinel->right->key << endl;
*/
	size = t.size;
	sentinel->right = AVLTree::recursive_postorder_copy(t.sentinel->right);
	sentinel->right->parent = sentinel;
//	cout << "sentinel->right->right->parent->key   : " << sentinel->right->right->parent->key << endl;
//	cout << "sentinel->right->key           : " << sentinel->right->key << endl;
//	cout << "t.sentinel->right->key         : " << t.sentinel->right->key << endl;

	return *this;
}

bool imbalance(const AVLNode *n)
{
	int height_difference;

	height_difference = abs((int)n->left->height - (int)n->right->height);

	if(height_difference >= 2) return true;

	return false;
}


void rotate(AVLNode *n)
{
	AVLNode *parent;
	AVLNode *grandparent;
	//AVLNode *middle;
	AVLNode *tmp;
	string side;

	if(n->parent->height == 0){
		return;
	}

	parent = n->parent;
	grandparent = parent->parent;
	if(n->key == parent->right->key){
		//middle = parent->left;
		side = "left";
	}
	else{
		//middle = parent->right;
		side = "right";
	}

	if(grandparent->height == 0) grandparent->right = n;
	else if(grandparent->right->key == parent->key) grandparent->right = n;
	else {
		//tmp = grandparent->left;
		grandparent->left = n;
		//grandparent->right = tmp;
	}

	/*
	cout << "n           : " << n->key << endl;
	cout << "parent      : " << parent->key << endl;
	cout << "grandparetn : " << grandparent->key << endl;
	*/
	
	n->parent = grandparent;

	if(side == "left"){
		tmp = n->left;
		n->left = parent;
		parent->parent = n;
		parent->right = tmp;
		tmp->parent = parent;
	}
	if(side == "right"){
		tmp = n->right;
		n->right = parent;
		parent->parent = n;
		parent->left = tmp;
		tmp->parent = parent;
	}

}

void fix_height(AVLNode *n)
{
	size_t max;

	if(n->right->height >= n->left->height){
		max = n->right->height;
	}
	else max = n->left->height;

	if(n->height != max+1){
		n->height = max+1;
	}

}

void fix_imbalance(AVLNode *n)
{
	AVLNode *tmp;


	if(n->left->height > n->right->height){
		//Zig-Zig left
		if(n->right->height == 0 && n->left->height != 0){
			//cout << "Zig-Zig left 1\n";
			rotate(n->left);
			while(n->height != 0){
				fix_height(n);
				n =n->parent;
			}
		}
		else if(n->left->left->height >= n->left->right->height){
			//cout << "Zig-Zig left 2\n";
			rotate(n->left);
			while(n->height != 0){
				fix_height(n);
				n =n->parent;
			}

		}

	/*
	
		//Zig-Zag left right
		else if(n->right->height == 0 && n->left->left->height != 0){
			//cout << "Zig-Zag left right 1\n";
			//cout << "ROTATE ABOUT : " << n->left->right->key << endl;
			rotate(n->left->right);
			//cout << n->key << endl;
			tmp = n->left;
			while(tmp->height != 0){
				fix_height(tmp);
				tmp = tmp->parent;
			}
			rotate(n->left);

			tmp = n->parent->left;
			while(tmp->height != 0){
				fix_height(tmp);
				tmp = tmp->parent;
			}
			tmp = n;
			while(tmp->height != 0){
				fix_height(tmp);
				tmp = tmp->parent;
			}


	
		}

	*/
		
		else if (n->left->right->height >= n->left->left->height){
				//cout << "Zig-Zag left right 2\n";
				//cout << "ROTATE ABOUT : " << n->left->right->key << endl;
			rotate(n->left->right);
			//	cout << n->key << endl;
			tmp = n->left;
			while(tmp->height != 0){
				fix_height(tmp);
				tmp = tmp->parent;
			}
			rotate(n->left);

			tmp = n->parent->left;
			while(tmp->height != 0){
				fix_height(tmp);
				tmp = tmp->parent;
			}
			tmp = n;
			while(tmp->height != 0){
				fix_height(tmp);
				tmp = tmp->parent;
			}

		}

	}
	//Zig-Zig right
	else if(n->right->height > n->left->height){
		if (n->left->height == 0 && n->left->height != 0){
			//cout << "Zig-Zig right 1\n";
			//cout << n->right->key << endl;
			rotate(n->right);
			//cout << n->key << endl;
			while(n->height != 0){
				fix_height(n);
				n =n->parent;
			}
		}
		else if(n->right->right->height >= n->right->left->height){
			//cout << "Zig-Zig right 2\n";
			//cout << n->right->key << endl;
			rotate(n->right);
			while(n->height != 0){
				fix_height(n);
				n = n->parent;
			}
			return;
		}
/*
		//Zig-Zag right left
		else if(n->left->height == 0 && n->right->left->height !=0){
			cout << "Zig-Zag right left 1\n";
			rotate(n->right->left);
			tmp = n->right;
			while(tmp->height != 0){
				fix_height(tmp);
				tmp = tmp->parent;
			}
			rotate(n->right);
			tmp = n->parent->right;
			while(tmp->height != 0){
				fix_height(tmp);
				tmp = tmp->parent;
			}
			tmp = n;
			while(tmp->height != 0){
				fix_height(tmp);
				tmp = tmp->parent;
			}
		}

*/
		else if(n->right->left->height >= n->right->right->height){
				//cout << "Zig-Zag right left 2\n";
				//cout << "ROTATE ABOUT : " << n->right->left->key << endl;
			rotate(n->right->left);
			//cout << "next rotate about: " << n->right->key << endl;
			tmp = n->right;
			while(tmp->height != 0){
				fix_height(tmp);
				tmp = tmp->parent;
			}
			//	cout << "here\n";
			rotate(n->right);
			//	cout << n->parent->right->key << endl;

			tmp = n->parent->right;
			while(tmp->height != 0){
				fix_height(tmp);
				tmp = tmp->parent;
			}
			tmp = n;
			while(tmp->height != 0){
				fix_height(tmp);
				tmp = tmp->parent;
			}
		}
	}
	else cout << "imbalance error" << endl;
}


/* I simply took Insert and Delete from their binary search tree
   implementations.  They aren't correct for AVL trees, but they are
   good starting points.  */

bool AVLTree::Insert(const string &key, void *val)
{
	AVLNode *parent;
	AVLNode *n;
	AVLNode *h;
	size_t max;

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

	/* Finds maximum of the childs parent
	 * Checks if the parents height is at least (1+ the height of its greatest child)
	 * If it isn't it adds the new height of the parent is (1+ the height of its greatest child)
	 * It keeps going up and checking until it hits the sentinel */

	h = n->parent;
	while(h != sentinel){
		if(h->right->height >= h->left->height){
			max = h->right->height;
		}
		else max = h->left->height;

		if(h->height < max+1){
			h->height = max+1;
		}
		h = h->parent;

	}

	h = n->parent;
	//cout << h->key << endl;
	while(h != sentinel){
		if(imbalance(h)){
			//cout << "found imbalance at " << h->key << endl;
			fix_imbalance(h);
			//size++;
			//return true;
		}
		h = h->parent;
		//cout << h->key << endl;
	}




	/* Increment the size */

	size++;
	return true;
}

bool AVLTree::Delete(const string &key)
{
	AVLNode *n, *parent, *mlc, *tmp;
	string tmpkey;
	void *tmpval;
	size_t max;

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

		tmp = parent;
		while(tmp != sentinel){
			if(tmp->right->height >= tmp->left->height){
				max = tmp->right->height;
			}
			else max = tmp->left->height;

			if(tmp->height != max+1){
				tmp->height = max+1;
			}
			tmp = tmp->parent;
		}

		tmp = parent;
		//cout << tmp->key << endl;
		while(tmp != sentinel){
			if(imbalance(tmp)){
				//cout << "imbalance at : " << tmp->key << endl;
				fix_imbalance(tmp);
			}
			tmp = tmp->parent;
		}


	return true;
}

/* You need to write these two.  You can lift them verbatim from your
   binary search tree lab. */

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

size_t AVLTree::Height() const
{	
	return sentinel->right->height;
}

/* You need to write this to help you with the assignment overload.
   It makes a copy of the subtree rooted by n.  That subtree is part
   of a different tree -- the copy will be part of the tree that
   is calling the method. */

AVLNode *AVLTree::recursive_postorder_copy(const AVLNode *n) const
{
	AVLNode *c;

	if(n->height == 0) return sentinel;

	//cout << "N->key: "<< n->key << endl;

	c = new AVLNode;
	c->key = n->key;
	c->val = n->val;
	//cout << "c->key: " << c->key << endl;
	c->height = n->height;

	//cout << "n->left->key: " << n->left->key << endl;
	c->left = AVLTree::recursive_postorder_copy(n->left);
	c->left->parent = c;
	//cout << "c->left->key: " << c->left->key << endl << endl;

	//cout << "n->right->key: " << n->right->key << endl;
	c->right = AVLTree::recursive_postorder_copy(n->right);
	c->right->parent = c;
	//cout << "c->right->key: " << c->right->key << endl<< endl;

	return c;
}
