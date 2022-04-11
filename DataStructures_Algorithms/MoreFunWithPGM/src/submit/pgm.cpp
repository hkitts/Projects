/* Hunter Kitts
 * pgm.cpp Lab 3
 */
#include "pgm.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

bool Pgm::Read(const std::string &file)
{
	ifstream fin;
	string s;
	size_t i, j, r, c, v;

	fin.open(file.c_str());
	if (fin.fail()) return false;

	if (!(fin >> s)) return false;
	if (s != "P2") return false;
	if (!(fin >> c) || c <= 0) return false;
	if (!(fin >> r) || r <= 0) return false;
	if (!(fin >> i) || i != 255) return false;

	Pixels.resize(r);
	for (i = 0; i < r; i++) {
		Pixels[i].clear();
		for (j = 0; j < c; j++) {
			if (!(fin >> v) || v > 255) return false;
			Pixels[i].push_back(v);
		}
	}
	if (fin >> s) return false;
	fin.close();
	return true;
}

bool Pgm::Write(const string &file) const
{
	//count is used to make the columns in file always 20 execpt the last one
	int count = 0;
	ofstream fout;
	size_t i, j, r, c;

	//if the row size is ever 0 fails
	if(Pixels.size() == 0)return false;


	//stores row and column value
	r=Pixels.size();
	c=Pixels[0].size();

	//opens the file if it can not fails
	fout.open(file.c_str());
	if(fout.fail())return false;

	//prints out the data for the pgm
	fout << "P2" << endl;
	fout << c << " " << r << endl;
	fout << 255 << endl;
	//prints pixel values
	for(i=0; i < Pixels.size(); i++){
		for(j=0;j < Pixels[i].size(); j++){
			fout << Pixels[i][j];
			count++;
			//makes sure the end of the file is a pixel value not a space or new line
			if((i+1 == Pixels.size()) && (j+1 == Pixels[i].size())){
				fout << endl;
				fout.close();
				return true;
			}
			//prints a space for 19 pixeel values
			if(count !=20) fout << " ";
			//prints a newline at the last pixel value for the row
			if(count == 20){
				count = 0;
				fout << endl;
			}
		}
	}
	fout.close();
	return true;
}

bool Pgm::Create(size_t r, size_t c, size_t pv)
{
	//used to loop through pgm
	size_t i,j;
	//if any values given are not within range fails
	if (c <= 0) return false;
	if (r <= 0) return false;
	if (pv > 255) return false;
	//makes pgm the correct row width
	Pixels.resize(r);
	//loop through row and make columns the correct width and fill with pv
	for (i = 0; i < r; i++) {
		Pixels[i].clear();
		for (j = 0; j < c; j++) {
			Pixels[i].push_back(pv);
		}
	}
	return true;
}
bool Pgm::Clockwise()
{
	//makes 2d vector to temporary hold transformation
	vector<vector<int> > temp;

	//used to loop through vectors
	size_t z, i, j;

	//resizes temp to match pgm
	temp.resize(Pixels[0].size());
	for(z=0; z < Pixels[0].size(); z++){
		temp[z].resize(Pixels.size());
	}

	//goes through each value of Pixels and stores in the correct/transformed place in temp
	for(i=0; i < Pixels.size(); i++){
		for(j = 0; j < Pixels[0].size(); j++){
			temp[j][temp[0].size()-i-1] = Pixels[i][j];
		}
	}

	//makes Pixels into temp that has correct transmations
	Pixels = temp;

	return true;
}
bool Pgm::Cclockwise()
{
	//makes 2d vector to temporary hold transformation
	vector<vector<int> > temp;
	//used to loop through vectors
	size_t z, i, j;
	
	//resizes temp to match pgm
	temp.resize(Pixels[0].size());
	for(z=0; z < Pixels[0].size(); z++){
		temp[z].resize(Pixels.size());
	}

	//goes through each value of Pixels and stores in the correct/transformed place in temp
	for(i=0; i < Pixels.size(); i++){
		for(j = 0; j < Pixels[0].size(); j++){
			temp[temp.size()-j-1][i] = Pixels[i][j];
		}
	}
	
	//makes Pixels into temp that has correct transmations
	Pixels = temp;

	return true;

}
bool Pgm::Pad(size_t w, size_t pv)
{
	//used to store transformations, sized to be columns + 2*w and rows + 2*w
	vector< vector <int> > temp;

	//if the pixel value is not withing range fails
	if(pv > 255) return false;

	//used to hold values of row size and column size so when Pixels is transformed the informations is still vailable
	int rs, cs;
	rs =Pixels.size();
	cs = Pixels[0].size();

	//resizes temp vector to be Pixels row + 2*w and column + 2*w and fills it with pv
	temp.resize(Pixels.size() + (2*w), vector<int> (Pixels[0].size() + (2*w) , pv));
	//resizes Pixels rows to have an extra 2*w size and sets new rows to be all pv value
	Pixels.resize(rs + (2*w), vector<int>(cs + (2*w), pv));
	//fixes the Pixels columns that were not changed in the last statement to be the size of orginal Pixels columns + 2*w and fills new values with pv
	for(int k=0; k < rs; k++){
		Pixels[k].resize(cs + (2*w), pv);
	}

	//transformation and sets Pixels values in the middle of temp until no values remain in Pixels
	for(int i=0; i < rs; i++){
		for(int j =0; j <cs; j++){
			temp[i+w][j+w] = Pixels[i][j];
		}
	}

	//sets pixels to have the correct transformations in it
	Pixels = temp;

	return true;
}
bool Pgm::Panel(size_t r, size_t c)    
{
	//if either values are 0 fails
	if(r ==0 || c == 0)return false;

	//stores transformation temporarily, sized to Pixels rows*r and columns*c
	vector< vector <int> > temp;

	//resizes temp to Pixels rows*r and Pixels columns*c
	temp.resize(Pixels.size()* r, vector<int> (Pixels[0].size()*c));

	//goes through each value of temp and stores corresponding value of Pixels to panel temp
	for(size_t i=0; i < temp.size(); i++){
		for(size_t j=0; j < temp[0].size(); j++){
			temp[i][j] = Pixels[i%Pixels.size()][j%Pixels[0].size()];
		}
	}
	//makes pixels into temp that has the correct transformations
	Pixels = temp;

	return true;
}
bool Pgm::Crop(size_t r,    size_t c,size_t rows, size_t cols)
{
	vector< vector<int> > temp;
	size_t i,j, k;
	k=0;

	//if the columns or rows will not do any transformation fails
	if(r+rows > Pixels.size())return false;
	if(c+cols > Pixels[0].size())return false;
	
	/*resizes temp to be size of rows
	 * starts at r and c index in Pixels
	 * fills temp with values in Pixels
	 * once indexs rows and cols are reached values are no longer filled
	 * new size of temp is rows and cols for rows and columns
	 */
	temp.resize(rows);
	for(i=0+r; i <rows+r;i++){
		for(j=0+c; j < cols+c; j++){
			temp[k].push_back(Pixels[i][j]);
		}
		k++;
	}

	//stores temp with correct transformations into Pixels
	Pixels = temp;

	return true;
}

