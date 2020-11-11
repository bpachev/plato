#ifndef SEARCH_H
#define SEARCH_H

#define MAX_DEPTH 4
#define MAX_SCORE 1000000

int pickMove(PlatoBoard* board);
int countOpportunities(unsigned short* pos, unsigned short* otherPos, int* numForcing);

#endif
