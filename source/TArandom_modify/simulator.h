#include "board.h"
#include <cstdio>
#include <cstdlib>

class Simulator {
public:
	Simulater() {
		
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
	 * This function use Monte Carlo based tree search to 
	 * select one move from the MoveList.
	 * */
	 int pureMonteCaro(const mBoard &board, int turn, int move, int game_length, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE]){
	    mBoard TmpBoard = board;
	    int MoveList[HISTORYLENGTH];
	    int num_legal_moves = 0;
	    int nowTurn = turn;
	    
	    int iter = 0;
		iter = game_length * game_length;
	    while (move != 0 && iter >= 0) {
	        do_move(TmpBoard, nowTurn, move);
	        record(TmpBoard, GameRecord, game_length+1);
	        game_length++;
	        nowTurn = 3 - nowTurn;
	        num_legal_moves = TmpBoard.gen_legal_move(nowTurn, game_length, GameRecord, MoveList);
	        if (num_legal_moves == 0)
	        	break;
	        move = rand_pick_move(num_legal_moves, MoveList);
	        iter--;
	    }
	    
	    int score = TmpBoard.score();
	    return (turn == BLACK) ? score:-score;
	}
	int mcts_pick_move(mBoard &board, int turn, int time_limit,
			int game_length, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE]) {
		int MoveList[HISTORYLENGTH];
		int num_legal_moves = board.gen_legal_move(turn, game_length, GameRecord, MoveList);
	
		if (num_legal_moves == 0)
			return 0;
			
	    clock_t start_t, end_t, now_t;
	    // record start time
	    start_t = clock();
	    end_t = start_t + CLOCKS_PER_SEC * time_limit;
	    int best_move = MoveList[0];
	    int best_score = INT_MIN;
	    vector<int> first_step(num_legal_moves, 0);
	    for (int i = 0; i < first_step.size(); i++)
	    	first_step[i] = INT_MIN;
	    while (true) {
	    	for (int i = 0; i < num_legal_moves; i++) {
	    		int score = pureMonteCaro(board, turn, MoveList[i], game_length, GameRecord);
	    		if (score > first_step[i])
	    			first_step[i] = score;
				if (clock() > end_t)
					break;
			}
			if (clock() > end_t)
				break;
		}
		for (int i = 0; i < num_legal_moves; i++)  {
			if (first_step[i] > best_score) {
				best_move = MoveList[i];
	    		best_score = first_step[i];
			}
		}
		return best_move;
	}
	/*
	 * This function randomly generate one legal move (x, y) with return value x*10+y,
	 * if there is no legal move the function will return 0.
	 * */
	int genmove(mBoard &board, int turn, int time_limit, int game_length, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE]) {
		int return_move = mcts_pick_move(board, turn, time_limit, game_length, GameRecord);
		do_move(board, turn, return_move);
		return return_move % 100;
	}
};
