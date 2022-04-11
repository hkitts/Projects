/* Hunter Kitts
 * BoardFolding cs494 lab0 */
#include <iostream>
#include <vector>
#include <string.h>
#include <map>
#include <algorithm>
using namespace std;

class BoardFolding
{
	public:
		int howMany(int N, int M, vector <string> compressedPaper);

	protected:
		vector <int> rows;		//vector of the rows turned into symbolic numbers
		vector <int> srows;		//starting positions of rows
		vector <int> erows;		//ending positions of rows
		vector <int> columns;		//vector of the columns turned into symbolic numbers
		vector <int> scolumns;		//starting positions of columns
		vector <int> ecolumns;		//ending positions of columns

		unsigned int tonumber(const char &c);		   	    //uncompress compressed input
		int combinations();					    //gets return value, all possible combinations of board
		vector <int> starting_places(const vector <int> &symbol);   //gets starting places of vector given
		vector <int> rowToInt(const vector <string> &Grid);	    //changes the rows into symbolic numbers 
		vector <int> columnToInt(const vector <string> &Grid);      //changes the columns into symbolic numbers
};

/* Uncompresses string data given */
unsigned int BoardFolding::tonumber(const char &c)
{
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'a' && c <= 'z') return c - 'a' + 10;
	if(c >= 'A' && c <= 'Z') return c - 'A' + 36;
	if(c == '#') return 62;
	return 63;
}

/* Gets possible board combinations
 * takes the starting and ending of rows and vectors
 * multiply results of rows and columns
 */
int BoardFolding::combinations(){
	size_t i, j;	//used in for loops
	int r = 0;	//number of possible combinations of rows
	int c = 0;	//number of possible combinations of columns

	//get r
	for(i=0; i < srows.size(); i++){
		if(srows[i] == 1){
			for(j = 0; j < erows.size(); j++){
				if(erows[j] == 1 && j > i) r++;
			}
		}
	}
	//get c
	for(i=0; i < scolumns.size(); i++){
		if(scolumns[i] == 1){
			for(j = 0; j < ecolumns.size(); j++){
				if(ecolumns[j] == 1 && j > i) c++;
			}
		}
	}
	return r*c;
}

/* returns vector of 1's and 0's
 * if 1 the index can be a starting location
 * takes symbolic representaions of rows or columns to do comparison
 */
vector <int> BoardFolding::starting_places(const vector <int> &symbol)
{
	vector <int> r(symbol.size()+1, 0);	//return vector of starting locations
	r[0] = 1;				//initial index is always a starting location
	size_t j, w;				//used in for loops
	size_t e;				//used to compare rows or columns
	bool same;				//if rows or columns are the same this is True, else its False

	for(j=0; j < symbol.size(); j++){
		for(w = 1; w <= j && j+w <= symbol.size(); w++){
			//if j-w is a starting location
			if(r[j-w] == 1){
				e = 1;
				same = true;
				//check if the rectangle around j is the same, thus foldable
				while(e <= w && same){
					if(symbol[j+e-1] != symbol[j-e]) same = false;
					e++;
				}
				if(same == true) r[j] = 1;
			}
		}
	}
	return r;
}

/* turns the rows from the Grid into symbolic number representations */
vector <int> BoardFolding::rowToInt(const vector <string> &Grid)
{
	vector <int> r;			   //return vector
	map <string, int> symbol;	   //used to make symbolic numbers
	map <string, int>::iterator sit;   //used to find if string already exists
	int num = 0;			   //current symbolic number
	size_t i;			   //used in for loop

	for(i=0; i < Grid.size(); i++){
		sit = symbol.find(Grid[i]);
		//if the string is not in the map
		if(sit == symbol.end()){
			//push the new symbol into the return vector
			//insert it into the map & increase the current symbolic number
			r.push_back(num);
			symbol.insert(make_pair(Grid[i], num));
			num++;
		//if the string is already in the map, push its symbolic number into the vector
		} else r.push_back(sit->second);
	}
	return r;
}

/* turns the columns from the Grid into symbolic number representations */
vector <int> BoardFolding::columnToInt(const vector <string> &Grid)
{
	vector <int> r;			   //return vector
	map <string, int> symbol;          //used to make symbolic numbers
	map <string, int>::iterator sit;   //to find if already in map
	int num = 0;			   //current symbolic number
	size_t i, j;			   //used in for loops

	//goes down the Grid by rows and builds string to compare
	for(i = 0; i < Grid[0].size(); i++){
		string s;
		for(j = 0; j < Grid.size(); j++){
			s += Grid[j][i];
		}
		//search if that string is in the map
		sit = symbol.find(s);
		//if not set the number, increment, and put into map
		if(sit == symbol.end()){
			r.push_back(num);
			symbol.insert(make_pair(s, num));
			num++;
		//if its already in map push that symbolic number into the return vector
		} else r.push_back(sit->second);
	}
	return r;
}

/* Figure out how many possible combinations is given in the compressed paper
 * folds can only happen if both sides of seam match
 */
int BoardFolding::howMany(int N, int M, vector <string> compressedPaper)
{
	vector <string> Grid(N);	//uncompressed values of paper
	int rv;				//return value
	int i, j;			//used in for loops
	unsigned int ui;		//used to build strings of Grid
	
	//loop through each character in the compressed paper
	for(i = 0; i < N; i++){
		for(j=0; j < M; j++){
			//turn the character into a 1 or 0
			ui = ((tonumber(compressedPaper[i][j/6]) >> (j % 6)) % 2);
			if(ui == 0) Grid[i] += '0';
			if(ui == 1) Grid[i] += '1';
		}
	}
	rows = rowToInt(Grid);			     //get symbolic representation of rows
	columns = columnToInt(Grid);		     //get symbolic representation of columns
	srows = starting_places(rows);               //get starting places of rows
	scolumns = starting_places(columns);         //get starting places of columns
	reverse(rows.begin(), rows.end());
	reverse(columns.begin(), columns.end());
	erows = starting_places(rows);		     //get ending places of rows
	ecolumns = starting_places(columns);         //get ending places of columns
	reverse(erows.begin(), erows.end());
	reverse(ecolumns.begin(), ecolumns. end());
	rv = combinations();			     //get total combinations of possible boards
	return rv;
}
