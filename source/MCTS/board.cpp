#include "board.h"
#include "config.h"

static const int DirectionX[MAXDIRECTION] = {-1, 1, 0, 0};
static const int DirectionY[MAXDIRECTION] = { 0, 0, 1,-1};
/**
 *	liberty computing
 */
void mBoard::countLibertySingle(int X, int Y, int Liberties[MAXDIRECTION]) {
	mBoard ConnectBoard;
	memset(ConnectBoard.bitB, 0, sizeof(ConnectBoard.bitB));
	for (int d = 0; d < MAXDIRECTION; ++d) {
		Liberties[d] = 0;
		int tx = X+DirectionX[d],
			ty = Y+DirectionY[d],
			state = GETBOARD(tx, ty);
		if (state == BLACK || state == WHITE) {
			Liberties[d] = searchLibertySingleBy(tx, ty, d, ConnectBoard);
		}
	}
}
void mBoard::buildLibertyGraph(int comp_liberty[BOUNDARYSIZE][BOUNDARYSIZE]) {
	struct Disjoint_Set {
		int parent[BOUNDARYSIZE * BOUNDARYSIZE];
		int libert[BOUNDARYSIZE * BOUNDARYSIZE];
		void init(int n) {
			for (int i = 0; i < n; i++)
				parent[i] = i;
		}
		int findp(int x) {
			return parent[x] == x ? x : (parent[x] = findp(parent[x]));
		}
		void joint(int x, int y) {
			x = findp(x), y = findp(y);
			if (x == y)	return ;
			parent[x] = y;
		}
	} DisjointSet;
	int cases = 0, used[BOUNDARYSIZE][BOUNDARYSIZE] = {};
	memset(comp_liberty, 0, sizeof(int)*BOUNDARYSIZE*BOUNDARYSIZE);
	DisjointSet.init(BOUNDARYSIZE * BOUNDARYSIZE);
	for (int i = 1; i <= BOARDSIZE; i++) {
		for (int j = 1; j <= BOARDSIZE; j++) {
			if (GETBOARD(i, j) == EMPTY)
				continue;
			for (int k = 0; k < MAXDIRECTION; k++) {
				int tx = i+DirectionX[k],
					ty = j+DirectionY[k];
				if (GETBOARD(i, j) == GETBOARD(tx, ty)) {
					DisjointSet.joint(i*BOUNDARYSIZE+j, tx*BOUNDARYSIZE+ty);
				}
			}
		}
	}
	for (int i = 1; i <= BOARDSIZE; i++) {
		for (int j = 1; j <= BOARDSIZE; j++) {
			if (GETBOARD(i, j) == EMPTY)
				continue;
			int x = i*BOUNDARYSIZE+j;

			if (DisjointSet.parent[x] == x) {
				cases++;
				DisjointSet.libert[x] = searchLibertySlow(i, j, used, cases);
			}
		}
	}
	for (int i = 1; i <= BOARDSIZE; i++) {
		for (int j = 1; j <= BOARDSIZE; j++) {
			if (GETBOARD(i, j) == EMPTY)
				continue;
			int x = i*BOUNDARYSIZE+j;
			comp_liberty[i][j] = DisjointSet.libert[DisjointSet.findp(x)];
		}
	}
}

/* 0 <= label <= 3, O(n^n) = O(121) */
int mBoard::searchLibertySingleBy(int x, int y, int label, mBoard &ConnectBoard) {
	ConnectBoard.ORBOARD(x, y, 1<<label);
	int ret = 0;
	for (int d = 0 ; d < MAXDIRECTION; d++) {
		int tx = x + DirectionX[d],
			ty = y + DirectionY[d];
		if ((ConnectBoard.GETBOARD(tx, ty)>>label)&1)
			continue;
		ConnectBoard.ORBOARD(tx, ty, 1<<label);
		if (GETBOARD(tx, ty) == EMPTY)
			ret++;
		else if (GETBOARD(tx, ty) == GETBOARD(x, y))
			ret += searchLibertySingleBy(tx, ty, label, ConnectBoard);
	}
	return ret;
}

int mBoard::searchLibertySlow(int x, int y, int used[][BOUNDARYSIZE], int cases) {
	used[x][y] = cases;
	int ret = 0;
	for (int d = 0 ; d < MAXDIRECTION; d++) {
		int tx = x + DirectionX[d],
			ty = y + DirectionY[d];
		if (used[tx][ty] == cases)
			continue;
		used[tx][ty] = cases;
		if (GETBOARD(tx, ty) == EMPTY)
			ret++;
		else if (GETBOARD(tx, ty) == GETBOARD(x, y))
			ret += searchLibertySlow(tx, ty, used, cases);
	}
	return ret;
}

void mBoard::fetchLibertyBy(int x, int y, int Liberties[], int comp_liberty[BOUNDARYSIZE][BOUNDARYSIZE]) {
	for (int d = 0; d < MAXDIRECTION; ++d) {
		Liberties[d] = 0;
		int tx = x+DirectionX[d],
			ty = y+DirectionY[d],
			state = GETBOARD(tx, ty);
		if (state == BLACK || state == WHITE)
			Liberties[d] = comp_liberty[tx][ty];
	}
}

/**
 *	simulate main
 */
void mBoard::getAdjacencyState(int X, int Y, int turn, 
		int* empt, int* self, int* oppo ,int* boun, 
		int NeighboorhoodState[MAXDIRECTION]) {
	for (int d = 0 ; d < MAXDIRECTION; ++d) {
		int tx = X + DirectionX[d],
		ty = Y + DirectionY[d];
		switch(GETBOARD(tx, ty)) {
			case EMPTY:    
				(*empt)++, NeighboorhoodState[d] = EMPTY;
				break;
			case BLACK:
				if (turn == BLACK)
					(*self)++, NeighboorhoodState[d] = SELF;
				else
					(*oppo)++, NeighboorhoodState[d] = OPPONENT;
				break;
			case WHITE:
				if (turn == WHITE)
					(*self)++, NeighboorhoodState[d] = SELF;
				else
					(*oppo)++, NeighboorhoodState[d] = OPPONENT;
				break;
			case BOUNDARY: 
				(*boun)++, NeighboorhoodState[d] = BOUNDARY;
				break;
		}
	}
}

int mBoard::removeComponent(int X, int Y, int turn) {
	int remove_stones = (GETBOARD(X, Y) == EMPTY) ? 0 : 1;
	CLRBOARD(X, Y);
	for (int d = 0; d < MAXDIRECTION; ++d) {
		int tx = X + DirectionX[d],
			ty = Y + DirectionY[d];
		if (GETBOARD(tx, ty) == turn)
			remove_stones += removeComponent(tx, ty, turn);
	}
	return remove_stones;
}

void mBoard::transferBoard(int X, int Y, int turn) {
	int num_neighborhood_self = 0,
		num_neighborhood_oppo = 0,
		num_neighborhood_empt = 0,
		num_neighborhood_boun = 0;
	int Liberties[4];
	int NeighboorhoodState[4];
	getAdjacencyState(X, Y, turn,
			&num_neighborhood_empt,
			&num_neighborhood_self,
			&num_neighborhood_oppo,
			&num_neighborhood_boun, NeighboorhoodState);
	if (num_neighborhood_oppo != 0) {
		countLibertySingle(X, Y, Liberties);
		for (int d = 0 ; d < MAXDIRECTION; ++d) {
			int tx = X + DirectionX[d],
				ty = Y + DirectionY[d];
			if (GETBOARD(tx, ty) != EMPTY && NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1)
				removeComponent(tx, ty, GETBOARD(tx, ty));
		}
	}
	SETBOARD(X, Y, turn);
}

int mBoard::isLegalMove(int X, int Y, int turn) {
	if (X < 1 || X > BOARDSIZE || Y < 1 || Y > BOARDSIZE || GETBOARD(X, Y) != EMPTY)
		return 0;
	int num_neighborhood_self = 0,
		num_neighborhood_oppo = 0,
		num_neighborhood_empt = 0,
		num_neighborhood_boun = 0;
	int Liberties[4];
	int NeighboorhoodState[4];
	getAdjacencyState(X, Y, turn,
			&num_neighborhood_empt,
			&num_neighborhood_self,
			&num_neighborhood_oppo,
			&num_neighborhood_boun, NeighboorhoodState);

	int legal_flag = 0;
	countLibertySingle(X, Y, Liberties);

	if (num_neighborhood_empt != 0)
		legal_flag = 1;
	else {
		for (int d = 0; d < MAXDIRECTION; ++d) {
			if (NeighboorhoodState[d] == SELF && Liberties[d] > 1)
				legal_flag = 1;
		}
		if (legal_flag == 0) {
			for (int d = 0; d < MAXDIRECTION; ++d) {
				if (NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1)
					legal_flag = 1;
			}
		}
	}

	if (legal_flag == 1) {
		if (num_neighborhood_oppo != 0) {
			for (int d = 0 ; d < MAXDIRECTION; ++d) {
				int tx = X + DirectionX[d],
					ty = Y + DirectionY[d];
				if (NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1 && GETBOARD(tx, ty) != EMPTY)
					removeComponent(tx, ty, GETBOARD(tx, ty));
			}
		}
		SETBOARD(X, Y, turn);
	}
	return legal_flag == 1;
}

/**
 *	generator next state
 */
int mBoard::legalMoves(int turn, int game_length, set<mBoard> &GameRecord, int MoveList[]) {
	int comp_liberty[BOUNDARYSIZE][BOUNDARYSIZE];
	mBoard NextBoard;
	int num_neighborhood_self = 0,
		num_neighborhood_oppo = 0,
		num_neighborhood_empt = 0,
		num_neighborhood_boun = 0;
	int legal_moves = 0;
	int next_x, next_y;
	int Liberties[4] = {};
	int NeighboorhoodState[4] = {};
	bool eat_move = 0;
	buildLibertyGraph(comp_liberty);
	for (int x = 1 ; x <= BOARDSIZE; ++x) {
		for (int y = 1 ; y <= BOARDSIZE; ++y) {
			if (GETBOARD(x, y) != EMPTY)
				continue;
			// check if current
			// check the liberty of the neighborhood intersections
			num_neighborhood_self = num_neighborhood_oppo = 0;
			num_neighborhood_empt = num_neighborhood_boun = 0;
			// count the number of empy, self, opponent, and boundary neighboorhood
			getAdjacencyState(x, y, turn,
					&num_neighborhood_empt,
					&num_neighborhood_self,
					&num_neighborhood_oppo,
					&num_neighborhood_boun, NeighboorhoodState);
			// check if the emtpy intersection is a legal move
			next_x = next_y = 0;
			eat_move = 0;
			fetchLibertyBy(x, y, Liberties, comp_liberty);
			// Case 1: exist empty intersection in the neighborhood
			if (num_neighborhood_empt > 0) {
				next_x = x, next_y = y;
				// check if it is a capture move
				for (int d = 0 ; d < MAXDIRECTION; ++d) {
					if (NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1)
						eat_move = 1;
				}
			} else {
				// Case 2: no empty intersection in the neighborhood
				// Case 2.1: Surround by the self piece
				if (num_neighborhood_self + num_neighborhood_boun == MAXDIRECTION) {
					int check_eye_flag = num_neighborhood_boun, extra_eye_flag = 0;
					// Avoid fill self eye
					// Check if there is one self component which has more than one liberty
					for (int d = 0 ; d < MAXDIRECTION; ++d) {
						if (NeighboorhoodState[d] == SELF && Liberties[d] > 1)
							check_eye_flag++, extra_eye_flag = 1;
					}
					if (extra_eye_flag >= 1 && check_eye_flag < 4)
						next_x = x, next_y = y;
				} else if (num_neighborhood_oppo > 0) {
					// Case 2.2: Surround by opponent or both side's pieces.
					int check_flag = 0;
					int eat_flag = 0;
					for (int d = 0 ; d < MAXDIRECTION; ++d) {
						// Check if there is one self component which has more than one liberty
						if (NeighboorhoodState[d] == SELF && Liberties[d] > 1)
							check_flag = 1;
						// Check if there is one opponent's component which has exact one liberty
						if (NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1)
							eat_flag = 1;
					}
					if (check_flag == 1) {
						next_x = x, next_y = y;
						if (eat_flag == 1)
							eat_move = 1;
					} else { // check_flag == 0
						if (eat_flag == 1) {
							next_x = x, next_y = y;
							eat_move = 1;
						}
					}
				}
			}

			if (next_x != 0 && next_y != 0) {
				// copy the current board to next board
				NextBoard = *this;
				// do the move
				// The move is a capture move and the board needs to be updated.
				if (eat_move == 1)
					NextBoard.transferBoard(next_x, next_y, turn);
				else
					NextBoard.SETBOARD(x, y, turn);
				// Check the history to avoid the repeat board
				bool repeat_move = GameRecord.count(NextBoard) != 0;
				if (repeat_move == 0) {
					// 3 digit zxy, z means eat or not, and put at (x, y)
					MoveList[legal_moves] = eat_move * 100 + next_x * 10 + next_y;
					legal_moves++;
				}
			}
		}
	}
	return legal_moves;
}
int mBoard::legalMovesR(int turn, int game_length, set<mBoard> &GameRecord, int filter) {
	mBoard NextBoard;
	int num_neighborhood_self = 0,
		num_neighborhood_oppo = 0,
		num_neighborhood_empt = 0,
		num_neighborhood_boun = 0;
	int next_x, next_y;
	int Liberties[4] = {};
	int NeighboorhoodState[4] = {};
	bool eat_move = 0;
	pair<int, int> EMXY[BOARDSIZE*BOARDSIZE];
	int EMSZ = 0;
	for (int x = 1 ; x <= BOARDSIZE; ++x) {
		for (int y = 1 ; y <= BOARDSIZE; ++y) {
			if (GETBOARD(x, y) != EMPTY)
				continue;
			if (filter == 1) {
				int cx = BOUNDARYSIZE/2, cy = BOUNDARYSIZE/2;
				int dist = max(abs(x - cx), abs(y - cy));
				if (dist >= BOUNDARYSIZE/2 - 1)
					continue;
			}
			EMXY[EMSZ] = {x, y}, EMSZ++;
		}
	}
	random_shuffle(EMXY, EMXY + EMSZ);
	for (int it = 0, x, y; it < EMSZ; it++) {
		x = EMXY[it].first, y = EMXY[it].second;
		// check if current
		// check the liberty of the neighborhood intersections
		num_neighborhood_self = num_neighborhood_oppo = 0;
		num_neighborhood_empt = num_neighborhood_boun = 0;
		// count the number of empy, self, opponent, and boundary neighboorhood
		getAdjacencyState(x, y, turn,
				&num_neighborhood_empt,
				&num_neighborhood_self,
				&num_neighborhood_oppo,
				&num_neighborhood_boun, NeighboorhoodState);
		// check if the emtpy intersection is a legal move
		next_x = next_y = 0;
		eat_move = 0;
		countLibertySingle(x, y, Liberties);
		// Case 1: exist empty intersection in the neighborhood
		if (num_neighborhood_empt > 0) {
			next_x = x, next_y = y;
			// check if it is a capture move
			for (int d = 0 ; d < MAXDIRECTION; ++d) {
				if (NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1)
					eat_move = 1;
			}
		} else {
			// Case 2: no empty intersection in the neighborhood
			// Case 2.1: Surround by the self piece
			if (num_neighborhood_self + num_neighborhood_boun == MAXDIRECTION) {
				int check_eye_flag = num_neighborhood_boun, extra_eye_flag = 0;
				// Avoid fill self eye
				// Check if there is one self component which has more than one liberty
				for (int d = 0 ; d < MAXDIRECTION; ++d) {
					if (NeighboorhoodState[d] == SELF && Liberties[d] > 1)
						check_eye_flag++, extra_eye_flag = 1;
				}
				if (extra_eye_flag >= 1 && check_eye_flag < 4)
					next_x = x, next_y = y;
			} else if (num_neighborhood_oppo > 0) {
				// Case 2.2: Surround by opponent or both side's pieces.
				int check_flag = 0;
				int eat_flag = 0;
				for (int d = 0 ; d < MAXDIRECTION; ++d) {
					// Check if there is one self component which has more than one liberty
					if (NeighboorhoodState[d] == SELF && Liberties[d] > 1)
						check_flag = 1;
					// Check if there is one opponent's component which has exact one liberty
					if (NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1)
						eat_flag = 1;
				}
				if (check_flag == 1) {
					next_x = x, next_y = y;
					if (eat_flag == 1)
						eat_move = 1;
				} else { // check_flag == 0
					if (eat_flag == 1) {
						next_x = x, next_y = y;
						eat_move = 1;
					}
				}
			}
		}

		if (next_x != 0 && next_y != 0) {
			// copy the current board to next board
			NextBoard = *this;
			// do the move
			// The move is a capture move and the board needs to be updated.
			if (eat_move == 1)
				NextBoard.transferBoard(next_x, next_y, turn);
			else
				NextBoard.SETBOARD(x, y, turn);
			// Check the history to avoid the repeat board
			bool repeat_move = GameRecord.count(NextBoard) != 0;
			if (repeat_move == 0) {
				// 3 digit zxy, z means eat or not, and put at (x, y)
				return eat_move * 100 + next_x * 10 + next_y;
			}
		}
	}
	return 0;
}

/*
 * This function counts the number of points remains in the board by Black's view
 * */
int mBoard::score() {
	int is_black, is_white;
	int black, white;
	black = white = 0;
	for (int i = 1 ; i <= BOARDSIZE; ++i) {
		for (int j = 1; j <= BOARDSIZE; ++j) {
			switch (GETBOARD(i, j)) {
				case EMPTY:
					is_black = is_white = 0;
					for (int d = 0 ; d < MAXDIRECTION; ++d) {
						int tx = i+DirectionX[d], 
							ty = j+DirectionY[d];
						if (GETBOARD(tx, ty) == BLACK) is_black = 1;
						else if (GETBOARD(tx, ty) == WHITE) is_white = 1;
					}
					if (is_black + is_white == 1)
						black += is_black, white += is_white;
					break;
				case WHITE:
					white++;
					break;
				case BLACK:
					black++;
					break;
			}
		}
	}
	return black - white;
}
