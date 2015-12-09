#ifndef _SIMULATOR_H
#define _SIMULATOR_H

#include "MCTS.h"
#include "board.h"
#include <cstdio>
#include <cstdlib>

class Simulator {
public:
	MCTS mcts;
	void record(mBoard board, mBoard GameRecord[MAXGAMELENGTH], int game_length);
	int genmove(mBoard &board, int turn, int time_limit, 
		int game_length, mBoard GameRecord[MAXGAMELENGTH]);
};

#endif
