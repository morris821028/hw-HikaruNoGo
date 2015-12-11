#ifndef _MCTS_H
#define _MCTS_H
 
#include "board.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <map>
#include <set>
using namespace std;

struct Node {
	// record data
	int games, wins;
	float sum, sqsum;
	mBoard board;
	int game_length, turn;
	vector<Node*> son;
	vector<int> move;
	Node *parent;
	Node() {
		son = vector<Node*>(), move = vector<int>();
		games = wins = 0, sum = sqsum = 0.f;
		parent = NULL;
	}
	float UCB(int N) {
		float ucb = wins * 1.f / games + 0.1 * sqrtf(log(N) / games + sqrtf(sqsum - 1.f * sum * sum / games));
		return sum / games * 0.8f + (turn == BLACK ? ucb : -ucb) * 0.2f;
	}
};
struct Picker {
	int N, select;
	inline Node* max(Node *a, Node *b) {
		return a->UCB(N) > b->UCB(N) ? a : b;
	}
	inline Node* min(Node *a, Node *b) {
		return a->UCB(N) < b->UCB(N) ? a : b;
	}
	Node* want(Node *a, Node *b) {
		if (select == BLACK)
			return max(a, b);
		else
			return min(a, b);
	}
	void mount(int select, int N) {
		this->select = select, this->N = N;
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
	void backpropagation(Node *leaf, int games, float sum, float sqsum, int posscr, int negscr);
};

#endif 
