#ifndef BOARD_H
#define BOARD_H

#define HEIGHT 5
#define NSTACKS 16
#define LENGTH 4

//Represents a 4x4x5 board
typedef struct {
	unsigned short whitePos[HEIGHT];
	unsigned short blackPos[HEIGHT];
	char heights[NSTACKS];
	char whiteTurn;
} PlatoBoard;

void init_board(PlatoBoard* board);
int doMove(PlatoBoard * board, int stackNum);
int revertMove(PlatoBoard* board, int stackNum);

int checkVictory(PlatoBoard* board);
int checkLevelVictory(unsigned short mask);
int checkPosVictory(unsigned short * pos);

#endif
