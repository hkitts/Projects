# B-Tree
This b-tree is attached to a simulated disk.

## Available commands
- Create a b-tree or attach to to preexisting b-tree.
	-./b_tree "jdisk" CREATE "size of jdisk (multiple of 1024)" "key size"
		- ./b_tree tree.jdisk CREATE 20480 200     (Create)
		- ./b_tree tree.jdisk                      (Attach)
- Insert a key and value into the b-tree. If the key is already in the tree the value is replaced.
	- I "key" "val"
		- I Taylor Sly
- Find a a value given the key in the b-tree
	- F "key"
		- F Taylor
- Print the b-tree. Prints breadth-first
	- P
	
