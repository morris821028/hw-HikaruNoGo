#ifndef _BOARD_H
#define _BOARD_H

#include "config.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <limits.h>
#include <assert.h>
#include <set>
using namespace std;

static int _board_size = BOARDSIZE;
static int _board_boundary = BOUNDARYSIZE;
static double _komi =  DEFAULTKOMI;
static const int DirectionX[MAXDIRECTION] = {-1, 1, 0, 0};
static const int DirectionY[MAXDIRECTION] = { 0, 0, 1,-1};

#define BITSIZE ((BOUNDARYSIZE*BOUNDARYSIZE*4)/32 + 1)
//#define BITSIZE 16
class mBoard {
public:
//	int B[BOUNDARYSIZE][BOUNDARYSIZE];
	unsigned int bitB[BITSIZE];	// (x, y) has 4 bits
	mBoard() {
		reset();
	}
	inline void CLRBOARD(int x, int y) {
		x = x * BOUNDARYSIZE + y;
		bitB[x>>3] &= ~(15U<<((x&7)<<2));
	}
	inline void ORBOARD(int x, int y, unsigned int v) {
		assert(v <= 15);
		x = x * BOUNDARYSIZE + y;
		bitB[x>>3] |= v<<((x&7)<<2);
	}
	inline int GETBOARD(int x, int y) {
		x = x * BOUNDARYSIZE + y;
		return (bitB[x>>3]>>((x&7)<<2))&15;
	}
	inline void SETBOARD(int x, int y, unsigned int v) {
		CLRBOARD(x, y);
		ORBOARD(x, y, v);
	}
	void reset() {
		memset(bitB, 0, sizeof(bitB));
		for (int i = 0; i < BOUNDARYSIZE; i++) {
			ORBOARD(0, i, BOUNDARY);
			ORBOARD(i, 0, BOUNDARY);
			ORBOARD(BOUNDARYSIZE-1, i, BOUNDARY);
			ORBOARD(i, BOUNDARYSIZE-1, BOUNDARY);
		}
	}
	/* O(n^2) = O(121) */
	void count_liberty(int X, int Y, int Liberties[MAXDIRECTION]) {
	    mBoard ConnectBoard;
	    memset(ConnectBoard.bitB, 0, sizeof(ConnectBoard.bitB));
	    for (int d = 0; d < MAXDIRECTION; ++d) {
	    	Liberties[d] = 0;
	    	int tx = X+DirectionX[d],
	    		ty = Y+DirectionY[d],
				state = GETBOARD(tx, ty);
			if (state == BLACK || state == WHITE) {
		    	Liberties[d] = count_liberty(tx, ty, d, ConnectBoard);
		    }
	    }
	}
	/* O(4) */
	void count_neighboorhood_state(int X, int Y, int turn, int* empt, int* self, int* oppo ,int* boun, int NeighboorhoodState[MAXDIRECTION]) {
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
	int remove_piece(int X, int Y, int turn) {
	    int remove_stones = (GETBOARD(X, Y) == EMPTY) ? 0 : 1;
	    CLRBOARD(X, Y);
	    for (int d = 0; d < MAXDIRECTION; ++d) {
	    	int tx = X + DirectionX[d],
	    		ty = Y + DirectionY[d];
			if (GETBOARD(tx, ty) == turn)
		    	remove_stones += remove_piece(tx, ty, turn);
		}
	    return remove_stones;
	}
	void update_board(int X, int Y, int turn) {
	    int num_neighborhood_self = 0,
			num_neighborhood_oppo = 0,
			num_neighborhood_empt = 0,
			num_neighborhood_boun = 0;
	    int Liberties[4];
	    int NeighboorhoodState[4];
	    count_neighboorhood_state(X, Y, turn,
		    &num_neighborhood_empt,
		    &num_neighborhood_self,
		    &num_neighborhood_oppo,
		    &num_neighborhood_boun, NeighboorhoodState);
	    if (num_neighborhood_oppo != 0) {
			count_liberty(X, Y, Liberties);
			for (int d = 0 ; d < MAXDIRECTION; ++d) {
				int tx = X + DirectionX[d],
					ty = Y + DirectionY[d];
		    	if (GETBOARD(tx, ty) != EMPTY && NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1)
					remove_piece(tx, ty, GETBOARD(tx, ty));
			}
	    }
	    SETBOARD(X, Y, turn);
	}
	/*
	 * This function update Board with place turn's piece at (X,Y).
	 * Note that this function will check if (X, Y) is a legal move or not.
	 * */
	int legal_step(int X, int Y, int turn) {
	    // Check the given coordination is legal or not
	    if (X < 1 || X > BOARDSIZE || Y < 1 || Y > BOARDSIZE || GETBOARD(X, Y) != EMPTY)
			return 0;
	    int num_neighborhood_self = 0,
			num_neighborhood_oppo = 0,
			num_neighborhood_empt = 0,
	    	num_neighborhood_boun = 0;
	    int Liberties[4];
	    int NeighboorhoodState[4];
	    count_neighboorhood_state(X, Y, turn,
		    &num_neighborhood_empt,
		    &num_neighborhood_self,
		    &num_neighborhood_oppo,
		    &num_neighborhood_boun, NeighboorhoodState);
		    
	    int legal_flag = 0;
	    count_liberty(X, Y, Liberties);
	    
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
			    		remove_piece(tx, ty, GETBOARD(tx, ty));
		    	}
			}
			SETBOARD(X, Y, turn);
	    }
	    return legal_flag == 1;
	}
	int gen_legal_move(int turn, int game_length, set<mBoard> &GameRecord, int MoveList[]) {
	    mBoard NextBoard;
	    int num_neighborhood_self = 0,
	    	num_neighborhood_oppo = 0,
	    	num_neighborhood_empt = 0,
	    	num_neighborhood_boun = 0;
	    int legal_moves = 0;
	    int next_x, next_y;
	    int Liberties[4];
	    int NeighboorhoodState[4];
	    bool eat_move = 0;
	    /* O(n^2 n^2) = O(10000) */
	    for (int x = 1 ; x <= BOARDSIZE; ++x) {
			for (int y = 1 ; y <= BOARDSIZE; ++y) {
				if (GETBOARD(x, y) != EMPTY)
					continue;
		    	// check if current
				// check the liberty of the neighborhood intersections
				num_neighborhood_self = num_neighborhood_oppo = 0;
				num_neighborhood_empt = num_neighborhood_boun = 0;
				// count the number of empy, self, opponent, and boundary neighboorhood
				count_neighboorhood_state(x, y, turn,
					&num_neighborhood_empt,
					&num_neighborhood_self,
					&num_neighborhood_oppo,
					&num_neighborhood_boun, NeighboorhoodState);
				// check if the emtpy intersection is a legal move
				next_x = next_y = 0;
				eat_move = 0;
				count_liberty(x, y, Liberties);
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
						int check_eye_flag = num_neighborhood_boun;
				    	// Avoid fill self eye
				    	// Check if there is one self component which has more than one liberty
						for (int d = 0 ; d < MAXDIRECTION; ++d) {
						    if (NeighboorhoodState[d] == SELF && Liberties[d] > 1)
								check_eye_flag++;
						}
						if (check_eye_flag >= 1 && check_eye_flag < 4)
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
						NextBoard.update_board(next_x, next_y, turn);
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
	/*
	 * This function counts the number of points remains in the board by Black's view
	 * */
	int score() {
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
	bool operator==(const mBoard &board) const {
		return memcmp(this->bitB, board.bitB, sizeof(board.bitB)) == 0;
	}
	bool operator<(const mBoard &board) const {
		for (int i = 0; i < BITSIZE; i++) {
			if (bitB[i] != board.bitB[i])
				return bitB[i] < board.bitB[i];
		}
		return false;
	}
private:
	/* 0 <= label <= 3, O(n^n) = O(121) */
	int count_liberty(int x, int y, int label, mBoard &ConnectBoard) {
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
		    	ret += count_liberty(tx, ty, label, ConnectBoard);
	    }
	    return ret;
	}
	void show() {
	    for (int i = 0; i < BOUNDARYSIZE; ++i) {
			cout << "#" << 10-i;
			for (int j = 0; j < BOUNDARYSIZE; ++j) {
			    switch (GETBOARD(i, j)) {
					case EMPTY: cout << " .";break;
					case BLACK: cout << " X";break;
					case WHITE: cout << " O";break;
					case BOUNDARY: cout << " -";break;
		    	}
			}
			cout << endl;
	    }
	    cout << endl << endl;
	}
};

#endif // _BOARD_H
