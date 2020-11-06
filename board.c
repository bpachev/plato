#include "stdio.h"
#include "board.h"

#define MATCH(mask, filter) ((mask&filter) == filter)

void init_board(PlatoBoard* board) {
	int i;
	for (i = 0; i < HEIGHT; i++) board->whitePos[i] = board->blackPos[i] = 0;
	for (i = 0; i < NSTACKS; i++) board->heights[i] = 0;
	board->whiteTurn = 1;
}

int doMove(PlatoBoard* board, int stackNum) {
	int level = board->heights[stackNum]++;
	short mask = 1 << stackNum;
	if (board->whiteTurn) {
		board->whitePos[level] |= mask;
	} else {
		board->blackPos[level] |= mask;
	}
	//flip turn
	board->whiteTurn ^= 1;
}

int revertMove(PlatoBoard* board, int stackNum) {
	//heights points to the next insertion point, so we need to first decrement, then get the value
	//because we want the place where the last ball was inserted
	int level = --(board->heights[stackNum]);
	short mask = 1 << stackNum;
	//flip turn first, so we know whose turn it was when the last move was made
	board->whiteTurn ^= 1;

	//xor to make a 1 bit be a zero
	if (board->whiteTurn) {
		board->whitePos[level] ^= mask;
	} else {
		board->blackPos[level] ^= mask;
	}
}

//Check one horizontal level of the board for a win
int checkLevelVictory(short mask) {
	if (!mask) return 0;

	//check rows and columns
	short rowFilter = 0xf;
	short colFilter = 0x1111;
	for (int i = 0; i < 4; i++) {
		if (MATCH(mask, rowFilter) || MATCH(mask, colFilter)) return 1;
		rowFilter << 4;
		colFilter << 1;
	}

	short principalDiag = 0x1248;
	short offDiag = 0x8421;

	return (MATCH(mask, principalDiag) || MATCH(mask, offDiag));
}

int checkPosVictory(short* pos) {
	//check if a given position is a win
	//Note - we only need to know the positions of one player's tokens
	int i;
	
	for (i = 0; i < HEIGHT; i++) {
		if (checkLevelVictory(pos[i])) {
			//printf("Victory on level %d\n", i);
			return 1;
		}
	}

	return 0;
}

int checkVictory(PlatoBoard * board) {
	//If it is white's turn to move, check if black's last move won, and vice versa
	return (board->whiteTurn) ? checkPosVictory(board->blackPos) : checkPosVictory(board->whitePos);
}
