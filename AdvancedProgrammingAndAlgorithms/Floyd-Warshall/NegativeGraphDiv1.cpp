/* Hunter Kitts hkitts2
 * cs494 - Lab 4 
 * Topcoder SRM 626, D1
 */

#include <iostream>
#include <cstdio>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
using namespace std;

long long findMin(int N, vector<long> &from, vector<long> &to, vector<long> &weight, long charges)
{
	size_t v, f, t, i, j, z;										//used in for loops

	vector < vector<long> > S(N, vector<long> (N, 6000000));		//shortest paths
	vector < vector<long> > L(N, vector<long> (N, 0));				//largest weight
	vector < vector<long> > A(N, vector<long> (N, 6000000));		//one negative edge

	//all used in final loop
	vector < vector<long> > Ai(N, vector<long> (N, 6000000));		
	vector < vector<long> > Aj(N, vector<long> (N, 6000000));
	vector < vector<long> > Ax(N, vector<long> (N, 6000000));

	long cur_charges = 0;
	int As;														//use to loop through Acalculated
	vector < vector < vector<long> > > Acalculated;			//Ax values that have been calculated

	//create adjacency matrix and L
	for(v = 0; v < from.size(); v++){
		if(weight[v] < S[ from[v] - 1 ][ to[v] - 1 ] ){
			S[ from[v] - 1 ][ to[v]- 1 ] = weight[v];
		}
		if(weight[v] > L[ from[v] - 1 ][ to[v] - 1 ] ){
			L[ from[v] - 1 ][ to[v] - 1 ] = weight[v];
		}
	}

	/*
	   for(i=0; i < S.size(); i++){
	   for(j=0; j < S.size(); j++){
	   printf("%10d", S[i][j]);
	   }
	   cout << endl;
	   }
	   */

	//Floyd-Warshall to correct S
	for(v = 0; v < S.size(); v++){
		for(i = 0; i < S.size(); i++){
			for(j = 0; j < S.size(); j++){
				if((S[i][v] + S[v][j]) < S[i][j]){
					S[i][j] = S[i][v] + S[v][j];
				}
			}
		}
		S[v][v] = 0;
	}
	/*
	   for(i=0; i < S.size(); i++){
	   for(j=0; j < S.size(); j++){
	   printf("%10ld", S[i][j]);
	   }
	   cout << endl;
	   }
	*/
	if(charges == 0){
		return S[0][N-1];
	}

	//create shortest path lengths with one negative edge used
	for(f = 0; f < S.size(); f++){
		for(t = 0; t < S.size(); t++){
			for(i = 0; i < S.size(); i++){
				for(j = 0; j < S.size(); j++){
					if(((L[i][j] * -1 + (S[f][i] + S[j][t])) < A[f][t]) && L[i][j] != 0){
					//if( ((S[f][i] + S[j][t]) < (L[i][j])) && ((L[i][j] * -1 + (S[f][i] + S[j][t])) < A[f][t])){
						A[f][t] = (L[i][j] * -1 + (S[f][i] + S[j][t]));
					}
				}
			}
		}
		if(A[f][f] == 6000000) A[f][f] = 0;
	}
	/*	
	   for(i=0; i < S.size(); i++){
	   for(j=0; j < S.size(); j++){
	   printf("%10ld", A[i][j]);
	   }
	   cout << endl;
	   }
	*/

	if(charges == 1){
		return A[0][N-1];
	}

	Ai = A;		
	Aj = A;
	Ax = A;				//value to be calculated
	cur_charges = 1;

	while(cur_charges < charges){

		//see if Ai can be doubled (example Ai = A2 -> Aj = A4)
		if((cur_charges*2) <= charges){

			//if so the previous Ax value can be used
			//set both Ai and Aj to Ax so when Floyd-Warshall
			//is run Ax is calculated against Ax
			Ai = Ax;
			Aj = Ax;

			Acalculated.push_back(Ax);
			cur_charges = cur_charges *2;
		}

		//have to find a value in Acalculated that can be used
		else{
			for(As = Acalculated.size() - 1; As >= 0; As--){
				//look for power that can be used to increase charges
				if(cur_charges + pow(2,As) <= charges){	
					//set Ai to the saved value of Ax 
					Ai = Acalculated[As];
					//set Aj to Ax that was calculated in previous loop
					Aj = Ax;

					cur_charges += pow(2,As);
					break;
				}
			}

		}
		//do Floyd-Warshall esc calculation
		//use Ai (t to z) and Aj (z to t) to get Ax
		for(f = 0; f < Ax.size(); f++){
			for(t = 0; t < Ax.size(); t++){
				for(z = 0; z < Ax.size(); z++){
					Ax[f][t] = min(Ax[f][t], Ai[f][z] + Aj[z][t]);
				}
			}
		}
	}




	return Ax[0][N-1];

}


int main()
{
	istringstream ss;
	long N, charges, num;
	long long r;
	string line;
	vector<long> from;
	vector<long> to;
	vector<long> weight;

	getline(cin, line);
	ss.clear();
	ss.str(line);
	if(ss >> N){}

	getline(cin, line);
	from.clear();
	ss.clear();
	ss.str(line);
	while(ss >> num) from.push_back(num);

	getline(cin, line);
	to.clear();
	ss.clear();
	ss.str(line);
	while(ss >> num) to.push_back(num);

	getline(cin, line);
	weight.clear();
	ss.clear();
	ss.str(line);
	while(ss >> num) weight.push_back(num);

	getline(cin, line);
	ss.clear();
	ss.str(line);
	if(ss >> charges){}

	r = findMin(N, from, to, weight, charges);
	printf("%lld\n", r);
	return 0;
}
