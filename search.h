#ifndef SEARCH_H
#define SEARCH_H

#define MAX_DEPTH 4
#define MAX_SCORE 1000000
#define IS_POW2(x) (x && !(x&(x-1)))
#define FILTER_COMPLEMENT(mask, filter) ((~(mask & filter)) & filter)

//TODO:: tune these parameters - probably an assignment for Joseph
#define FORCING_WEIGHT 1000
#define OPPORTUNITY_WEIGHT 100
#define OPPONENT_OPPORTUNITY_WEIGHT 100

int pickMove(PlatoBoard* board, int searchDepth);
int countOpportunities(unsigned short* pos, unsigned short* otherPos, int* numForcing);
int pow2_row(unsigned short pow2);
int alphaBetaSearch(PlatoBoard* board, int depth, int my_best, int other_best, int* bestMove);


#endif
