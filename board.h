#ifndef BOARD_H
#define BOARD_H

#define HEIGHT 5
#define NSTACKS 16
#define LENGTH 4
#define MATCH(mask, filter) ((mask&filter) == filter)
#define DIAG_MATCH(mask) (MATCH(mask, 0x1248)||MATCH(mask, 0x8421))
#define NUM_LEVEL_FILTERS 10
#define VALID_MOVE(board, i) (board->heights[i] < HEIGHT)

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
