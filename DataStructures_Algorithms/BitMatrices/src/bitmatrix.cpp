/* Hunter Kitts
 * bitmatrix.cpp
 * Lab 5
 */
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include "bitmatrix.hpp"
using namespace std;

Bitmatrix::Bitmatrix(int rows, int cols)
{
	//error checking rows and cols
	if(rows <=0) throw((string) "Bad rows");
	if(cols <=0) throw((string) "Bad cols");

	//resises M to be rows by cols in size
	M.resize(rows);
	for(int i=0; i < rows; i++){
		for(int j=0; j < cols; j++){
			M[i].push_back('0');
		}
	}
}

Bitmatrix::Bitmatrix(const string &fn)
{
	size_t found;				//used to error check fn
	ifstream fin;				//used to open file
	string row, rowNoSpace;		//row is for getline
								//rowNoSpace is row with no spaces

	//open file if possible
	fin.open(fn.c_str());
	if(fin.fail()) throw((string) "Can't open file");
	
	//gets the bits line by line in from the file fn
	while(getline(fin, row)){
		
		//if a character is found out of bounds throws error
		found = row.find_first_not_of("10 \n");
		if( found != std::string::npos) throw((string) "Bad file format");
		
		//changes from rows with spaces to no spaces
		for(size_t i =0; i < row.size(); i++){
			if(row[i] == '0' || row[i] == '1'){
				rowNoSpace.push_back(row[i]);
			}
		}

		//enters rows into M as long as its not empty
		if(rowNoSpace != ""){
			M.push_back(rowNoSpace);
		}
		rowNoSpace.clear();
	}
}

Bitmatrix *Bitmatrix::Copy() const
{
	Bitmatrix *bmC;

	//makes copy of Bitmatrix and M values
	bmC = new Bitmatrix(M.size(), M[0].size());
	bmC-> M = M;
	return bmC;
}

bool Bitmatrix::Write(const string &fn) const
{
	ofstream fout;		//used to epen file
	size_t i;			//iterates through M


	fout.open(fn.c_str());
	if(fout.fail()) return false;
	
	//prints M to file fn
	else{
		for(i =0;i < M.size(); i++){
			fout << M[i] << endl;
		}
		fout.close();
		return true;
	}
}

void Bitmatrix::Print(size_t w) const
{
	/* space : copy of w that is incremented through the loop
	 * i, j  : used for for loops
	 * count : used to know when to print a space in the row every w characters
	 * count2: used to know when to print a space in the col every w rows
	 */
	size_t space, i, j, count, count2;
	
	count2 = 0;

	//set space
	if(w <= 0){
		space = 0;
	}
	else{
		space = w;
	}

	//loop through M rows
	for(i =0; i < M.size(); i++){
		count =0;
		//loop through chars in M[i] row
		for(j= 0; j < M[i].size(); j++){

			//print out character or space if loop as happed w times
			if(count == space && j != M[i].size()  && space != 0){
				cout << " ";
				count =0;
			}
			cout << M[i][j];
			count++;

			//print out newline after w rows but not the last row
			if(j == M[i].size() -1){
				cout << "\n";
				count2++;
			}
			if(count2 == w && i != M.size() -1 && space != 0){
				cout << "\n";
				count2 = 0;
			}
		}
	}
}

bool Bitmatrix::PGM(const string &fn, int p, int border) const
{
	ofstream fout;						//used to output pgm to file
	vector< vector<int> > pgm;			//stores pgm in double vector
	int num;							//used to convert M's chars into ints for pgm vector
	size_t i, j, k, l, m, n, o, q;		//used to increment throught pgm, posistion to start is determined by p and border
	size_t z;							//used to resize pgm to correct size

	//these are used to increment through M one bit at a time
	o =0;								
	q =0;

	//error checking p, borader, and fn
	if(p <=0 || border < 0) return false;
	if(fn.size() < 4) return false;

	//resises pgm to correct size with all 0's no data from the bitmatrix
	pgm.resize( ((M.size()+1) * border) + (M.size()* p) );
	for(z=0; z < pgm.size(); z++){
		pgm[z].resize( ((M[0].size()+1) * border) + (M[0].size()* p));
	}

	/*these loops are all to get to the indexes where the M data is to replace the 0 in the pgm vector
	 * the first for loop is for the rows in pgm
	 * the second is for the cols in pgm
	 * the third increments through the p*p points rows
	 * the forth increments through the p*p points cols
	 * each bit in the bitmatrix is used to repalce the points in pgm in a p*p square
	 * each bit happens one at a time then the first and second loops go to the next bit
	 * this goes to the thrid and forth loops that make a p*p square insied the pgm vector
	 * the squares value is determinted by the bitmatrix value 
	 */
	for(i = border; i < pgm.size(); i=i+p+border){
		for(j = border; j < pgm[0].size(); j=j+p+border){
			for(k=i; k < i+p; k++){
				for(l=j; l < j+p; l++){
					if(M[o][q]=='1'){
						num = 100;
					}
					if(M[o][q]=='0'){
						num = 255;
					}
					pgm[k][l] = num;
				}
			}
			q++;
		}
		o++;
		q=0;
	}

	//open file and prints pgm file requirements to the top. The row and col size is from pgm vector
	fout.open(fn.c_str());
	if(fout.fail()) return false;
	else{
		fout << "P2" << endl;
		fout << pgm[0].size() << " " << pgm.size() << endl;
		fout << "255" << endl;

		//prints out pgm vector into fn, prints space after every int and endl after every row, execpt the last
		for(m=0; m<pgm.size(); m++){
			for(n=0; n < pgm[0].size(); n++){
				fout << pgm[m][n];
				if(n < pgm[0].size() -1){
					fout << " ";
				}
				if(n == (pgm[0].size() -1) && m != (pgm.size()-1) ){
					fout << endl;
				}
			}
		}
		return true;
	}
	return false;
}

int Bitmatrix::Rows() const
{
	//gets row size from M
	int numRows;
	numRows = M.size();
	return numRows;
}

int Bitmatrix::Cols() const
{
	//gets col size from M
	int numCols;
	numCols = M[0].size();
	return numCols;
}

char Bitmatrix::Val(int row, int col) const
{
	//gets value at row and col index in M

	size_t rt, ct;		//used to error check row and col 
	char element;		//returned value

	//rt & ct are used to get rid of comparison warining and error check
	rt = row;			
	ct = col;
	element = 'x';

	//error checks row and col if they are out of range it returns 'x'
	if(row < 0 || rt > M.size()) return element;
	if(col < 0 || ct > M[row].size()) return element;

	//gets value at row and col index
	element = M[row][col];
	return element;
}

bool Bitmatrix::Set(int row, int col, char val)
{	
	size_t rt, ct;		//used to error check row and col
	
	//used to get rid of comparison warining and error check
	rt = row;
	ct = col;
	
	//repalces the value at row and col index in M with val 
	if(val == '0' || val == '1'){
		if(rt > M.size() || row < 0)return false;
		if(ct > M[row].size() || col < 0)return false;
		if(val == 0 || val == '0'){
			M[row][col] = '0';
			return true;
		}
		if(val == 1 || val == '1'){
			M[row][col] = '1';
			return true;
		}
	}
	return false;
}

bool Bitmatrix::Swap_Rows(int r1, int r2)
{
	size_t t1, t2;			//used to error check r1 and r2 and get rid of comparison warining 
	string row1, row2;		//stores values of rows at M[r1] and M[r2]

	t1 = r1;
	t2 = r2;

	if(r1 < 0 || r2 < 0 || t1 > M.size() || t2 > M.size() ) return false; 

	//stores values at indexes and swaps them
	row1 = M[r1];
	row2 = M[r2];
	M[r1] = row2;
	M[r2] = row1;
	return true;

}

bool Bitmatrix::R1_Plus_Equals_R2(int r1, int r2)
{

	size_t t1, t2;				//error checks r1 and r2
	size_t i;					//used to increment through loop
	string sum;					//sum of row 1 and 2 added up
	int num1, num2, tot;		//used to store value in M that is a char and do some math with it
	
	t1 = r1;
	t2 = r2;
	
	if(r1 < 0 || r2 < 0 || t1 >= M.size() || t2 >= M.size() ) return false; 
	if(M[r1].size() != M[r2].size()) return false;
	
	//resizes sum to be the same size as M cols. then loops through values of M adding them up mod 2
	//each character is converted to a number, the total is found, converted back to a char and stored in sum
	sum.resize(M[r1].size());
	for(i =0; i < M[r1].size(); i++){
		num1 =M[r1][i] - '0';
		num2 =M[r2][i] - '0';
		tot = (num1 + num2)%2;
		sum[i] = tot + '0';
	}
	//sum replaces the row at r1 in M
	M[r1] = sum;
	return true;
}

Bitmatrix *Sum(const Bitmatrix *a1, const Bitmatrix *a2)
{	
	int i, j;						//used to loop through each Bitamtrix
	int num1, num2, tot;			//used to do bit addition and change chars to int
	Bitmatrix *bmC;					//used to create new bitmatrix with sum of a1 and a2

	//error checking a1 and a2
	if(a1->Rows() != a2->Rows()){
		return NULL;
	}
	if(a1->Cols() != a2->Cols()){
		return NULL;
	}

	//set memory asside for new Bitmatrix that bmC points to
	bmC = new Bitmatrix(a1->Rows() , a1->Cols());
	
	//do bit addition to each value in a1's and a2's M and sum the bit to put into bmC's M
	for(i = 0; i < a1->Rows(); i++){
		for(j = 0;j < a1->Cols(); j++){
			num1 = a1->Val(i, j) - '0';
			num2 = a2->Val(i, j) - '0';
			tot = (num1+num2)%2;
			bmC->Set(i, j, tot+'0');
		}
	}
	return bmC;
}

Bitmatrix *Product(const Bitmatrix *a1, const Bitmatrix *a2)
{
	int i, j, k;				//used to loop through each Bitamtrix
	int num1, num2, tot;		//used to do bit multiplication, changes chars to int
	Bitmatrix *bmC;				//used to create new bitmatrix with product of a1 and a2

	tot = 0;

	if(a1->Cols() != a2->Rows()){
		return NULL;
	}

	//set memory asside for new Bitmatrix that bmC points to
	bmC = new Bitmatrix(a1->Rows() , a2->Cols());

	/* loop through a1 and a2 each last loop should be the same value for a1 cols and a2 rows
	 * multiplies each values in a1's and a2's M and adds them to tot
	 * tot will become a string the length of that string depends on the sizes of a1 and a2
	 * tot is then added into bmC's M, the final product will be a new bitmatrix
	 */
	for(i =0; i < a2->Cols(); i++){
		for(j =0; j < a1->Rows(); j++){
			for(k =0; k < a1->Cols(); k++){
				num1 = a1->Val(j,k) - '0';
				num2 = a2->Val(k,i) - '0';
				tot = tot + (num1 *num2);
			}
			tot = tot%2;
			bmC->Set(j, i, tot+'0');
			tot = 0;
		}
	}
	return bmC;
}

Bitmatrix *Sub_Matrix(const Bitmatrix *a1, const vector <int> &rows)
{
	Bitmatrix *bmC;			//used to create new bitmatrix with product of a1 and a2
	size_t i;				//for loops through a1's rows and cols
	int j;

	//set memory asside for new Bitmatrix that bmC points to
	bmC = new Bitmatrix(rows.size(), a1->Cols());

	//goes through a1's M and stores the rows indexed by the rows vector into bmC's M
	for(i = 0; i< rows.size(); i++){
		for(j = 0; j < a1->Cols(); j++){
			if(rows[i] > a1->Rows()) return NULL;
			else{
				bmC->Set(i, j, a1->Val(rows[i], j));
			}
		}
	}
	return bmC;
}

Bitmatrix *Inverse(const Bitmatrix *m)
{
	/* bmC: copy of m to do matrix opperations on and find the inverse if possible
	 * bmI: the inverseve of m at first it is set to the identity matrix
	 * i,j: used to go through the bitmatrix
	 */
	Bitmatrix *bmC, *bmI;
	int i, j;

	//error checking 
	if(m->Rows() != m->Cols()){
		return NULL;
	}

	//set memory aside for bmC and bmI
	bmC = new Bitmatrix(m->Rows(), m->Cols());
	bmI = new Bitmatrix(m->Rows(), m->Cols());

	//sets bmC to copy m
	bmC = m->Copy();

	//set up bmI to be identity matrix of m
	for(i =0; i < bmI->Rows(); i++){
		bmI->Set(i,i,'1');
	}

	/* every opperation is done to both bmC's M and bmI's M
	 * goes through each row and looks for the first bit in one of the rows to be '1'
	 * uses Swap_Rows if necessary
	 * if none of them are it can not be inverted
	 * then it goes through every other row and looks for a '1' in the same possition as the first
	 * loop continuses untill all values in the first bit are '0' except the first
	 * it uses R1_Plus_Equals_R2 to accomplish this
	 * the loop then repeats for the next column ignoring the first previous row(s)
	 */
	for(i = 0; i < bmC->Rows(); i++){
		j = i + 1;
		while(bmC->Val(i,i) != '1'){
			//error checking
			if(j >= bmC->Rows()){
				delete bmC;
				delete bmI;
				return NULL;
			}

			if(bmC->Val(j,i) == '1'){
				bmC->Swap_Rows(i , j);
				bmI->Swap_Rows(i , j);
			}
			j++;
		}
		for(j = i; j < bmC->Rows(); j++){
			if(bmC->Val(j,i) == '1'&& j >i){
				bmC->R1_Plus_Equals_R2(j, i);
				bmI->R1_Plus_Equals_R2(j, i);
			}
			if(bmC->Val(j,i) == '1' && i >j){
				delete bmC;
				delete bmI;
				return NULL;
			}
		}
	}

	/*after this is completed bmC's M should be a upper-triangluar matrix
	 * it then does the prevous loop and starts at the last row and last col
	 * only the R1_Plus_Equals_R2 is used the rows are never swapped
	 * after it is finished bmC should be the identity matrix
	 * bmI will be the inverse matrix of m and the pointer to it is returned
	 */
	for(i = bmC->Rows()-1; i >=0; i--){
		for(j = i; j < bmC->Cols();j++){
			if(bmC->Val(i,j) == '1'&& j > i){
				bmC->R1_Plus_Equals_R2(i, j);
				bmI->R1_Plus_Equals_R2(i, j);
			}
		}
	}
	return bmI;
}

//hash function to be used in the hash table
unsigned int djb_hash(const string &s)
{
	size_t i;
	unsigned int h;

	h = 5381;

	for (i = 0; i < s.size(); i++) {
		h = (h << 5) + h + s[i];
	}

	return h;
}

BM_Hash::BM_Hash(int size)
{
		//sets the table size of the hash table, it is set in bitmatrix_editor

		if(size <=0)throw((string) "Bad size");
		Table.resize(size);
}

bool BM_Hash::Store(const string &key, Bitmatrix *bm)
{
	size_t i;				//used to loop through hash table
	int h;					//hashed value
	HTE *p;					//used to point to key and bitmatrix to put in hash table

	//hash the key
	h = djb_hash(key) % Table.size();

	//put the key and bitmatrix into hash table entry
	p = new HTE;
	p->key = key;
	p->bm = bm;

	//finds a spot for the HTE to be put in
	for(i = 0; i < Table[h].size(); i++){
		if(key == Table[h][i].key)return false;
	}

	//puts p into the table
	Table[h].push_back(*p);
	return true;

}

Bitmatrix *BM_Hash::Recall(const string &key) const
{
	size_t i;				//used to loop through hash table
	int h;					//hashed value

	//hash the key
	h = djb_hash(key) % Table.size();

	//find the key in the table and return the bm at at that key if not found returns null
	for(i = 0; i < Table[h].size(); i++){
		if(Table[h][i].key == key){
			return Table[h][i].bm;
		}
	}
	return NULL;
}

vector <HTE> BM_Hash::All() const
{
	vector <HTE> rv;		//used to store each HTE in the hash table
	size_t i, j;			//used to loop through hash table

	//goes through hash table and if a HTE is present push it into rv then return rv which is made up of keys and bm at each index
	for(i = 0; i < Table.size(); i++){
		if(!Table[i].empty()){
			for(j = 0; j < Table[i].size(); j++){
				rv.push_back(Table[i][j]);
			}
		}
	}
	return rv;
}
