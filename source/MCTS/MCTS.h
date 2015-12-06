#include "board.h"
#include <vector>
#include <cstring>
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
struct Result {
	int games, wins;
	float sum, sqsum;
};
#define MAXNODES 1024
class MCTS {
public:
	Node _mem[MAXNODES];
	Node *root;
	int cases, nodesize = 0;
	mBoard GameRecord[MAXGAMELENGTH];
	struct Pick {
		Node* (*pick)(Node*, Node*);
		static Node* max(Node *a, Node *b) {
			return a->UCB > b->UCB ? a : b;
		}
		static Node* min(Node *a, Node *b) {
			return a->UCB < b->UCB ? a : b;
		}
		void mount(int turn) {
			if (turn == BLACK)
				this->pick = Pick::max;
			else
				this->pick = Pick::min;
		}
	} decider;
	
	Node* newNode(mBoard board) {
		assert(nodesize < MAXNODES);
		Node *p = &_mem[nodesize++];
		*p = Node(), p->board = board;
		return p;
	}
	void init(mBoard init_board, int game_length, int turn, mBoard GR[MAXGAMELENGTH]) {
		cases = 0;
		nodesize = 0;
		root = newNode(init_board);
		root->game_length = game_length, root->turn = turn;
		for (int i = 0; i <= game_length; i++)
			memcpy(&GameRecord[i], &GR[i], sizeof(mBoard));
	}
	int rand_pick_move(int num_legal_moves, int MoveList[HISTORYLENGTH]) {
	    if (num_legal_moves == 0)
			return 0;
		int move_id = rand()%num_legal_moves;
		return MoveList[move_id];
	}
	void record(mBoard board, mBoard GameRecord[MAXGAMELENGTH], int game_length) {
		memcpy(&GameRecord[game_length], &board, sizeof(mBoard));
	}
	void do_move(mBoard &board, int turn, int move) {
	    int move_x = (move % 100) / 10, move_y = move % 10;
	    if (move < 100)
			board.SETBOARD(move_x, move_y, turn);
	    else
			board.update_board(move_x, move_y, turn);
	}
	int run() {
		srand(time(NULL));
		const int MAXSIM = 15;
		for (int it = 0; it < 10; it++) {
			Node *leaf = selection();
			int ways = expansion(leaf);
			for (int i = 0; i < leaf->son.size(); i++) {
				Node *p = leaf->son[i];
				float sum = 0, sqsum = 0;
				
				record(p->board, GameRecord, p->game_length+1);
				for (int j = 0; j < MAXSIM; j++) {
					int score = simulation(p);
					sum += score, sqsum += score * score;
				}
				p->sum = sum, p->sqsum = sqsum;
				p->games = MAXSIM;
				p->updatescore();
				backpropagation(p);
			}
		}
				
		if (root->son.size() == 0)
			return 0;
		
		decider.mount(root->turn);
		int move = root->move[0];
		Node *best = root->son[0], *tmp;
		for (int i = 1; i < root->son.size(); i++) {
			tmp = decider.pick(root->son[i], best);
			if (tmp != best)
				best = tmp, move = root->move[i];
		}
		return move;
	}	
	Node* selection() {
		Node *p = root;
		while (true) {
			if (p->son.size() == 0)	
				return p;
			decider.mount(p->turn);
			Node *q = p->son[0];
			for (int i = 1; i < p->son.size(); i++)
				q = decider.pick(q, p->son[i]);
			q->parent = p;
			p = q;
    		record(p->board, GameRecord, p->game_length+1);
		}
		return NULL;
	}
	int expansion(Node *leaf) {
		static int MoveList[HISTORYLENGTH];
		int num_legal_moves = leaf->board.gen_legal_move(leaf->turn, leaf->game_length, GameRecord, MoveList);
		int ok = 0;
		for (int i = 0; i < num_legal_moves; i++) {
			if (leaf->game_length < 12) {
				int move = MoveList[i];
				int cx = BOUNDARYSIZE/2, cy = BOUNDARYSIZE/2;
    			int tx = (move%100) / 10, ty = move % 10, dist = max(abs(tx - cx), abs(ty - cy));
    			if (dist >= BOUNDARYSIZE/2 - 1 || dist <= 1)
    				continue;
    		}
			Node *t = newNode(leaf->board);
			t->game_length = leaf->game_length+1, t->turn = leaf->turn == BLACK ? WHITE : BLACK;
			do_move(t->board, leaf->turn, MoveList[i]);
			leaf->son.push_back(t), leaf->move.push_back(MoveList[i]);
			t->parent = leaf;
			ok++;
		}
		return ok;
	}
	int simulation(Node *leaf) {
	    static int MoveList[HISTORYLENGTH];
		mBoard tmpBoard = leaf->board;
	    int num_legal_moves = 0, move;
	    int game_length = leaf->game_length;
	    int turn = leaf->turn;
	    num_legal_moves = tmpBoard.gen_legal_move(turn, game_length, GameRecord, MoveList);
	    
	    int limit = game_length * game_length;
	    while (true && limit >= 0) {
	    	if (num_legal_moves == 0) {
	    		int score = tmpBoard.score();
	    		return score;
			}
	    	move = rand_pick_move(num_legal_moves, MoveList);
	    	if (game_length < 12) {
				int cx = BOUNDARYSIZE/2, cy = BOUNDARYSIZE/2;
    			int tx = (move%100) / 10, ty = move % 10, dist = max(abs(tx - cx), abs(ty - cy));
    			if (dist >= BOUNDARYSIZE/2 - 1 || dist <= 1)
    				continue;
    		}
			do_move(tmpBoard, turn, move);
	        record(tmpBoard, GameRecord, game_length+1);
	        game_length++;
	        turn = turn == BLACK ? WHITE : BLACK;
	        num_legal_moves = tmpBoard.gen_legal_move(turn, game_length, GameRecord, MoveList);
	        limit--;
	    }
	    
	    int score = tmpBoard.score();
	    return score;
	}
	void backpropagation(Node *leaf) {
		Node *p = leaf->parent;
		int games = leaf->games;
		float sum = leaf->sum, sqsum = leaf->sqsum;
		while (p != NULL) {
			p->sum += sum, p->sqsum += sqsum;
			p->games += games;
			p->updatescore();
			p = p->parent;
		}
	}
};
