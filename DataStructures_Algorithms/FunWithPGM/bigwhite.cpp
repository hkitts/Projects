#include <iostream>
#include <cstdio>
#include <vector>
#include <sstream>

using namespace std;

int main(int argc, char **argv){

	//used to store argument so it can be used
	istringstream ss;
	//number of rows, columns
	int rows, columns;
	//stores 255 white value pixel to be entered in vector
	int num;
	//stores pixels in vector of vector with rows and columns
	vector<vector <int> > pixel;
	//used in for loop to print pixel less than the vector size
	size_t x,y;

	//sets number at 255 white value
	num = 255;

	//makes sure total arguments is 3 if not returns error
	if (argc !=3){
		fprintf(stderr, "usage: bigwhite rows cols\n");
		return 1;
	}
	//when 3 arguments are given continues program
	else{
		/*the first arguments is skipped because it is the program itself
		 * the second is then stored into ss variable if it is a number
		 * then it is stored as the number of rows.
		 */
		ss.str(argv[1]);
		if(ss >> rows){
			//if the number is not valid returns error (if negative or 0)
			if(rows < 1){
				fprintf(stderr, "usage: bigwhite rows cols\n");
				return 1;	
			}

			//clears stringsteam so the next argument can be checked
			ss.clear();
			ss.str(argv[2]);
			//if the third argument is a number continues
			if(ss >> columns){
				//if the number is not valid returns error(if negative or 0)
				if(columns < 1){
					fprintf(stderr, "usage: bigwhite rows cols\n");
					return 1;	
				}
				//clears the stringstream just in case it might be accessed again
				ss.clear();
				/*double loop to store double vector with all pixels of 255 value
				 * the first loop is used to go through the rows to created pgm
				 * a temporary vector is used to store column data, or 255 in this case
				 * the second loop stores the pixel 255 into a vector that makes up one row the size of the maximum column
				 * this vector is then stored into the indexed row of the pixel vector
				 * this creates a double vector of rows and columns
				 */
				for(int i=0; i < rows; i++){
					vector<int> temp;
					for(int j=0; j < columns; j++){
						temp.push_back(num);
					}
					pixel.push_back(temp);			
				}
				//prints out P2, the number of columns, and rows
				printf("%s\n%i%s%i\n%i\n","P2",columns," ",rows,255);
				//goes through double vector and prints every value
				for(x=0; x < pixel.size(); x++){
					for(y=0; y < pixel[x].size();y++){
						printf("%i\n",pixel[x][y]);
					}
				}	
				return 0;
			}
			//occurs if the third argument is not a number
			else
			{
				fprintf(stderr, "usage: bigwhite rows cols\n");
				return 1;
			}
		}
		//occurs if the second argument is not a number
		else
		{
			fprintf(stderr, "usage: bigwhite rows cols\n");
			return 1;
		}

	}
}
