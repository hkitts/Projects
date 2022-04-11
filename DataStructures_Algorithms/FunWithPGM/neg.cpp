#include<iostream>
#include<vector>
#include<cstdio>
#include<string>

using namespace std;

main()
{
	//stores pixels of pgm file
	vector<vector<int> > pixel;
	//total rows and columns
	int rows, columns;
	//number after rows looking for 255
	int colorSpef;
	//used to get pixel and then store into vector
	int num;
	//currect pixel
	int count;
	//total pixels or rows * columns
	int totalPixels;
	//current index of the inner vector of pixels. Used to check if number is correct value
	int currentI;
	//stores first value, looking for P2
	string s;
	//used in for loop to print out pixel vecto
	size_t x, y;
	

	count  =1;
	//gets first value and prints error if it is not P2
	cin >> s;
	if(s != "P2"){
		fprintf(stderr, "Bad PGM file -- first word is not P2\n");
		return 1;
	}
	//gets columns and rows gets total Pixels values
	cin >> columns >> rows;
	totalPixels = rows * columns;

	//checks if columns and rows are good numbers if not prints error
	if(columns < 1){
		fprintf(stderr, "Bad PGM file -- No column specification\n");
		return 1;
	}
	if(rows < 1){
		fprintf(stderr, "Bad PGM file -- No row specification\n");
		return 1;
	}
	//gets number after rows and checks if it is 255 if not print error
	cin >> colorSpef;
	if(colorSpef != 255){
		fprintf(stderr, "Bad PGM file -- No 255 following the rows and columns\n");
		return 1;
	}
	/*doulbe loop to store values of pgm into pixel vector
	 * creates temporary vector for columns
	 * cand sets index of currentI to 0 so each pixel can be verrified as correct
	 */
	for(int i=0; i < rows; i++){
		vector<int> temp;
		currentI =0;
		for(int j=0; j < columns;j++){

			//gets pixel and stores it in temporary vector
			cin >> num;
			temp.push_back(num);

			//checks if pixel and current index is correct if not prints error
			if(temp[currentI] < 0 || temp[currentI] > 255){
				fprintf(stderr, "Bad PGM file -- pixel %i is not a number between 0 and 255\n", count);
				return 1;
			}

			//increase current pixel as count and currentI that is used to check correct pixel value
			currentI++;
			count++;

		}
		//stoes temp vector into final vector untill no rows remain
		pixel.push_back(temp);
	}
	//if the eof is not reached print error
	if(cin >> s){
				fprintf(stderr, "Bad PGM file -- Extra stuff after the pixels\n");
				return 1;

	}
	//print out P2, total columns, toal rows, and 255
	printf("%s\n%i%s%i\n%i\n", "P2", columns, " ", rows, 255);
	/*goes through each value of pixel vector and subtracts it from 255
	 * then prints out pixel
	 */

	for(x=0; x < pixel.size(); x++){
		for(y=0; y < pixel[x].size();y++){
			pixel[x][y]= 255 - pixel[x][y];
			printf("%i\n",pixel[x][y]);
		}
	}
	return 0;
}
