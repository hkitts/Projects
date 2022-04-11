#include<iostream>
#include<vector>
#include<cstdio>

using namespace std;

main()
{
	//vector used to store pixels of pgm
	vector <int> pgm;

	//stores number of rows, column, and total pixels
	int rows;
	int columns;
	int pixels;

	//used to store number after columns and pixels, looking for 255
	int colorSpef;

	//stores individual pixel of pgm
	int num;

	//current pixel
	int count;
	
	//d is used to take pixel value and turn it into a double. AvgPixel is total avgerage of pixels
	double avgPixel, d;
	
	//used to check first value
	string s;

	count = 0;
	avgPixel = 0;

	//gets first value if it is not P2 return error
	cin >> s;
	if (s != "P2"){
		cerr <<"Bad PGM file -- first word is not P2\n";
		return 1;
	}
	//gets columns rows and total pixels
	cin >> columns >> rows;
	pixels = rows * columns;

	//tests if columns and rows is good number. If not returns error
	if(columns < 1){
		cerr << "Bad PGM file -- No column specification\n";
		return 1;
	}
	if(rows < 1){
		cerr << "Bad PGM file -- No row specification\n";
		return 1;
	}
	//gets number after rows, looks for 255, if not 255 returns error
	cin >> colorSpef;
	if(colorSpef != 255){
		cerr << "Bad PGM file -- No 255 following the rows and colums\n";
		return 1;
	}

	//if all other values pass gets pixels and stores them into num
	else{
		while(cin >> num){
			//puts each pixel value into the vector, increases the count to find current pixel, and totals up avgPixel
			pgm.push_back(num);
			count++;
			d = pgm[count-1];
			avgPixel = d + avgPixel;
			//if the size is larger than the total pixels returns error
			if(pgm.size() > pixels){
				cerr << "Bad PGM file -- Extra stuff after the pixels\n";
				return 1;
			}
			//if the current pixel is not a valid number returns error
			if(pgm[count-1] < 0 || pgm[count-1] > 255){
				cerr << "Bad PGM file -- pixel " << count-1 << " is not a number between 0 and 255\n";
				return 1;
			}	
		}

		//calculates total pixel and prints out data in table
		avgPixel=avgPixel/pixels;
		printf("%-s%12i\n","# Rows:",rows);
		printf("%-s%9i\n","# Columns:",columns);
		printf("%-s%10i\n","# Pixels:",pixels);
		printf("%-s%9.3f\n","Avg Pixel:",avgPixel);

		return 0;
	}
}
