#include "stdio.h"
#include "board.h"
#include "search.h"

//Wrapper function to get the proposed move
int pickMove(PlatoBoard* board) {
	return 0;
}

//Compute the leaf node score
int score(PlatoBoard* board)
{
	unsigned short * pos, * otherPos;
	//If it is White's turn now, the last move was Black's.
	if (board->whiteTurn) {
		pos = board->blackPos;
		otherPos = board->whitePos;
	} else {
		pos = board->whitePos;
		otherPos = board->blackPos;
	}

	//Check to see if this move wins
	if (checkPosVictory(pos)) return MAX_SCORE;
//	return scoreColor(pos, otherPos) - scoreColor();
}

//Count and return the total number of squares where placing a token would win
//Many of these cannot be claimed yet, but represent future opportunities

int countOpportunities(unsigned short* pos, unsigned short* otherPos, int* numForcing)
{
	unsigned short opportunities[5];
	int i,j;
	//We logically check each 4-in-a row as for the victory checking
	//However, our processing is more intense

	const unsigned short filters[] = {0xf, 0xf0, 0xf00, 0xf000, 0x1111, 0x2222, 0x4444, 0x8888, 0x1248, 0x8421};

	//First, check each level
	for (i = 0; i < HEIGHT; i++) {
		if (!pos[i]) continue;
		for (j = 0; j < NUM_LEVEL_FILTERS; j++) {
			//See https://stackoverflow.com/questions/108318/whats-the-simplest-way-to-test-whether-a-number-is-a-power-of-2-in-c
			unsigned short maybe_pow2 = (~(pos[i]&filters[j])) & filters[j];
			//printf("Filter %x, maybepow2 %x, %x\n", filters[j], maybe_pow2, maybe_pow2&(maybe_pow2-1));
			if (maybe_pow2 && !(maybe_pow2&(maybe_pow2-1))) {
				opportunities[i] |= maybe_pow2;
			}
		}
	}

	//Check verticals - note because of how we stack things, the only possibilities to win are on the top two levels
	opportunities[3] |= (pos[0]&pos[1]&pos[2]);
	opportunities[4] |= (pos[1]&pos[2]&pos[3]);

	//Check diagonals - start with the simpler ones
	unsigned short forwardMasks[4];
	unsigned short backwardMasks[4];
	unsigned short temp1, temp2;
	static const unsigned short colMasks[] = {0x1111, 0x2222, 0x4444, 0x8888};

	for (int offset = 0; offset < 2; offset++) {
		unsigned short * base = pos + offset;
		//first handle row
		//extract first and last row
		forwardMasks[0] = base[0] & 0xf00f;
		temp1 = (((base[1] >> 4) & 0xf) + ((base[1] << 4) & 0xf000));
		temp2 = (((base[2] >> 8) & 0xf) + ((base[2] << 8) & 0xf000));
		forwardMasks[1] = forwardMasks[0] & temp1;
		forwardMasks[2] = forwardMasks[1] & temp2;

		backwardMasks[0] = ((base[3] >> 12) & 0xf) + ((base[3] << 12) & 0xf000);
		backwardMasks[1] = backwardMasks[0] & temp2;
		backwardMasks[2] = backwardMasks[1] & temp1;

		opportunities[offset] |= backwardMasks[2];

		for (i =1; i < 4; i++) {
			temp1 = forwardMasks[i-1];
			if (i < 3) temp1 &= backwardMasks[2-i];
			opportunities[i+offset] |= (temp1 >> 4*i) + (temp1 << 4*i);
		}

		//Now handle columns
		forwardMasks[0] = base[0] & 0x9999;
		temp1 = (((base[1] & colMasks[1]) >> 1) + ((base[1] & colMasks[2]) >> 1));
		temp2 = (((base[1] & colMasks[2]) >> 2) + ((base[1] & colMasks[1]) >> 2));
		forwardMasks[1] = forwardMasks[0] & temp1;
		forwardMasks[2] = forwardMasks[1] & temp2;

		backwardMasks[0] = (((base[3] & colMasks[3]) >> 3) + ((base[3] & colMasks[0]) >> 3));
		backwardMasks[1] = backwardMasks[0] & temp2;
		backwardMasks[2] = backwardMasks[1] & temp1;

		opportunities[offset] |= backwardMasks[2];

		for (i =1; i < 4; i++) {
			temp1 = forwardMasks[i-1];
			if (i < 3) temp1 &= backwardMasks[2-i];
			opportunities[i+offset] |= (temp1 << i) + (temp1 >> i);
		}
	}

	//printf("pos level 0 %x\n", pos[0]);

	int numOps = 0;
	int numForce = 0;
	for (i = 0; i < HEIGHT; i++) {
		//We cannot play in a square if there is a piece of the opposite color
		//So three-in- a rows don't matter if there is not something already there
		opportunities[i] &= ~(otherPos[i]);
		numOps += __builtin_popcount(opportunities[i]);
		//If we're on the first level, any three in a row is a threat
		//Otherwise, we need to check the leve below to see if there is 'support' for the peice
		numForce +=  (i) ? __builtin_popcount(opportunities[i] & (pos[i-1] | otherPos[i-1])) : __builtin_popcount(opportunities[i]);
	}

	*numForcing = numForce;

	return numOps;
}
