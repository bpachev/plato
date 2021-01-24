#include "stdio.h"
#include "board.h"

void init_board(PlatoBoard* board) {
	int i;
	for (i = 0; i < HEIGHT; i++) board->whitePos[i] = board->blackPos[i] = 0;
	for (i = 0; i < NSTACKS; i++) board->heights[i] = 0;
	board->whiteTurn = 1;
}

int doMove(PlatoBoard* board, int stackNum) {
	int level = board->heights[stackNum]++;
	unsigned short mask = 1 << stackNum;
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
	unsigned short mask = 1 << stackNum;
	//flip turn first, so we know whose turn it was when the last move was made
	board->whiteTurn ^= 1;

	//xor to make a 1 bit be a zero
	if (board->whiteTurn) {
		board->whitePos[level] ^= mask;
	} else {
		board->blackPos[level] ^= mask;
	}
}

//Check the first four levels for a vertical diagonal victory
int checkDiagVictory(unsigned short* base) {
	int i;
	//Select first and last rows
	unsigned short rowMask = base[0] & 0xf00f;
	//For each 4-bit 'row', get the first and last bit
	unsigned short colMask = base[0] & 0x9999;

	static const unsigned short colMasks[] = {0x1111, 0x2222, 0x4444, 0x8888};

	for (i=1; i < 4; i++) {
		rowMask &= ((base[i] >> 4*i) & 0xf) + ((base[i] << 4*i) & 0xf000);
		colMask &= ((base[i]&colMasks[i]) >> i) + ((base[i]&colMasks[3-i]) << i);
	}

	if (rowMask|colMask) return 1;

	unsigned short diagMask = (base[0] & 0x1001) + (base[1] & 0x220) + (base[2] & 0x440) + (base[3] & 0x8008);
	unsigned short offDiagMask = (base[3] & 0x1001) + (base[2] & 0x220) + (base[1] & 0x440) + (base[0] & 0x8008);

	return DIAG_MATCH(diagMask) || DIAG_MATCH(offDiagMask);
}

//Check one horizontal level of the board for a win
int checkLevelVictory(unsigned short mask) {
	if (!mask) return 0;

	//check rows and columns
	unsigned short rowFilter = 0xf;
	unsigned short colFilter = 0x1111;
	for (int i = 0; i < 4; i++) {
		if (MATCH(mask, rowFilter) || MATCH(mask, colFilter)) return 1;

		rowFilter = rowFilter << 4;
		colFilter = colFilter << 1;
	}

	return DIAG_MATCH(mask);
}

int checkPosVictory(unsigned short* pos) {
	//check if a given position is a win
	//Note - we only need to know the positions of one player's tokens
	int i;

	for (i = 0; i < HEIGHT; i++) {
		if (checkLevelVictory(pos[i])) {
			//printf("Victory on level %d\n", i);
			return 1;
		}
	}

	//If any of the middle levels have none of our pieces, no vertical victories are possible
	if (!pos[3] || !pos[2] || !pos[1]) return 0;
	//Check for a vertical win
	unsigned short middle = pos[1] & pos[2] & pos[3];
	if (middle&pos[0]) return 1;
	else if (HEIGHT >= 5 && (middle&pos[4])) return 1;

	//Check for diagonal victories
	for (i = 0; i < HEIGHT-3; i++) {
    	if (checkDiagVictory(pos+i)) return 1;
	}
}

int checkVictory(PlatoBoard * board) {
	//If it is white's turn to move, check if black's last move won, and vice versa
	return (board->whiteTurn) ? checkPosVictory(board->blackPos) : checkPosVictory(board->whitePos);
}
