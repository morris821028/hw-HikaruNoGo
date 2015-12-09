#ifndef _BOARD_H
#define _BOARD_H

#include "config.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <limits.h>
#include <assert.h>
#include <set>
using namespace std;

#define BITSIZE ((BOUNDARYSIZE*BOUNDARYSIZE*4)/32 + 1)
class mBoard {
public:
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
	
	/**
	 *	liberty computing
	 */
	void countLibertySingle(int X, int Y, int Liberties[MAXDIRECTION]);
	void buildLibertyGraph(int comp_liberty[BOUNDARYSIZE][BOUNDARYSIZE]);
	void fetchLibertyBy(int x, int y, int Liberties[], int comp_liberty[BOUNDARYSIZE][BOUNDARYSIZE]);
	int searchLibertySingleBy(int x, int y, int label, mBoard &ConnectBoard);
	int searchLibertySlow(int x, int y, int used[][BOUNDARYSIZE], int cases);
	
	/**
	 *	simulate main
	 */
	void transferBoard(int X, int Y, int turn);
	int removeComponent(int X, int Y, int turn);
	void getAdjacencyState(int X, int Y, int turn, 
		int* empt, int* self, int* oppo ,int* boun, int NeighboorhoodState[MAXDIRECTION]);
	/**
	 *	simulate helper
	 */
	static int randMove(int moveSize, int MoveList[HISTORYLENGTH]) {
	    if (moveSize == 0)	return 0;
		return MoveList[rand()%moveSize];
	}
	static void makeMove(mBoard &board, int turn, int move) {
	    int move_x = (move % 100) / 10, move_y = move % 10;
	    if (move < 100)
			board.SETBOARD(move_x, move_y, turn);
	    else
			board.transferBoard(move_x, move_y, turn);
	}
	/**
	 *	generator next state
	 */
	int isLegalMove(int X, int Y, int turn);
	int legalMoves(int turn, int game_length, set<mBoard> &GameRecord, int MoveList[]);
	
	/**
	 *	evaluation function
	 */
	int score();
	
	/**
	 *	board configuration compare
  	 */
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
	
	/**
	 *	output helper
	 */
	void showLegalMove(int turn) {
		int MoveList[HISTORYLENGTH];
		set<mBoard> S;
		int n = legalMoves(turn, 0, S, MoveList);
		int tmp[BOUNDARYSIZE][BOUNDARYSIZE] = {};
		for (int i = 0; i < n; i++) {
			int move = MoveList[i];
			tmp[move%100 / 10][move % 10] = 1;
		}
		for (int i = 0; i < BOUNDARYSIZE; ++i) {
			cout << "#" << 10-i;
			for (int j = 0; j < BOUNDARYSIZE; ++j) {
				if (tmp[i][j] == 1) {
					cout << " +";
					continue;
				}
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
