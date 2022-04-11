/* Hunter Kitts
 * sb-analyze.cpp
 * takes similar colors on superball like board and puts them into a disjoint set
 * prints out the size, color, and location of score if above give minimum size
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <vector>
#include "disjoint.h"
using namespace std;

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))

class Superball {
	public:
		Superball(int argc, char **argv);
		int r;
		int c;
		int mss;
		int empty;
		vector <int> board;
		vector <int> goals;
		vector <int> colors;

};

// Recursivly analyzes a superball like board
// Creates a disjoint set of similar colors that is rooted on goal
// if set goes over more than one goal it removes that goal so it is not printed twice
// i is the row 
// j is the column
// count is the size of the set
void analyze_superball(Superball *s, DisjointSet *d, int i, int j, int &count)
{

	int s1, s2;				//square to be compared, if they are in the same set
	
	if(s->board[i*s->c+j] == 42) return;			//if the value is an empty goal not a color, stop

	//top
	//if there is a row above the current square
	//and that square is the same color as the current square
	if((i-1) >= 0 && s->board[i*s->c+j] == s->board[(i-1)*s->c+j]){
		//find the current root
		//find the root to the square above the current 
		s1 = d->Find(i*(s->c)+j);
		s2 = d->Find((i-1)*(s->c)+j);
		//if they are not in the same set
		if(s1 != s2){
			//union them and increase set count
			d->Union(s1, s2);
			count++;
			//if the node above is also a goal remove it from to goals, so it doesn't print twice
			if(s->goals[(i-1)*(s->c)+j] == 1){
				s->goals[(i-1)*(s->c)+j] = 0;
			}
			analyze_superball(s, d, i-1, j, count);			//recursivly call sqaure above
		}
	}

	/* The rest of the if statements are the same as the above
	 * the only change in each is the comparison to square that is direcly adjacent (above   , below  , left    , & right)
	 * each if statment also checks if the adjacent square is possible              (i-1 >= 0, i+1 < r, j-1 >= 0, j+1 < c)
	 * if the color of the adjacent sqaure is the same it is added to the set and then recursivly follows that path until no more can be added
	 */

	//bottom
	if((i+1) < s->r && s->board[i*s->c+j] == s->board[(i+1)*s->c+j]){
		s1 = d->Find(i*s->c+j);
		s2 = d->Find((i+1)*s->c+j);
		if(s1 != s2){
			d->Union(s1, s2);
			count++;
			if(s->goals[(i+1)*s->c+j] == 1){
				s->goals[(i+1)*s->c+j] = 0;
			}
			analyze_superball(s, d, i+1, j, count);
		}
	}
	//left
	if((j-1) >= 0 && s->board[i*s->c+j] == s->board[i*s->c+(j-1)] ){
		s1 = d->Find(i*s->c+j);
		s2 = d->Find(i*s->c+(j-1));
		if(s1 != s2){
			d->Union(s1, s2);
			count++;
			if(s->goals[i*s->c+(j-1)] == 1){
				s->goals[i*s->c+(j-1)] = 0;
			}
			analyze_superball(s, d, i, j-1, count);
		}
	}
	//right
	if((j+1) < s->c && s->board[i*s->c+j] == s->board[i*s->c+(j+1)] ){
		s1 = d->Find(i*(s->c)+j);
		s2 = d->Find(i*(s->c)+(j+1));
		if(s1 != s2 ){
			d->Union(s1, s2);
			count++;
			if(s->goals[i*(s->c)+(j+1)] == 1){
				s->goals[i*(s->c)+(j+1)] = 0;
			}
			analyze_superball(s, d, i, j+1, count);
		}
	}
	return;
}


void usage(const char *s) 
{
	fprintf(stderr, "usage: sb-play rows cols min-score-size colors\n");
	if (s != NULL) fprintf(stderr, "%s\n", s);
	exit(1);
}

Superball::Superball(int argc, char **argv)
{
	int i, j;
	string s;

	if (argc != 5) usage(NULL);
	if (sscanf(argv[1], "%d", &r) == 0 || r <= 0) usage("Bad rows");
	if (sscanf(argv[1], "%d", &r) == 0 || r <= 0) usage("Bad rows");
	if (sscanf(argv[2], "%d", &c) == 0 || c <= 0) usage("Bad cols");
	if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0) usage("Bad min-score-size");

	colors.resize(256, 0);

	for (i = 0; i < strlen(argv[4]); i++) {
		if (!isalpha(argv[4][i])) usage("Colors must be distinct letters");
		if (!islower(argv[4][i])) usage("Colors must be lowercase letters");
		if (colors[argv[4][i]] != 0) usage("Duplicate color");
		colors[argv[4][i]] = 2+i;
		colors[toupper(argv[4][i])] = 2+i;
	}

	board.resize(r*c);
	goals.resize(r*c, 0);

	empty = 0;

	for (i = 0; i < r; i++) {
		if (!(cin >> s)) {
			fprintf(stderr, "Bad board: not enough rows on standard input\n");
			exit(1);
		}
		if (s.size() != c) {
			fprintf(stderr, "Bad board on row %d - wrong number of characters.\n", i);
			exit(1);
		}
		for (j = 0; j < c; j++) {
			if (s[j] != '*' && s[j] != '.' && colors[s[j]] == 0) {
				fprintf(stderr, "Bad board row %d - bad character %c.\n", i, s[j]);
				exit(1);
			}
			board[i*c+j] = s[j];
			if (board[i*c+j] == '.') empty++;
			if (board[i*c+j] == '*') empty++;
			if (isupper(board[i*c+j]) || board[i*c+j] == '*') {
				goals[i*c+j] = 1;
				board[i*c+j] = tolower(board[i*c+j]);
			}
		}
	}
}

main(int argc, char **argv)
{
	Superball *s;		//superball to be used 
	DisjointSet *d;		//disjoint set to be used, set by rank
	int count;			//amount of sqaures in the set
	char c;				//used to output color of set

	count = 1;							//count of set
	s = new Superball(argc, argv);				//new instance of Superball, used to fill superball like board
	d = new DisjointSetByRankWPC(s->r * s->c);	//ne instance of disjoint set, used to find scoring sets on superball like board

	cout << "Scoring sets:\n";

	//loop through all values until a goal is found that also has a color
	for(int i = 0; i < s->r; i++){
		for(int j = 0; j < s->c; j++){
			if(s->goals[i*s->c + j] == 1 && s->board[i*s->c + j] != '*'){

				analyze_superball(s, d, i, j, count);		//recursivly find the set on that goal
				c = char((s->board[i*s->c+j]));				//set the color of that set

				//if the size of the set is witing the minimum scoring size print it out	
				if(count >= s->mss){
					cout << "  Size: " << count << " Char: " << c;
					cout << " Scoring Cell: " << i << ',' << j << endl;
				}
			}
			count = 1;		//reset the size for the next set
		}
	}

	return 0;
}
