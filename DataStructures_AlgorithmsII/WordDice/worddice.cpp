/* Hunter Kitts
 * worddice.cpp
 * given dice and words from a file using Edmonds-Karp finds if the words
 * in the file can be spelled by the dice
 *
 * Used Camille Crumpton's youtube vidoes for structure of the program 
 * and ideas on how to implement 
 * https://www.youtube.com/watch?v=-dVTf2bdSzM    
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <list>

using namespace std;


//used to set nodes as different types so they can be checked
typedef enum{SOURCE, SINK, DICE, WORD} nodeType;

class Node{
	public:
		nodeType type;				//type of node as seen above
		int id;						//index of node, source is 0, and sink will be the last
		int visited;				//used in bfs
		vector<bool> letters;		//26 units long, used to determin if a dice node had repeate letters
		vector<class Edge*> adj;	//edges adjacent to this node
		class Edge* backEdge;		//used to move along the graph backwards and check if word can be spelled
};

//edge that connect two nodes
class Edge{
	public:
		Node* to;					//where the edge is going
		Node* from;					//where the edge came from
		Edge* reverse;				//excat same edge going in the opposite direction

		//made these to follow network flow style, even through flow is all the same
		int original;				//originally set to 1
		int residual;				//originally set to 0

};

class Graph{
	public:
		bool BFS();
		bool canSpell();
		void resetGraph();

		int min;					//minimum number of nodes to spell the word
		vector<int> spellingIDs;	//the index of the dice used to spell the word
		vector<Node*> graph;		//a graph of nodes made up of the source, dice, letters, and sink
};

bool Graph::BFS(){

	Node *n;				//used to temperarily hold a node from the list to compare
	list<Node*> l;			//used to find a path
	size_t i;				//used in for loop

	//set all the backEdges and visited
	for(i = 0; i < graph.size(); i++){
		graph[i]->visited = 0;
		graph[i]->backEdge = NULL;
	}

	//push back the first node
	graph[0]->visited = 1;
	l.push_back(graph[0]);

	while(!l.empty()){
		//set the node to the first in the list and pop it out 
		n = l.front();
		l.pop_front();

		//if the node is the sink a path can be tested
		if(n->type == SINK){
			return true;
		}

		//if an adjacent node is not visited and original set to 1
		//set it to visited 
		//set its backedge
		//add it to the list
		for(i = 0; i < n->adj.size(); i++){
			if(n->adj[i]->to->visited == false && n->adj[i]->original == 1){
				n->adj[i]->to->visited = true;
				n->adj[i]->to->backEdge = n->adj[i];
				l.push_back(n->adj[i]->to);
			}
		}
	}

	return false;
}

/* Using the path found by bfs checks if the word can be spelled from the dice */
bool Graph::canSpell(){

	size_t i, j, k;		//i & j are used in for loops, k is used to set the node to the last node in the graph/the sink
	Node *n;			//used to temporarily hold a node to check its edges 
	Edge *e;			//holds the backedge of n

	while(BFS()){

		//set n to the last node in the graph
		k = graph.size() -1;
		n = graph[k];

		//follow the bacEdges to the source
		while(n->type != SOURCE){
			e = n->backEdge;

			//swap the edges original and reverse values
			if(e->original == 1 && e->residual == 0){
				e->original = 0;
				e->residual = 1;
				e->reverse->original = 1;
				e->reverse->residual = 0;

			}
			else if(e->original == 0 && e->residual == 1){
				e->original = 1;
				e->residual = 0;
				e->reverse->original = 0;
				e->reverse->residual = 1;
			}

			//set the node to its previous node by following the edge
			n = e->from;
		}
	}

	for(i = min; i < graph.size()-1; i++){
		for(j = 0; j < graph[i]->adj.size(); j++){

			//if an edge in the graph to the sink has not been set to residual return false
			if(graph[i]->adj[j]->to->type == SINK && graph[i]->adj[j]->residual == 0){
				return false;
			}

			//will only follow a path if it has not been taken before
			//if a node is not the source
			//and is connected to the dice type
			//and its original is 1
			else if(graph[i]->type != SOURCE &&
					graph[i]->adj[j]->to->type == DICE &&
					graph[i]->adj[j]->original == 1){
				//push its adjacent->to id into the spellingIDs
				spellingIDs.push_back(graph[i]->adj[j]->to->id -1);
			}
		}
	}
	return true;
}


/* Resets the graph so another word can be checked */
void Graph::resetGraph(){

	size_t i;			//used in for loops
	size_t min_t;		//set so min does not have to be cast several times

	min_t =(size_t)min;

	//remove the edges from the dice
	for(i = 1; i < min_t; i++){
		graph[i]->adj.clear();
	}
	//reset the original and residual
	for(i = 0; i < graph[0]->adj.size(); i++){
		graph[0]->adj[i]->original = 1;
		graph[0]->adj[i]->residual = 0;
	}
	//remove all the nodes from the graph
	for(i = graph.size()-1; i > min_t; i--){
		graph.pop_back();
	}
	graph.resize(min_t);
}


int main(int argc, char* argv[]){


	string input;							//used to get input from files
	ifstream inf;							//used to open file
	vector<Node*> dice;						//stores dice as nodes	
	vector<string> full_words;				//stores entire word as string from word file

	//individual letters that are derived from the full_words vector
	//this is reset to check another word in full_words after the result is determinded
	vector<Node*> ind_letters;			
	size_t i, j, k, l;						//used in for loops
	int idCount;
	Graph gi;

	idCount = 1;

	//open the dice file create new dice nodes
	inf.open(argv[1]);
	while(getline(inf, input)){
		Node* n = new Node;
		n->type = DICE;
		n->id = idCount;

		//if a letter is present in the die its index in letters is set to 1/true
		n->letters.resize(26);
		for(j = 0; j < input.size(); j++){
			n->letters[input[j]-65] = 1;
		}

		//push back the dice created until all dice are nodes in the dice vector
		dice.push_back(n);
		idCount++;
	}
	inf.close();


	//Open the words file and fill full_words with each line
	inf.open(argv[2]);
	while(getline(inf, input)){
		full_words.push_back(input);
	}


	/* For each full word go through and test if it can be written by the dice in
	 * the dice vector
	 * new source and sink nodes are made each loop
	 * ind_letters is changed each loop
	 */
	for(i = 0; i < full_words.size(); i++){

		//create a source node
		Node* source = new Node;
		source->type = SOURCE;
		source->id = 0;
		source->visited = 0;
		source->backEdge = NULL;

		//create a sink node
		Node* sink = new Node;
		sink->type = SINK;
		sink->id = dice.size() + full_words[i].size() + 1;
		sink->backEdge = NULL;

		//create nodes at each index of full_words and push them into the letters vector
		for(j = 0; j < full_words[i].size(); j++){
			Node *n = new Node;
			n->type = WORD;
			n->id = idCount;
			n->visited = 0;
			n->letters.resize(26);						//letters is boolean and set to 1 at the index of the current letter
			n->letters[full_words[i][j] -65] = 1;
			ind_letters.push_back(n);
			idCount++;
		}

		//Connect all die to the source, followed by connecting
		//all letter nodes to the sink

		//connect the dice to the source with edges
		for(j = 0; j < dice.size(); j++){

			//forward edge
			Edge * n = new Edge;
			n->to = dice[j];
			n->from = source;
			n->original = 1;
			n->residual = 0;

			//reverese edge
			Edge * r = new Edge;
			r->to = source;
			r->from = dice[j];
			r->original = 0;
			r->residual = 1;

			n->reverse = r;
			r->reverse = n;

			source->adj.push_back(n);
			dice[j]->adj.push_back(r);
		}

		//create edges for the individual letters to the sink
		for(j = 0; j < ind_letters.size(); j++){

			//forward edge
			Edge * n = new Edge;
			n->to = sink;
			n->from = ind_letters[j];
			n->original = 1;
			n->residual = 0;

			//reverse edge
			Edge * r = new Edge;
			r->to = ind_letters[j];
			r->from = sink;
			r->original = 0;
			r->residual = 1;

			n->reverse = r;
			r->reverse = n;

			ind_letters[j]->adj.push_back(n);
			sink->adj.push_back(r);
		}

		//create edges between the dice and individual letters
		for(j = 0; j < dice.size(); j++){
			for(l = 0; l < ind_letters.size(); l++){
				for(k = 0; k < dice[j]->letters.size(); k++){


					//create an edge if the current die letter
					//matches the current individual letter
					if(dice[j]->letters[k] && ind_letters[l]->letters[k]){

						//forward edge
						Edge * n = new Edge;
						n->to = ind_letters[l];
						n->from = dice[j];
						n->original = 1;
						n->residual = 0;

						//reverese edge
						Edge * r = new Edge;
						r->to = dice[j];
						r->from = ind_letters[l];
						r->original = 0;
						r->residual = 1;

						n->reverse = r;
						r->reverse = n;

						dice[j]->adj.push_back(n);
						ind_letters[l]->adj.push_back(r);
					}
				}
			}
		}

		//all of the edges and nodes have now been set
		//add all the nodes to the graph vector 
		gi.min = dice.size();
		gi.graph.push_back(source);
		//add the dice nodes
		for(j = 0; j < dice.size(); j++){
			gi.graph.push_back(dice[j]);
		}
		//add the individual lette nodes
		for(j = 0; j < ind_letters.size(); j++){
			gi.graph.push_back(ind_letters[j]);
		}
		gi.graph.push_back(sink);

		//if the word can be spelled
		if(gi.canSpell()){
			//print out the ids of the dice used to spell the word
			for(j = 0; j < gi.spellingIDs.size(); j++){
				if(j != 0){
					cout << "," << gi.spellingIDs[j];
				} else cout << gi.spellingIDs[j];
			}
			//print out the word
			cout << ": " << full_words[i] << endl;
		}
		//if the word can not be spelled
		else cout << "Cannot spell " << full_words.at(i) << endl;

		//clear out the individual letters, spellingIDs, and reset the graph
		//all this goes back to the for loop with the i variable to go to another word
		ind_letters.clear();
		gi.spellingIDs.clear();

		//resetGraph clears the edges and nodes out of the graph
		gi.resetGraph();
		//id reset to size after the dice
		idCount = dice.size() + 1;
		delete sink;

	}

	//end of for loop checking the words
	inf.close();
}
