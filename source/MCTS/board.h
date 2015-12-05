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

using namespace std;

static int _board_size = BOARDSIZE;
static int _board_boundary = BOUNDARYSIZE;
static double _komi =  DEFAULTKOMI;
static const int DirectionX[MAXDIRECTION] = {-1, 1, 0, 0};
static const int DirectionY[MAXDIRECTION] = { 0, 0, 1,-1};

class mBoard {
public:
	int B[BOUNDARYSIZE][BOUNDARYSIZE];
	mBoard() {
		reset();
	}
	unsigned long long hash() {
		unsigned long long ret = 0;
		unsigned long long a = 63689, b = 378551;
		for (int i = 1; i <= BOARDSIZE; i++) {
			for (int j = 1; j <= BOARDSIZE; j++) {
				ret = ret * a + B[i][j];
				a *= b;
			}
		}
		return ret;
	}
	/*
	 * This function reset the board, the board intersections are labeled with 0,
	 * the boundary intersections are labeled with 3.
	 * */
	void reset() {
		memset(B, EMPTY, sizeof(B));
	    for (int i = 0 ; i < BOUNDARYSIZE; i++) {
			B[0][i] = B[i][0] = BOUNDARY;
			B[BOUNDARYSIZE-1][i] = B[i][BOUNDARYSIZE-1] = BOUNDARY;
		}
	}
	/*
	 * This function count the liberties of the given intersection's neighboorhod
	 * */
	void count_liberty(int X, int Y, int Liberties[MAXDIRECTION]) {
	    int ConnectBoard[BOUNDARYSIZE][BOUNDARYSIZE];
	    // Initial the ConnectBoard
	    memset(ConnectBoard, 0, sizeof(ConnectBoard));
	    memset(Liberties, 0, sizeof(int)*MAXDIRECTION);
	    
	    // Find the same connect component and its liberity
	    for (int d = 0; d < MAXDIRECTION; ++d) {
	    	int tx = X+DirectionX[d],
	    		ty = Y+DirectionY[d];
			if (B[tx][ty] == BLACK || B[tx][ty] == WHITE)
		    	Liberties[d] = count_liberty(tx, ty, d, ConnectBoard);
	    }
	}
	/*
	 * This function count the number of empty, self, opponent, and boundary intersections of the neighboorhod
	 * and saves the type in NeighboorhoodState.
	 * */
	void count_neighboorhood_state(int X, int Y, int turn, int* empt, int* self, int* oppo ,int* boun, int NeighboorhoodState[MAXDIRECTION]) {
	    for (int d = 0 ; d < MAXDIRECTION; ++d) {
	    	int tx = X + DirectionX[d],
	    		ty = Y + DirectionY[d];
			// check the number of nonempty neighbor
			switch(B[tx][ty]) {
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
	/*
	 * This function remove the connect component contains (X, Y) with color "turn"
	 * And return the number of remove stones.
	 * */
	int remove_piece(int X, int Y, int turn) {
	    int remove_stones = (B[X][Y] == EMPTY) ? 0 : 1;
	    B[X][Y] = EMPTY;
	    for (int d = 0; d < MAXDIRECTION; ++d) {
	    	int tx = X + DirectionX[d],
	    		ty = Y + DirectionY[d];
			if (B[tx][ty] == turn)
		    	remove_stones += remove_piece(tx, ty, turn);
		}
	    return remove_stones;
	}
	/*
	 * This function update Board with place turn's piece at (X,Y).
	 * Note that this function will not check if (X, Y) is a legal move or not.
	 * */
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
	    // check if there is opponent piece in the neighboorhood
	    if (num_neighborhood_oppo != 0) {
			count_liberty(X, Y, Liberties);
			for (int d = 0 ; d < MAXDIRECTION; ++d) {
				int tx = X + DirectionX[d],
					ty = Y + DirectionY[d];
		    	// check if there is opponent component only one liberty
		    	if (B[tx][ty] != EMPTY && NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1)
					remove_piece(tx, ty, B[tx][ty]);
			}
	    }
	    B[X][Y] = turn;
	}
	/*
	 * This function update Board with place turn's piece at (X,Y).
	 * Note that this function will check if (X, Y) is a legal move or not.
	 * */
	int legal_step(int X, int Y, int turn) {
	    // Check the given coordination is legal or not
	    if (X < 1 || X > BOARDSIZE || Y < 1 || Y > BOARDSIZE || B[X][Y] != EMPTY)
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
	    // Check if the move is a legal move
	    // Condition 1: there is a empty intersection in the neighboorhood
	    int legal_flag = 0;
	    count_liberty(X, Y, Liberties);
	    if (num_neighborhood_empt != 0)
			legal_flag = 1;
	    else {
			// Condition 2: there is a self string has more than one liberty
			for (int d = 0; d < MAXDIRECTION; ++d) {
			    if (NeighboorhoodState[d] == SELF && Liberties[d] > 1)
					legal_flag = 1;
			}
			if (legal_flag == 0) {
			// Condition 3: there is a opponent string has exactly one liberty
			    for (int d = 0; d < MAXDIRECTION; ++d) {
					if (NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1)
				    legal_flag = 1;
			    }
			}
	    }
	
	    if (legal_flag == 1) {
	    	// check if there is opponent piece in the neighboorhood
			if (num_neighborhood_oppo != 0) {
		    	for (int d = 0 ; d < MAXDIRECTION; ++d) {
		    		int tx = X + DirectionX[d],
						ty = Y + DirectionY[d];
					// check if there is opponent component only one liberty
					if (NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1 && B[tx][ty]!=EMPTY)
			    		remove_piece(tx, ty, B[tx][ty]);
		    	}
			}
			B[X][Y] = turn;
	    }
	    return legal_flag == 1;
	}
	/*
	 * This function return the number of legal moves with clor "turn" and
	 * saves all legal moves in MoveList
	 * */
	int gen_legal_move(int turn, int game_length, int GameRecord[][BOUNDARYSIZE][BOUNDARYSIZE], int MoveList[]) {
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
	    for (int x = 1 ; x <= BOARDSIZE; ++x) {
			for (int y = 1 ; y <= BOARDSIZE; ++y) {
				if (B[x][y] != EMPTY)
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
						for (int d = 0 ; d < MAXDIRECTION; ++d) {
					    	// Avoid fill self eye
					    	// Check if there is one self component which has more than one liberty
						    if (NeighboorhoodState[d] == SELF && Liberties[d] > 1)
								check_eye_flag++;
						}
						if (check_eye_flag >= 1 && check_eye_flag != 4)
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
						NextBoard.B[x][y] = turn;
					// Check the history to avoid the repeat board
					bool repeat_move = 0;
					for (int t = 0 ; t < game_length; ++t) {
						bool repeat_flag = 1;
						for (int i = 1; i <= BOARDSIZE; ++i) {
						    for (int j = 1; j <= BOARDSIZE; ++j) {
								if (NextBoard.B[i][j] != GameRecord[t][i][j])
							    	repeat_flag = 0, i = j = BOARDSIZE;
							}
						}
						if (repeat_flag == 1) {
						   	repeat_move = 1;
						    break;
						}
					}
					if (repeat_move == 0) {
						// 3 digit zxy, z means eat or not, and put at (x, y)
						MoveList[legal_moves] = eat_move * 100 + next_x * 10 + y ;
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
			    switch(B[i][j]) {
					case EMPTY:
				    	is_black = is_white = 0;
				    	for (int d = 0 ; d < MAXDIRECTION; ++d) {
				    		int tx = i+DirectionX[d], 
								ty = j+DirectionY[d];
							if (B[tx][ty] == BLACK) is_black = 1;
							else if (B[tx][ty] == WHITE) is_white = 1;
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
private:
	/*
	 * This function return the total number of liberity of the string of (X, Y) and
	 * the string will be label with 'label'.
	 * */
	int count_liberty(int x, int y, int label, int ConnectBoard[][BOUNDARYSIZE]) {
	    // Label the current intersection
	    ConnectBoard[x][y] |= 1<<label;
	    int ret = 0;
	    for (int d = 0 ; d < MAXDIRECTION; d++) {
	    	int tx = x + DirectionX[d],
				ty = y + DirectionY[d];
			// Check this intersection has been visited or not
			if ((ConnectBoard[tx][ty]>>label)&1)
				continue;
			// Check this intersection is not visited yet
			ConnectBoard[tx][ty] |=(1<<label);
			if (B[tx][ty] == EMPTY) // This neighboorhood is empty
		    	ret++;
			else if (B[tx][ty] == B[x][y]) // This neighboorhood is self stone
		    	ret += count_liberty(tx, ty, label, ConnectBoard);
	    }
	    return ret;
	}
};

#endif // _BOARD_H
