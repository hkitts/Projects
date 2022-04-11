/* AliceGame.cpp
 * Hunter Kitts
 * https://community.topcoder.com/stat?c=problem_statement&pm=13490&rd=16082
 */

#include <iostream>
#include <sstream>
#include <cstdio>
#include <math.h>
#include <vector>

using namespace std;

class AliceGame{
	public:
		long findMinimumValue(long x, long y);
};

int search(size_t first, size_t last, const long a, const long r) 
{
	size_t middle;
	long testf, testh, points;
	if(last >= first){
		middle = (first + last)/2;
		points = (((2*r*(middle+1))-(pow((middle+1), 2.0))));
		testf = a - points;
		testh = 2*(r-(middle+1)) + 1;
		if(0 <= testf && testf <= testh){
			return middle;
		}
		else if(points < a){
			return search(middle+1, last, a, r);
		} else{
			return search(first, middle-1, a, r);
		}
	}
	return -1;
}

long findMinimum(long a, long r)
{
	if(a == 2) return -1;
	if(a == 0) return 0;
	if((a < 2*r) && (a % 2 != 0)) return 1;
	if((a <= 2*r) && (a % 2 == 0)) return 2;
	if(a == 2*r + 1) return 3;

	cerr << "f didnt work\n";
	return -1;
}

long AliceGame::findMinimumValue(long x, long y)
{
	double rsquared, rtest;
	long r, newa, newr, min;
	vector <long> possibleh;
	int h;

	//test if (a+b) is prefect square
	rsquared = x + y;
	rtest = sqrt(rsquared);
	rtest = ((2*rtest)*(rtest+1))/2 - rtest;
	if(rtest != rsquared) return -1;

	r = sqrt(x + y);

	h = search(0, r, x, r);

	if(h < 0 ) return -1;
	else{
		newa = x - (((2*r*(h+1))-(pow((h+1), 2.0))));
		newr = r - (h+1);
		min = findMinimum(newa, newr);
		if(min == -1)return -1;
	
		return min + (h+1);
	}
	return -1;
}

int main(int argc, char **argv)
{
	istringstream ss;
	long x, y, r;
	AliceGame ag;

	if(argc != 3){
		cerr << "usage: Alice num1 num2\n";
		return 1;
	}
	ss.str(argv[1]);
	if(!(ss >> x)){
		fprintf(stderr, "The argument %s is not a long.\n", argv[1]);
		return 1;
	}
	ss.clear();
	ss.str(argv[2]);
	if(!(ss >> y)){
		fprintf(stderr, "The argument %s is not a long.\n", argv[2]);
		return 1;
	}

	r = ag.findMinimumValue(x, y);
	printf("%ld\n", r);
	return 0;
}
