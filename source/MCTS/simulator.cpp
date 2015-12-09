#include "simulator.h"
#include "board.h"

void Simulator::record(mBoard board, mBoard GameRecord[MAXGAMELENGTH], int game_length) {
	memcpy(&GameRecord[game_length], &board, sizeof(mBoard));
}
int Simulator::genmove(mBoard &board, int turn, int time_limit, int game_length, mBoard GameRecord[MAXGAMELENGTH]) {
	mcts.init(board, game_length, turn, GameRecord);
	int move = mcts.run(time_limit);
	mBoard tmp = board;
	mBoard::makeMove(tmp, turn, move);
	board = tmp;
	return move % 100;
}
