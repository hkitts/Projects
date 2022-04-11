#include<iostream>
#include<vector>
#include<cstdio>
#include<string>

using namespace std;

main()
{
	//stores pixel values of pgm
	vector<vector<int> > pixel;
	//total rows and columns
	int rows, columns;
	//number after rows looking for 255
	int colorSpef;
	// pixel value
	int num;
	//current pixel
	int count;
	//total amount of pixels rows*columns
	int totalPixels;
	//current index of temp vector used to verify pixel is within range
	int currentI;
	//used to print out pixel vector
	size_t x, y;
	//used to check if the first value is P2
	string s;

	count  =0;

	//gets in first value and if it is not P2 print error
	cin >> s;
	if(s != "P2"){
		fprintf(stderr, "Bad PGM file -- first word is not P2\n");
		return 1;
	}
	//gets columns, rows, and calcultes total amount of pixels
	cin >> columns >> rows;
	totalPixels = rows * columns;
	//if rows or pixels is not a good number prints error
	if(columns < 0){
		fprintf(stderr, "Bad PGM file -- No column specification\n");
		return 1;
	}
	if(rows < 0){
		fprintf(stderr, "Bad PGM file -- No column specification\n");
		return 1;
	}

	//gets in number after rows if it is not 255 print error
	cin >> colorSpef;
	if(colorSpef != 255){
		fprintf(stderr, "Bad PGM file -- No 255 following the rows and columns\n");
		return 1;
	}

	//loops through rows and creates temp vector for columns to store pixel value
	for(int i=0; i < rows; i++){
		vector<int> temp;
		//used to check if the currect pixel is within range
		currentI =0;
		for(int j=0; j < columns;j++){
			//if the current pixel is a number continue
			if(cin >> num){
				//store pixel value into temp column vector
				temp.push_back(num);
				//if the currect pixel is not withing range print error
				if(temp[currentI] < 0 || temp[currentI] > 255){
					fprintf(stderr, "Bad PGM file -- pixel %i is not a number between 0 and 255\n", count);
					return 1;
				}

			}
			//if the currnt pixel is not an int print error
			else{
					fprintf(stderr, "Bad PGM file -- pixel %i is not a number between 0 and 255\n", count);
					return 1;

			}
			currentI++;
			count++;
		}
		//stoes temp vector of pixles that has a toal of the total columns into the double vector
		pixel.push_back(temp);
	}

	//if the total pixels is not equal to the count prints error
	if(count != totalPixels){
		fprintf(stderr, "Bad PGM file -- pixel %i is not a number between 0 and 255\n", count-1);
		return 1;
	}
	//if the eof is not reached print error;
	if(cin >> s){
		fprintf(stderr, "Bad PGM file -- Extra stuff after the pixels\n");
		return 1;

	}

	//prints P2, total columns, total vectors , and 255
	printf("%s\n%i%s%i\n%i\n", "P2", columns, " ", rows, 255);
	//loops through vector and prints out vector with columns in opposite order
	for(x=0; x < pixel.size();x++){
		for(y=pixel[x].size(); y > 0; y--){
			printf("%i\n",pixel[x][y-1]);
		}
	}
	return 0;
}
