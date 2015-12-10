#ifndef _MCTS_H
#define _MCTS_H
 
#include "board.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
#include <set>
using namespace std;

struct Node {
	// record data
	int games, wins;
	float UCB, sum, sqsum;
	mBoard board;
	int game_length, turn;
	vector<Node*> son;
	vector<int> move;
	Node *parent;
	Node() {
		son = vector<Node*>(), move = vector<int>();
		games = wins = 0;
		UCB = 0.f;
		parent = NULL;
	}
	void updatescore() {
		UCB = sum / games;
	}
};
struct Picker {
	Node* (*pick)(Node*, Node*);
	static Node* max(Node *a, Node *b) {
		return a->UCB > b->UCB ? a : b;
	}
	static Node* min(Node *a, Node *b) {
		return a->UCB < b->UCB ? a : b;
	}
	void mount(int turn) {
		if (turn == BLACK)
			this->pick = Picker::max;
		else
			this->pick = Picker::min;
	}
};

#define MAXNODES 32767
class MCTS {
public:
	Node _mem[MAXNODES];
	Node *root;
	int cases, nodesize;
	set<mBoard> oGameRecord;	// root
	set<mBoard> iGameRecord;	// iterator middle
	Picker decider;
	
	Node* newNode(mBoard board);
	void record(mBoard board, set<mBoard> &GameRecord);
	
	/**
	 *	control
	 */
	void init(mBoard init_board, int game_length, int turn, mBoard GR[MAXGAMELENGTH]);
	int run(int time_limit);
	
	/**
	 *	Monte Carlo Tree Search Step: selection, expansion, simulation, backpropagation
	 */
	Node* selection(set<mBoard> &sGameRecord);
	int expansion(Node *leaf, set<mBoard> &eGameRecord);
	int simulation(Node *leaf, set<mBoard> &tGameRecord);
	void backpropagation(Node *leaf);
};

#endif 
