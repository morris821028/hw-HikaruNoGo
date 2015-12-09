## Windows Output Executable file
WHITEPLAYER = randomgoTA.exe
BLACKPLAYER = Stephanie.exe
## Linux
#WHITEPLAYER = randomgoTA
#BLACKPLAYER = Stephanie

GAME_NUM = 2
SGFFILE_DIR = test
SGFFILE = result
SGPREFIX = test

TWOGTP_FLAGS = -alternate -size 9 -komi 7 -verbose -auto
TWOGTP_FLAGS += -games $(GAME_NUM) -sgffile ./$(SGFFILE_DIR)/$(SGFFILE)/$(SGPREFIX)

CURDIR = ~/Desktop/github/hw-HikaruNoGo/source/MCTS
GOGUI_FLAGS = -size 9 -computer-both -auto

CXX = g++
CXX_FLAGS = -Wall -lm -std=c++11
DEBUG_FLAGS = -g -pg
OPT_FLAGS = -O2 -march=native
TEST_FLAGS = -DUCT -DTEST

all:
	$(CXX) ./test/randomgo.cpp -o $(WHITEPLAYER)
	$(CXX) simulator.cpp -c $(OPT_FLAGS) $(CXX_FLAGS) $(TEST_FLAGS) $(DEBUG_FLAGS)
	$(CXX) board.cpp -c $(OPT_FLAGS) $(CXX_FLAGS) $(TEST_FLAGS) $(DEBUG_FLAGS)
	$(CXX) MCTS.cpp -c $(OPT_FLAGS) $(CXX_FLAGS) $(TEST_FLAGS) $(DEBUG_FLAGS)
	$(CXX) Stephanie.cpp -c $(OPT_FLAGS) $(CXX_FLAGS) $(TEST_FLAGS) $(DEBUG_FLAGS)
	$(CXX) Stephanie.o MCTS.o board.o simulator.o -o $(BLACKPLAYER) $(OPT_FLAGS) $(CXX_FLAGS) $(TEST_FLAGS) $(DEBUG_FLAGS)
play:
	rm -rf ./$(SGFFILE_DIR)/$(SGFFILE)
	mkdir ./$(SGFFILE_DIR)/$(SGFFILE)
	gogui-twogtp $(TWOGTP_FLAGS) -white $(WHITEPLAYER) -black $(BLACKPLAYER)
guiplay:
	gogui -program "gogui-twogtp -white $(WHITEPLAYER) -black $(BLACKPLAYER) -size 9" -computer-both
prof:
	gprof $(BLACKPLAYER) gmon.out >analysis.txt
clean:
	rm -rf ./$(SGFFILE_DIR)/$(SGFFILE) $(WHITEPLAYER) $(BLACKPLAYER)
	rm -f Stephanie.o board.o MCTS.o simulator.o
