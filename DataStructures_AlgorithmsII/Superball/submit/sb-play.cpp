/* Hunter Kitts
 * sb-play.cpp
 * prints out swap and score to play supperball 
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
		int ml;							//moves left
		vector <int> board;
		vector <int> goals;
		vector <int> colors;
		vector <int> size;
		vector <int> same_color;		//same color square as the greatest set
		int greatest;					//root of the greatest set
		int greatest_size;				//greatest set's size
		int score;						//scoring square
		vector <int> greatest_closest;	//squares of the greatest set closest to the goal
		vector <int> greatest_surround;	//swappable squarest next to the greatest set
		int greatest_closest_dist;		//distance away from goal of the greatest_closest squares 

};

void swap_score(Superball *s, DisjointSet *d)
{
	int ip, jp, ic, jc;			//ip and jp  - i and j of first square, to be swapped or scored
								//ic and jc  - i and j of square to be swapped
	int element1, element2;		//i and j of squares in i*c+j form
	int i;						//loop through board to find random squares
	int test;					//used to find random squares not a part of the greatest set

	element1 = -1;
	element2 = -1;		//if element2 is negative the board will score

		//if there is a swappable square between the greatest set and the goal
		if(s->greatest_surround.size() != 0){
			element1 = s->greatest_surround.back();
			s->greatest_surround.pop_back();
		//otherwise swap the root, or score
		} else element1 = s->greatest;

		//if there is a node that is the same color as the greatest set and not a part of the set
		if(s->same_color.size() != 0){
			element2 = s->same_color.back();
			s->same_color.pop_back();
		}

		//if the moves left is less than the amount need to score, try to score
		if(s->ml < s->mss){
			//if its possible to score
			if(s->score != -1 && s->greatest_size >= s->mss){
				element1 = s->score;
				element2 = -1;
			}

		//if both colors not a part of the set and swappable squares are not present, grab 2 random squares not in the set
		} else{
			i = 0;
			while(element2 == -1 && i < s->board.size()){
				if(s->board[i] != '*' && s->board[i] != '.'){
					test = d->Find(i);
					if(test != s->greatest){
						element2 = test;
					}
				}
				i++;
			}
		}
	//calculate the i and j of both elements
	ip = element1 /10;
	jp = element1 %10;
	ic = element2 /10;
	jc = element2 %10;

	//if its time to score
	if(element2 == -1){
		cout << "SCORE " << ip << " " << jp << endl;
	//otherwise swap
	}else{
		cout << "SWAP " << ip << " " << jp << " " << ic << " " << jc << endl;	
	}
}

//finds the closest squares to the goal and puts them in a vector
void closest(Superball *s, DisjointSet *d, vector<int> const &dist)
{
	s->greatest_closest_dist = 6;	//over maximum distance to the goal
	int color;						//color of greatest set
	int e;							//square to be compared
	int	i, j;						//used in for loops
	int ce;

	s->score = -1;					//set scoring square to -1 until one is found
	color = s->board[s->greatest];	//get the color of the greatest set

	//loop through all the squares on the board
	for(i =0; i < s->r; i++){
		for(j =0; j < s->c; j++){
			//get the squares pat of the greatest set
			e = d->Find(i*(s->c)+j);
			if(e == s->greatest){

				//if the square is on the goal make it the score 
				if(dist[i*(s->c)+j] == 0){
					s->score = i*(s->c)+j;
				}

				//check surroundings
				
				//top
				if((i-1) >= 0){
					if(s->board[(i-1)*s->c+j] != '*' && s->board[(i-1)*s->c+j] != '.'){
						//if the top sqaure is not a part of the set
						ce = d->Find((i-1)*s->c+j);
						if(ce != e){
							//if the square is closer to the the goal then the last 
							//make it the new closest distance
							if(abs(dist[(i-1)*s->c+j]) < abs(s->greatest_closest_dist)){
								s->greatest_surround.clear();
								s->greatest_surround.push_back((i-1)*s->c+j);
								s->greatest_closest_dist = dist[(i-1)*s->c+j];
							} //if its the same distance to the goal as the last square add it to the vector
							else if(abs(dist[(i-1)*(s->c)+j]) == abs(s->greatest_closest_dist)){
								s->greatest_surround.push_back((i-1)*s->c+j);
							}
						}
					}
				}
				//each other if statement checks the same things just in different locations
				
				//bottom
				if((i+1) < s->r){
					if(s->board[(i+1)*s->c+j] != '*' && s->board[(i+1)*s->c+j] != '.'){
						ce = d->Find((i+1)*s->c+j);
						if(ce != e){
							if(abs(dist[(i+1)*s->c+j]) < abs(s->greatest_closest_dist)){
								s->greatest_surround.clear();
								s->greatest_surround.push_back((i+1)*s->c+j);
								s->greatest_closest_dist = dist[(i+1)*s->c+j];
							}
							else if(abs(dist[(i-1)*(s->c)+j]) == abs(s->greatest_closest_dist)){	
								s->greatest_surround.push_back((i+1)*s->c+j);
							}
						}
					}
				}
				//left
				if((j-1) >= 0){
					if(s->board[i*s->c+(j-1)] != '*' && s->board[i*s->c+(j-1)] != '.'){
						ce = d->Find((i*s->c+(j-1)));
						if(ce != e){
							if(abs(dist[i*s->c+(j-1)]) < abs(s->greatest_closest_dist)){
								s->greatest_surround.clear();
								s->greatest_surround.push_back(i*s->c+(j-1));
								s->greatest_closest_dist = dist[i*s->c+(j-1)];
							}
							else if(abs(dist[i*(s->c)+(j-1)]) == abs(s->greatest_closest_dist)){
								s->greatest_surround.push_back(i*s->c+(j-1));
							}
						}
					}
				}
				//right
				if((j+1) < s->c){
					if(s->board[i*s->c+(j+1)] != '*' && s->board[i*s->c+(j+1)] != '.'){
						ce = d->Find((i*s->c+(j+1)));
						if(ce != e){
							if(abs(dist[i*s->c+(j+1)]) < abs(s->greatest_closest_dist)){
								s->greatest_surround.clear();
								s->greatest_surround.push_back(i*s->c+(j+1));
								s->greatest_closest_dist = dist[i*s->c+(j+1)];
							}
							else if(abs(dist[i*(s->c)+(j+1)]) == abs(s->greatest_closest_dist)){
								s->greatest_surround.push_back(i*s->c+(j+1));
							}
						}
					}
				}
			} //if the color is the same but not a part of the greatest set add it to the color vector
			else if(s->board[e] == color){
				s->same_color.push_back(e);
			}
		}
	}
	return;
}

//finds the greatest set
void top_size(Superball *s, DisjointSet *d)
{
	s->greatest = -1;			//set the root of the greatest set outside the grid
	s->greatest_size = 0;		

	int i, j;					//used in for loop
	int e;						//used to compare sets roots

	//loop through all the squares
	for(i =0; i < s->r; i++){
		for(j =0; j < s->c; j++){
			//if it is a color
			if(s->board[i*s->c+j] != '*' && s->board[i*s->c+j] != '.'){
				//find its root
				e = d->Find(i*(s->c)+j);
				//increment roots size value (sets size value) 
				s->size[e]++;
				//if the set is the greastest make it the new greatest set
				if(s->size[e] >= s->greatest_size){
					s->greatest = e;
					s->greatest_size = s->size[e];
				}
			}
		}
	}
	return;
}

//set up the disjoint sets
void analyze(Superball *s, DisjointSet *d)
{
	int i, j;			//used in for loops
	int s1, s2;			//used to compares if square is in the same set

	//loop through all the sqaures
	for(i =0; i < s->r; i++){
		for(j =0; j < s->c; j++){

			//top
			//if the square above the current square is present and the same color, and a color
			if((i-1) >= 0 && s->board[i*s->c+j] == s->board[(i-1)*s->c+j]){
				if(s->board[i*s->c+j] != '*' && s->board[i*s->c+j] != '.'){
					//find there roots
					s1 = d->Find(i*(s->c)+j);
					s2 = d->Find((i-1)*(s->c)+j);
					//if the root is not the same union them
					if(s1 != s2){
						d->Union(s1, s2);
					}
				}
			}

			//each of the following if statements work the same
			//the only thing that changes is the adjacent square that is being compared

			//bottom
			if((i+1) < s->r && s->board[i*s->c+j] == s->board[(i+1)*s->c+j]){
				if(s->board[i*s->c+j] != '*' && s->board[i*s->c+j] != '.'){
					s1 = d->Find(i*s->c+j);
					s2 = d->Find((i+1)*s->c+j);
					if(s1 != s2){
						d->Union(s1, s2);
					}
				}
			}

			//left
			if((j-1) >= 0 && s->board[i*s->c+j] == s->board[i*s->c+(j-1)] ){
				if(s->board[i*s->c+j] != '*' && s->board[i*s->c+j] != '.'){
					s1 = d->Find(i*s->c+j);
					s2 = d->Find(i*s->c+(j-1));
					if(s1 != s2){
						d->Union(s1, s2);
					}
				}
			}

			//right
			if((j+1) < s->c && s->board[i*s->c+j] == s->board[i*s->c+(j+1)] ){
				if(s->board[i*s->c+j] != '*' && s->board[i*s->c+j] != '.'){
					s1 = d->Find(i*(s->c)+j);
					s2 = d->Find(i*(s->c)+(j+1));
					if(s1 != s2 ){
						d->Union(s1, s2);
					}
				}
			}
		}
	}
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
	Superball *s;
	DisjointSet *d;
	s = new Superball(argc, argv);
	d = new DisjointSetByRankWPC(s->c * s->r);

	//used to find the distance to the goal squares
	//each number represents the distance in moves to its closest goal
	vector<int> dist {2,2,3,4,5,5,4,3,2,2,
				      1,1,2,3,4,4,3,2,1,1,
					  0,0,1,2,3,3,2,1,0,0,
					  0,0,1,2,3,3,2,1,0,0,
		              0,0,1,2,3,3,2,1,0,0,
		              0,0,1,2,3,3,2,1,0,0,
		              1,1,2,3,4,4,3,2,1,1,
		              2,2,3,4,5,5,4,3,2,2,};

	s->ml = s->empty / s->mss;				//set the number of moves left
	s->same_color.resize(0);				//empty vector to be filled with similar colors that of the greatest set
	s->size.resize(s->c * s->r, 0);			//vector on grid with numbers represent size of the set
											//only the root will have the total size

	analyze(s, d);			//make the disjoint sets
	top_size(s, d);			//find the greatest set
	closest(s, d, dist);	//determine the sqaures that are the closest to the goal and swappable
							//also finds the nodes that are the same color as but not a part of the
							//greatest set
	swap_score(s, d);		//swap or score

	return 0;
}
