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

/* does majority of work
 * looking for h
 * h - the last h rounds go to Alice, but the previous rounds go to opponent
 * does binary search on possible h values
 * h value is index of binary search
 */ 
int search(size_t first, size_t last, const long a, const long r) 
{
	size_t middle; //h rounds, like index into an array
		       //just without array itself
	long testf, testh, points;
	//testf - points left after a-points
	//testh - make sure testf is less than testh

	//binary search on possible h values
	if(last >= first){
		middle = (first + last)/2;
		//calculates possible points mid search so no extra time
		//is spent creating vector or array to search through
		points = (((2*r*(middle+1))-(pow((middle+1), 2.0))));
		testf = a - points;
		testh = 2*(r-(middle+1)) + 1;
		
		//if found then this is the maximum h rounds
		if(0 <= testf && testf <= testh){
			return middle;
		}
		//look in larger half of the possible h rounds
		else if(points < a){
			return search(middle+1, last, a, r);

		//look in the smaller half of the possible h rounds
		} else{
			return search(first, middle-1, a, r);
		}
	}
	return -1;
}

//given alice's score and the number of rounds left
//calcualtes rounds to win
//only small values should hit this
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

/* finds the smallest rounds Alice could have won
 * x is Alice's points
 * y is opponent
 */
long AliceGame::findMinimumValue(long x, long y)
{
	double rsquared, rtest;
	long r, newa, newr, min;
	int h;    //maximum number of rounds Alice could win
		  //assuming previous rounds go to oppponent

	//test if (a+b) is prefect square
	rsquared = x + y;
	rtest = sqrt(rsquared);
	rtest = ((2*rtest)*(rtest+1))/2 - rtest;
	if(rtest != rsquared) return -1;

	//get the number of rounds and search
	//then search for maximum amount of rounds Alice
	//could win if previous rounds are assumed opponent
	r = sqrt(x + y);
	h = search(0, r, x, r);

	//once h value found
	if(h < 0 ) return -1;
	else{
		//calculate min number of rounds given h and r
		newa = x - (((2*r*(h+1))-(pow((h+1), 2.0))));
		newr = r - (h+1);
		min = findMinimum(newa, newr);
		if(min == -1)return -1;
	
		return min + (h+1);
	}
	return -1;
}
