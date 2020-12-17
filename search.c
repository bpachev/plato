#include "stdio.h"
#include "board.h"
#include "search.h"

//Wrapper function to get the proposed move
int pickMove(PlatoBoard* board, int searchDepth) {
	int move = 0;
	int score = alphaBetaSearch(board, searchDepth, -MAX_SCORE, -MAX_SCORE, &move);
    if(board->heights[move] >= HEIGHT){
        for(int i = 0; i <= NSTACKS; i++){
            if(board->heights[i] < HEIGHT){
                move = i;
                break;
            }
        }
    }
	const char * color = (board->whiteTurn) ? "White" : "Black";
    //printf("Picked move %d for %s, score %d\n", move, color, score);
	return move;
}

//Compute the leaf node score
//The computation is performed for the player whose move it is currently
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
	int myForcing=0, otherForcing=0;
	int myOpportunities = countOpportunities(pos, otherPos, &myForcing);
	int otherOpportunities = countOpportunities(otherPos, pos, &otherForcing);
	//If this move doesn't win, and leaves our opponent with a way to win, we are toast
	if (otherForcing) return -MAX_SCORE+1; //game over in 1 move
	if (myForcing > 1) return MAX_SCORE-2; //we have two threats, and he has none, so we win in two moves
	return FORCING_WEIGHT * myForcing + OPPORTUNITY_WEIGHT * myOpportunities - OPPONENT_OPPORTUNITY_WEIGHT * otherOpportunities;
}

//Perform alpha beta search - returning the best outcome
int alphaBetaSearch(PlatoBoard* board, int depth, int my_best, int other_best, int* bestMove)
{
	int scores[NSTACKS];
	int i, temp_score;
	register int best = 0;
	//Don't explore past a terminal node
	if (checkVictory(board)) return -MAX_SCORE;

	if (depth <= 1) {
		for (i = 0; i < NSTACKS; i++) {
			if (!VALID_MOVE(board, i)) continue;
			doMove(board, i);
			scores[i] = temp_score = score(board);
			revertMove(board, i);
			if (-temp_score < other_best) return temp_score;
			if (temp_score > my_best) {
				my_best = temp_score;
				best = i;
			}
		}

		*bestMove = best;
		return my_best;
	}

	int otherBestMove = 0;
	//TODO:: add move ordering
	for (i = 0; i < NSTACKS; i++) {
		if (!VALID_MOVE(board, i)) continue;
		doMove(board, i);
		scores[i] = temp_score = -alphaBetaSearch(board, depth-1, other_best, my_best, &otherBestMove);
		revertMove(board, i);
		//prune this node
		if (-temp_score < other_best) return temp_score;
		if (temp_score > my_best) {my_best = temp_score; best = i;}
	}

	*bestMove = best;
	return my_best;
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
		opportunities[i] = 0;
		if (!pos[i]) continue;
		for (j = 0; j < NUM_LEVEL_FILTERS; j++) {
			//See https://stackoverflow.com/questions/108318/whats-the-simplest-way-to-test-whether-a-number-is-a-power-of-2-in-c
			unsigned short maybe_pow2 = FILTER_COMPLEMENT(pos[i], filters[j]);
			//printf("Filter %x, maybepow2 %x, %x\n", filters[j], maybe_pow2, maybe_pow2&(maybe_pow2-1));
			if (IS_POW2(maybe_pow2)) {
				opportunities[i] |= maybe_pow2;
			}
		}
	}

	//Check verticals - note because of how we stack things, the only possibilities to win are on the top two levels
	opportunities[3] |= (pos[0]&pos[1]&pos[2]);
	opportunities[4] |= (pos[1]&pos[2]&pos[3]);

	//for(i=0; i<HEIGHT; i++) printf("opp level %d %x\n", i, opportunities[i]);


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

		unsigned short diagMask = (base[0] & 0x9) + (base[1] & 0x60) + (base[2] & 0x600) + (base[3] & 0x9000);
		unsigned short maybe_pow2 = FILTER_COMPLEMENT(diagMask, 0x8421);
		if (IS_POW2(maybe_pow2)) opportunities[offset + pow2_row(maybe_pow2)] |= maybe_pow2;
		maybe_pow2 = FILTER_COMPLEMENT(diagMask, 0x1248);
		if (IS_POW2(maybe_pow2)) opportunities[offset + pow2_row(maybe_pow2)] |= maybe_pow2;

		unsigned short offDiagMask = (base[3] & 0x9) + (base[2] & 0x60) + (base[1] & 0x600) + (base[0] & 0x9000);
		maybe_pow2 = FILTER_COMPLEMENT(diagMask, 0x8421);
		if (IS_POW2(maybe_pow2)) opportunities[offset + 3 - pow2_row(maybe_pow2)] |= maybe_pow2;
		maybe_pow2 = FILTER_COMPLEMENT(diagMask, 0x1248);
		if (IS_POW2(maybe_pow2)) opportunities[offset + 3 - pow2_row(maybe_pow2)] |= maybe_pow2;
	}


	int numOps = 0;
	int numForce = 0;
	for (i = 0; i < HEIGHT; i++) {
		//printf("pos level %d %x\n", i, pos[i]);
		//We cannot play in a square if there is a piece of the opposite color
		//So three-in- a rows don't matter if there is not something already there
		opportunities[i] &= ~(otherPos[i]);
		//printf("opp level %d %x\n", i, opportunities[i]);
		numOps += __builtin_popcount(opportunities[i]);
		//If we're on the first level, any three in a row is a threat
		//Otherwise, we need to check the leve below to see if there is 'support' for the peice
		numForce +=  (i) ? __builtin_popcount(opportunities[i] & (pos[i-1] | otherPos[i-1])) : __builtin_popcount(opportunities[i]);
	}

	*numForcing = numForce;

	return numOps;
}

//Figure out what 'row' a power of two lives on
//Mathematically, this is floor(log(pow2)/log(4))
//But I want to avoid FLOPS
int pow2_row(unsigned short pow2) {
	int row = 0;
	if (pow2 >= 0x100) {row += 2; pow2 >> 8;}
	return row + (pow2 >= 16);
}

