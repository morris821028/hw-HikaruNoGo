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
	void count_liberty(int X, int Y, int Liberties[MAXDIRECTION]);
	void count_neighboorhood_state(int X, int Y, 
		int turn, int* empt, int* self, int* oppo ,int* boun, 
		int NeighboorhoodState[MAXDIRECTION]);
	int remove_piece(int X, int Y, int turn);
	void update_board(int X, int Y, int turn);
	int legal_step(int X, int Y, int turn);
	void compute_comp_liberty(int comp_liberty[BOUNDARYSIZE][BOUNDARYSIZE]);
	void count_liberty_faster(int x, int y, int Liberties[], int comp_liberty[BOUNDARYSIZE][BOUNDARYSIZE]);
	int gen_legal_move(int turn, int game_length, set<mBoard> &GameRecord, int MoveList[]);
	int score();
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
	int count_liberty(int x, int y, int label, mBoard &ConnectBoard);
	int count_liberty_slow(int x, int y, int used[][BOUNDARYSIZE], int cases);
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
