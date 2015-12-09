#include "board.h"
#include "config.h"
#include "simulator.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <limits.h>
#include <ctype.h>

using namespace std;

static int _board_size = BOARDSIZE;
static double _komi =  DEFAULTKOMI;
static const char LabelX[]="0ABCDEFGHJ";
FILE* fdebug;
Simulator simulator;

/*
 * Following are commands for Go Text Protocol (GTP)
 **/
const char *KnownCommands[]={
    "protocol_version",
    "name",
    "version",
    "known_command",
    "list_commands",
    "quit",
    "boardsize",
    "clear_board",
    "komi",
    "play",
    "genmove",
    "undo",
    "quit",
    "showboard",
    "final_score"
};

void gtp_final_score(mBoard &board) {
    double result;
    result = board.score();
    result -= _komi;
    cout << "= ";
    if (result > 0.0) // Black win
		cout << "B+" << result << endl << endl<< endl;
    else if (result < 0.0) // White win
		cout << "W+" << -result << endl << endl<< endl;
    else // draw
		cout << "0" << endl << endl<< endl;
}
void gtp_undo(mBoard &board, int game_length, mBoard GameRecord[MAXGAMELENGTH]) {
    if (game_length != 0)
    	memcpy(&board, &GameRecord[game_length], sizeof(mBoard));
    cout << "= " << endl << endl;
}
void gtp_showboard(mBoard &board) {
    for (int i = 1; i <= BOARDSIZE; ++i) {
		cout << "#" << 10-i;
		for (int j = 1; j <= BOARDSIZE; ++j) {
		    switch (board.GETBOARD(i, j)) {
				case EMPTY: cout << " .";break;
				case BLACK: cout << " X";break;
				case WHITE: cout << " O";break;
	    	}
		}
		cout << endl;
    }
    cout << "#  ";
    for (int i = 1; i <=BOARDSIZE; ++i)
		cout << LabelX[i] <<" ";
    cout << endl << endl;
}
void gtp_protocol_version() {
    cout << "= 2" << endl<< endl;
}
void gtp_name() {
    cout << "= TCG-randomGo99" << endl << endl;
}
void gtp_version() {
    cout << "= 1.02" << endl << endl;
}
void gtp_list_commands(){
    cout << "= ";
    for (int i = 0 ; i < NUMGTPCOMMANDS; ++i)
		cout << KnownCommands[i] << endl;
    cout << endl;
}
void gtp_known_command(const char Input[]) {
    for (int i = 0 ; i < NUMGTPCOMMANDS; ++i) {
		if (strcmp(Input, KnownCommands[i]) == 0) {
	    	cout << "= true" << endl << endl;
	    	return;
		}
    }
    cout << "= false" << endl << endl;
}
void gtp_boardsize(int size) {
    if (size != 9)
		cout << "? unacceptable size" << endl << endl;
    else {
		_board_size = size;
		cout << "= " << endl << endl;
    }
}
void gtp_clear_board(mBoard &board, int NumCapture[]) {
	board.reset();
    NumCapture[BLACK] = NumCapture[WHITE] = 0;
    cout << "= "<<endl<<endl;
}
void gtp_komi(double komi) {
    _komi = komi;
    cout << "= " << endl << endl;
}
void gtp_play(char Color[], char Move[], mBoard &board, int game_length, mBoard GameRecord[MAXGAMELENGTH]) {
    int turn, move_i, move_j;
    if (tolower(Color[0]) == 'b')
		turn = BLACK;
    else
		turn = WHITE;
    if (strcmp(Move, "PASS") == 0 || strcmp(Move, "pass") == 0)
		simulator.record(board, GameRecord, game_length+1);
    else {
		// [ABCDEFGHJ][1-9], there is no I in the index.
		Move[0] = toupper(Move[0]);
		move_j = Move[0]-'A'+1;
		if (move_j == 10) 
			move_j = 9;
		move_i = 10-(Move[1]-'0');
		board.transferBoard(move_i, move_j, turn);
		simulator.record(board, GameRecord, game_length+1);
    }
    cout << "= " << endl << endl;
}
void gtp_genmove(mBoard &board, char Color[], int time_limit, int game_length, mBoard GameRecord[MAXGAMELENGTH]){
    int turn = (Color[0]=='b'||Color[0]=='B')?BLACK:WHITE;
    int move = simulator.genmove(board, turn, time_limit, game_length, GameRecord);
    int move_i, move_j;
    simulator.record(board, GameRecord, game_length+1);
    if (move==0) {
		cout << "= PASS" << endl<< endl<< endl;
    } else {
		move_i = (move%100)/10;
		move_j = (move%10);
//	cerr << "#turn("<<game_length<<"): (move, move_i,move_j)" << turn << ": " << move<< " " << move_i << " " << move_j << endl;
		cout << "= " << LabelX[move_j] << 10-move_i << endl << endl;
    }
}
/*
 * This main function is used of the gtp protocol
 * */
void gtp_main(int display) {
    char Input[COMMANDLENGTH],
		Command[COMMANDLENGTH],
		Parameter[COMMANDLENGTH];
    char Move[4], Color[6];
    int ivalue;
    double dvalue;
    mBoard board, GameRecord[MAXGAMELENGTH];
    int NumCapture[3] = {}; // 1:Black, 2: White
    int time_limit = DEFAULTTIME;
    int game_length = 0;
    if (display == 1) {
		gtp_list_commands();
		gtp_showboard(board);
    }
    while (fgets(Input, COMMANDLENGTH, stdin) != 0) {
        if (sscanf(Input, "%s", Command) != 1)
        	continue;
        if (Command[0] == '#')
            continue;
        if (!strcmp(Command, "protocol_version")) {
            gtp_protocol_version();
        } else if (!strcmp(Command, "name")) {
            gtp_name();
        } else if (!strcmp(Command, "version")) {
            gtp_version();
        } else if (!strcmp(Command, "list_commands")) {
            gtp_list_commands();
        } else if (!strcmp(Command, "known_command")) {
            sscanf(Input, "known_command %s", Parameter);
            gtp_known_command(Parameter);
        } else if (!strcmp(Command, "boardsize")) {
            sscanf(Input, "boardsize %d", &ivalue);
            gtp_boardsize(ivalue);
        } else if (!strcmp(Command, "clear_board")) {
            gtp_clear_board(board, NumCapture);
            game_length = 0;
        } else if (!strcmp(Command, "komi")) {
            sscanf(Input, "komi %lf", &dvalue);
            gtp_komi(dvalue);
        } else if (!strcmp(Command, "play")) {
            sscanf(Input, "play %s %s", Color, Move);
            gtp_play(Color, Move, board, game_length, GameRecord);
            game_length++;
            if (display == 1)
            	gtp_showboard(board);
        } else if (!strcmp(Command, "genmove")) {
            sscanf(Input, "genmove %s", Color);
            gtp_genmove(board, Color, time_limit, game_length, GameRecord);
            game_length++;
            if (display == 1)
				gtp_showboard(board);
        } else if (!strcmp(Command, "quit")) {
            break;
        } else if (!strcmp(Command, "showboard")) {
            gtp_showboard(board);
        } else if (!strcmp(Command, "undo")) {
            game_length--;
            gtp_undo(board, game_length, GameRecord);
            if (display == 1)
            	gtp_showboard(board);
        } else if (!strcmp(Command, "final_score")) {
            if (display == 1)
            	gtp_showboard(board);
            gtp_final_score(board);
        }
    }
}
int main(int argc, char* argv[]) {
    fdebug = fopen("go-debug.txt", "w");
    fprintf(fdebug, "begin");
    fflush(fdebug);
    // int type = GTPVERSION;	// 1: local version, 2: gtp version
    int display = 0; 		// 1: display, 2 nodisplay
    if (argc > 1) {
        if (!strcmp(argv[1], "-display")) {
            display = 1;
        }
        if (!strcmp(argv[1], "-nodisplay")) {
            display = 0;
        }
    }
#ifdef DEBUG
	display = 1;
#endif
    gtp_main(display);
    fclose(fdebug);
    return 0;
}
