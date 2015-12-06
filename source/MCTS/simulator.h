#include "board.h"
#include "MCTS.h"
#include <cstdio>
#include <cstdlib>

static MCTS mcts;
class Simulator {
public:
	void init(mBoard board, int turn, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE], int game_length) {
		
	}
	/*
	 * This function randomly selects one move from the MoveList.
	 * */
	int rand_pick_move(int num_legal_moves, int MoveList[HISTORYLENGTH]) {
	    if (num_legal_moves == 0)
			return 0;
		int move_id = rand()%num_legal_moves;
		return MoveList[move_id];
	}
	void do_move(mBoard &board, int turn, int move) {
	    int move_x = (move % 100) / 10, move_y = move % 10;
	    if (move < 100)
			board.SETBOARD(move_x, move_y, turn);
	    else
			board.update_board(move_x, move_y, turn);
	}
	/*
	 * This function records the current game baord with current
	 * game length "game_length"
	 * */
	void record(mBoard board, mBoard GameRecord[MAXGAMELENGTH], int game_length) {
		memcpy(&GameRecord[game_length], &board, sizeof(mBoard));
	}
	/*
	 * This function randomly generate one legal move (x, y) with return value x*10+y,
	 * if there is no legal move the function will return 0.
	 * */
	int genmove(mBoard &board, int turn, int time_limit, int game_length, mBoard GameRecord[MAXGAMELENGTH]) {
		mcts.init(board, game_length, turn, GameRecord);
		int return_move = mcts.run();
		do_move(board, turn, return_move);
		return return_move % 100;
	}
};
