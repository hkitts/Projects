#include<iostream>
#include<vector>
#include<cstdio>
#include<string>

using namespace std;

main()
{
	//stores value of pixels in vector of rows, columns
	vector<vector<int> > pixel;
	//total rows and total columns
	int rows, columns;
	//number after rows looking for 255
	int colorSpef;
	// Pixel value
	int num;
	//current pixel
	int count;
	//total pixels in pgm
	int totalPixels;
	//current index in temporary vector used to check if pixel is within range
	int currentI;
	//used to print out pixel vector
	size_t x, y;
	//used to check if first value is P2
	string s;

	count  =0;
	x, y =0;

	//gets first value if its is not P2 prints error
	cin >> s;
	if(s != "P2"){
		fprintf(stderr, "Bad PGM file -- first word is not P2\n");
		return 1;
	}
	//if the second value is an int store it into columns
	if(cin >> columns){
		//if the third value is an int store it into rows
		if (cin>> rows){
			//gets total pixels in the pgm
			totalPixels = rows * columns;
			//if columns or rows is not within range print error
			if(columns <= 0){
				fprintf(stderr, "Bad PGM file -- No column specification\n");
				return 1;
			}
			if(rows <= 0){
				fprintf(stderr, "Bad PGM file -- No row specification\n");
				return 1;
			}
			//get next value if it si not 255 print error
			cin >> colorSpef;
			if(colorSpef != 255){
				fprintf(stderr, "Bad PGM file -- No 255 following the rows and columns\n");
				return 1;
			}

			//loops through outer part of vector and stores vectors of columns that is useds as a row
			for(int i=0; i < rows; i++){
				//use a temp vector to store pixles
				vector<int> temp;
				currentI =0;
				for(int j=0; j < columns;j++){
					//if the pixel is an int continue
					if(cin >> num){
						//store pixel into vector of columns
						temp.push_back(num);
						//check if pixel is withing value if not print error
						if(temp[currentI] < 0 || temp[currentI] > 255){
							fprintf(stderr, "Bad PGM file -- pixel %i is not a number between 0 and 255\n", count);
							return 1;
						}

					}
					//if the pixel is not an int print error
					else{
						fprintf(stderr, "Bad PGM file -- pixel %i is not a number between 0 and 255\n", count);
						return 1;

					}
					currentI++;
					count++;
				}
				//store vector of columns into vector pixel of rows
				pixel.push_back(temp);
			}
			//if the number of times the loop happended is not the total amount nof pixels print error
			if(count != totalPixels){
				fprintf(stderr, "Bad PGM file -- pixel %i is not a number between 0 and 255\n", count);
				return 1;
			}
			//if not eof print error
			if(cin >> s){
				fprintf(stderr, "Bad PGM file -- Extra stuff after the pixels\n");
				return 1;

			}
			//print P2, total columns, total rows, and 255
			printf("%s\n%i%s%i\n%i\n", "P2", columns, " ", rows, 255);
			//print outs the vector but prints the rows out opposite of how they were stored in
			//starts at last row of columns and goes up as it prints
			for(x=pixel.size(); x > 0; x--){
				for(y=0; y < pixel[x-1].size(); y++){
					printf("%i\n",pixel[x-1][y]);
				}
			}
			return 0;
		}
		//if the third value in the pgm file is not an int print error
		else{
			fprintf(stderr, "Bad PGM file -- No row specification\n");
			return 1;

		}
	}
	//if the second value in the pgm file is not an int print error
	else{
		fprintf(stderr, "Bad PGM file -- No column specification\n");
		return 1;
	}
}
