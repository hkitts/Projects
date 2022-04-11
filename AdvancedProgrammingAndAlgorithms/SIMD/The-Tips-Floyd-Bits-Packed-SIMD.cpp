/* Hunter Kitts cs494 Lab5 hkitts2
 * This is an adjustment to Dr. Planks solution.
 * It uses simd instructions and the Floyd-Warshall algorithm to make operations very fast.
 */

#include <iostream>
#include <vector>
#include <emmintrin.h>
#include "The-Tips.h"
using namespace std;

double TheTips::solve(vector <string> clues, vector <int> probability, int print)
{
	int i, j, v;
	vector < vector <char> > C;		//connectivity matrix
	vector <double> p;				//propability
	double x;						//used to calculated p
	size_t pad;						//used to determine size of C
	__m128i *a, *b;					//used for simd operations

	pad = clues.size() + 16 - (clues.size() % 16);

	/*
	if(print){
		for(size_t i =0; i < clues.size(); i++){
			printf("clues[%d]: %s\n", i, clues[i].c_str());
		}
		printf("\n");

		for(size_t j=0; j < probability.size(); j++){
			printf("probability[%d]: %d\n", j, probability[j]);
		}
	}
	*/

	C.resize(clues.size());

	for(i = 0; i < C.size(); i++){
		C[i].resize(pad);		//pad internal vectors to multiple of 16
		for(j = 0; j < C.size(); j++){
			C[i][j] = (clues[i][j] == 'Y') ? 1 : 0;
		}
		C[i][i] = 1;
	}

	if(print){
		printf("The Adjacency Matrix:\n\n");
		printf("    ");
		for(i = 0; i < C.size(); i++) printf("%X", i&0xf);
		printf("\n");
		printf("   -");
		for(i = 0; i < C.size(); i++) printf("-");
		printf("\n");
		for(i = 0; i < C.size(); i++){
			printf("%X | ", i&0xf);
			for(j = 0; j < C[i].size(); j++) printf("%d", C[i][j]);
			printf("\n");
		}
		printf("\n");
	}


	for(v=0; v < C.size(); v++){
		for(i=0; i < C.size(); i++){
			if(C[i][v]){
				//do the simd on every 16 bits
				for(j=0; j < C[0].size(); j += 16){
					a = (__m128i *) &C[i][j];
					b = (__m128i *) &C[v][j];
					*a = _mm_or_si128(*a, *b);
				}
			}
		}
	}

	if (print) {
		printf("Probabilities:\n\n");
		for (i = 0; i < probability.size(); i++) printf("%4d", probability[i]);
		printf("\n\n");
		printf("The Floyd-Warshall Matrix:\n\n");

		printf("    ");
		for (i = 0; i < C.size(); i++) printf("%X", i&0xf);
		printf("\n");
		printf("   -");
		for (i = 0; i < C.size(); i++) printf("-");
		printf("\n");
		for (i = 0; i < C.size(); i++) {
			printf("%X | ", i&0xf);
			for (j = 0; j < C[i].size(); j++) printf("%d", C[i][j]);
			printf("\n");
		}
		printf("\n");
	}

	p.resize(C.size(), 0);

	for (i = 0; i < C.size(); i++) {
		x = probability[i];
		x /= 100.0;
		for (j = 0; j < C.size(); j++) {
			if (C[i][j]) p[j] += ((1 - p[j]) * x);
		}
	}

	if (print) {
		printf("\nThe Expected Values\n\n");
		for (i = 0; i < C.size(); i++) {
			printf("I: %X    Prob: %7.5lf\n", i, p[i]);
		}
		printf("\n");
	}

	/* Calculate the final return value */

	x = 0;
	for (i = 0; i < C.size(); i++) x += p[i];

	return x;
}
