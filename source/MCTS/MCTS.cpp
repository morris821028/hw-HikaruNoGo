#include "MCTS.h"

Node* MCTS::newNode(mBoard board) {
	assert(nodesize < MAXNODES);
	Node *p = &_mem[nodesize++];
	*p = Node(), p->board = board;
	return p;
}
void MCTS::record(mBoard board, set<mBoard> &GameRecord) {
	GameRecord.insert(board);
}

/**
 *	control
 */
void MCTS::init(mBoard init_board, int game_length, int turn, mBoard GR[MAXGAMELENGTH]) {
	cases = 0;
	nodesize = 0;
	root = newNode(init_board);
	root->game_length = game_length, root->turn = turn;
	for (int i = 0; i < game_length; i++)
		oGameRecord.insert(GR[i]);
	iGameRecord.clear();
}
int MCTS::run(int time_limit) {
	srand(time(NULL));
	const int MAXSIM = 30;
	clock_t start_t, end_t;
	// record start time
	start_t = clock();
	end_t = start_t + CLOCKS_PER_SEC * time_limit;
	int rounds = 0;
	for (int it = 0; it < 1000; it++) {
		iGameRecord = oGameRecord;
		Node *leaf = selection(iGameRecord);
		expansion(leaf, iGameRecord);

		int chunk = 4;
#pragma omp parallel for schedule(dynamic, chunk)
		for (size_t i = 0; i < leaf->son.size(); i++) {
			if (clock() > end_t)
				continue;
			Node* p = leaf->son[i];
			int games = 0, posscr = 0, negscr = 0;
			float sum = 0, sqsum = 0;
			for (int j = 0; j < MAXSIM; j++) {
				set<mBoard> tGameRecord = iGameRecord;
				record(p->board, tGameRecord);
				int score = simulation(p, tGameRecord);
				sum += score, sqsum += score * score, games++;
				if (score > 0)
					posscr++;
				else
					negscr++;
				rounds++;
			}

			p->sum = sum, p->sqsum = sqsum;
			p->games = games;
			if (p->turn == BLACK)
				p->wins += posscr;
			else
				p->wins += negscr;
#pragma omp critical
			{
				rounds += games;
				backpropagation(leaf, games, sum, sqsum, posscr, negscr);
			}
		}
		if (clock() > end_t)
			break;
	}
	cerr << "rounds " << rounds << endl;
	if (root->son.size() == 0)
		return 0;

	decider.mount(root->turn, root->games);
	int move = root->move[0];
	Node *best = root->son[0], *tmp;
	// root->board.showLegalMove(root->turn);
	// for (size_t i = 0; i < root->son.size(); i++)
	// 	cerr << root->son[i]->UCB(root->games) << endl;
	for (size_t i = 1; i < root->son.size(); i++) {
		tmp = decider.want(root->son[i], best);
		if (tmp != best)
			best = tmp, move = root->move[i];
	}
	return move;
}	

/**
 *	Monte Carlo Tree Search Step: selection, expansion, simulation, backpropagation
 */
Node* MCTS::selection(set<mBoard> &sGameRecord) {
	Node *p = root;
	int select = 1;
	while (true) {
		if (p->son.size() == 0)	
			return p;
		decider.mount(p->turn, p->games);
		Node *q = p->son[0];
		for (size_t i = 1; i < p->son.size(); i++)
			q = decider.want(q, p->son[i]);
		q->parent = p;
		p = q;
		record(p->board, sGameRecord);
		select = 1 - select;
	}
	return NULL;
}
int MCTS::expansion(Node *leaf, set<mBoard> &eGameRecord) {
	int MoveList[HISTORYLENGTH];
	int num_legal_moves = leaf->board.legalMoves(leaf->turn, leaf->game_length, eGameRecord, MoveList);
	int ok = 0;
	for (int i = 0; i < num_legal_moves; i++) {
		if (leaf->game_length < 12) {
			int move = MoveList[i];
			int cx = BOUNDARYSIZE/2, cy = BOUNDARYSIZE/2;
			int tx = (move%100) / 10, ty = move % 10, dist = max(abs(tx - cx), abs(ty - cy));
			if (dist >= BOUNDARYSIZE/2 - 1)
				continue;
		}
		mBoard tmpBoard = leaf->board;
		mBoard::makeMove(tmpBoard, leaf->turn, MoveList[i]);
		Node *t = newNode(tmpBoard);
		t->game_length = leaf->game_length+1, t->turn = leaf->turn == BLACK ? WHITE : BLACK;
		leaf->son.push_back(t), leaf->move.push_back(MoveList[i]);
		t->parent = leaf;
		ok++;
	}
	return ok;
}
int MCTS::simulation(Node *leaf, set<mBoard> &tGameRecord) {
	mBoard tmpBoard = leaf->board;
	int game_length = leaf->game_length;
	int turn = leaf->turn, move;
	move = tmpBoard.legalMovesR(turn, game_length, tGameRecord, game_length < 12);
	// int limit = game_length * game_length;
	int limit = INT_MAX;
	while (true && limit >= 0) {
		if (move == 0) {
			int score = tmpBoard.score();
			return score;
		}
		mBoard::makeMove(tmpBoard, turn, move);
		record(tmpBoard, tGameRecord);
		game_length++;
		turn = turn == BLACK ? WHITE : BLACK;
		move = tmpBoard.legalMovesR(turn, game_length, tGameRecord, game_length < 12);
		limit--;
	}

	int score = tmpBoard.score();
	return score;
}
void MCTS::backpropagation(Node *p, int games, float sum, float sqsum, int posscr, int negscr) {
	while (p != NULL) {
		p->sum += sum, p->sqsum += sqsum;
		p->games += games;
		if (p->turn == BLACK)
			p->wins += posscr;
		else
			p->wins += negscr;
		p = p->parent;
	}
}
