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
	/*
	 * This function update the Board with put 'turn' at (x,y)
	 * where x = (move % 100) / 10 and y = move % 10.
	 * Note this function will not check 'move' is legal or not.
	 * */
	void do_move(mBoard &board, int turn, int move) {
	    int move_x = (move % 100) / 10, move_y = move % 10;
	    if (move < 100)
			board.B[move_x][move_y] = turn;
	    else
			board.update_board(move_x, move_y, turn);
	}
	/*
	 * This function records the current game baord with current
	 * game length "game_length"
	 * */
	void record(mBoard board, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE], int game_length) {
		memmove(GameRecord[game_length], board.B, sizeof(int)*BOUNDARYSIZE*BOUNDARYSIZE);
	}
	/*
	 * This function randomly generate one legal move (x, y) with return value x*10+y,
	 * if there is no legal move the function will return 0.
	 * */
	int genmove(mBoard &board, int turn, int time_limit, int game_length, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE]) {
		mcts.init(board, game_length, turn, GameRecord);
		int return_move = mcts.run();
		do_move(board, turn, return_move);
		return return_move % 100;
	}
};
