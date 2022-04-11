#include <iostream>
#include <vector>
using namespace std;

class BoardFolding
{
	public:
    	int howmany(int N, int M, vector <string> compressedPaper);
};

unsigned int tonumber(char c)
{
	if(c >= '0' && c <= '9') return c - '0';
    if(c >= 'a' && c <= 'z') return c - 'a' + 10;
    if(c >= 'A' && c <= 'Z') return c - 'A' + 36;
    if(c == '#') return 62;
    return 63;
}

int howMany(int N, int M, vector <string> compressedPaper)
{
    vector <string> Grid(M);
    
	int i, j;
    for(i = 0; i < N; i++){
		Grid[i].reserve(M);
        for(j=0; j < M; j++){
            Grid[i][j] = (tonumber(compressedPaper[i][j/6]) >> (j % 6)) % 2;
        }
    }
     
    for(i = 0; i < N; i++){
        for(j=0; j < M; j++){
            cout << Grid[i][j];
        }
        cout << endl;
    }
    return -1;
}

int main(int argc, char **argv)
{

	return 0;
}
