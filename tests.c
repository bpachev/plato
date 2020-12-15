#include "stdio.h"
#include "stdlib.h"
#include "board.h"
#include "string.h"
#include "search.h"

#define MAX_LINE_LEN 81

//firstLIne is a pointer to a buffer where we store the first line
int parseTest(const char* filename, char* firstLine, PlatoBoard * board) {
	FILE * fp;
	if (!(fp = fopen(filename, "r"))) {
		printf("Cannot read from file %s!\n", filename);
		return 0;
	}
	printf("Testing %s\n", filename);
	int n = MAX_LINE_LEN;
	char buf[n];
	char * p;
	init_board(board);

	//read first line, saving expected result for caller
	fgets(firstLine, n+1, fp);

	while ((p = fgets(buf, n+1, fp))) {
		int x = p[0] - '0';
		//not a digit
		if (x >= 10 || x < 0) break;

		int y = p[2] - '0';
		int stackNum = LENGTH * x + y;
		if (checkVictory(board)) {
			printf("Error - victory detected before end of game!\n");
			return 0;
		}

		const char * color = (board->whiteTurn) ? "White" : "Black";
		printf("%s move: %d %d, stacknum %d\n", color, x, y, stackNum);
		doMove(board, stackNum);
		return 1;
	}

	fclose(fp);
	return 1;
}

//Run a test game - ensure victory testing is correct
int testVictory(const char* filename) {
	PlatoBoard board;
	char firstLine[MAX_LINE_LEN];

	parseTest(filename, firstLine, &board);
	int expectedResult = firstLine[0] - '0', actualResult = 0;

	if (checkVictory(&board)) {
		actualResult = (board.whiteTurn) ? 2 : 1;
		printf("Game Over!\n");
	}

	if (actualResult != expectedResult) {
		printf("Incorrect result! Computed %d, expected %d\n", actualResult, expectedResult);
/*	if (actualResult != expectedResult) {
		printf("Incorrect result! Computed %d, expected %d\n", actualResult, expectedResult);
		for (int k =0; k < 4; k++) printf("White level %d: %x\n", k, board.whitePos[k]);
		for (int k =0; k < 4; k++) printf("Black level %d: %x\n", k, board.blackPos[k]);
		return false;
	}*/
		return 0;
	}

	return 1;
}

int testOpportunityCount(const char * filename)
{
	PlatoBoard board;
	char firstLine[MAX_LINE_LEN];
	if (!parseTest(filename, firstLine, &board)) return 0;

	int expectedOpportunities = firstLine[0]-'0';
	int expectedForcing = firstLine[2]-'0';
	int actualOpportunities = 0, actualForcing = 0;

	if (board.whiteTurn) {
		actualOpportunities = countOpportunities(board.blackPos, board.whitePos, &actualForcing);
	} else {
		actualOpportunities = countOpportunities(board.whitePos, board.blackPos, &actualForcing);
	}

	if ((actualOpportunities != expectedOpportunities) || (actualForcing != expectedForcing)) {
		printf("Computed opportunities: %d, expected %d. Computed num forcing %d, expected %d.\n",
			actualOpportunities, expectedOpportunities, actualForcing, expectedForcing);
		return 0;
	}

	return 1;
}

int testMovePick(const char* filename)
{
	PlatoBoard board;
	char firstLine[MAX_LINE_LEN];
	if (!parseTest(filename, firstLine, &board)) return 0;
	int depth = firstLine[0]-'0';
	pickMove(&board, depth);
}

int playTestGame()
{
	char* line = NULL;
	PlatoBoard board;
	PlatoBoard* boardp = &board;
	size_t len;
	init_board(boardp);

	while (1) {
		printf("White's move: ");
		getline(&line, &len, stdin);
		int stack = 4*(line[0]-'0') + (line[2]-'0');
		
		if (!VALID_MOVE(boardp, stack)) {
			printf("Invalid move!\n");
			continue;
		}

		doMove(boardp, stack);
		if (checkVictory(boardp)) {
			printf("You win!\n");
			break;
		}
		//TODO -- allow user to provide search depth
		int move = pickMove(boardp, 4);
		doMove(boardp, move);
		printf("Black's Move: %d %d\n", move/4, move % 4);
		if (checkVictory(boardp)) {
			printf("AI wins!\n");
			break;
		}
	}

	return 0;
}

int main(int argc, const char** argv) {
//	printf("AI for Pillars of Plato (3-D Connect4).\n");
	printf("Test script for Pillars of Plato AI.\n");

	if (argc < 2) {
		printf("Usage [mode] testfile1 [testfile 2]\n");
		return 0;
	}

	int (*testMethod)(const char *) = &testVictory;
	int start = 1;

	if (strlen(argv[1]) == 1) {
		if (argv[1][0] == 'g') {
			return playTestGame();
		}

		if (argc < 3) {
			printf("Usage [mode] testfile1 [testfile 2]\nWhen mode is provided, you must provide at least one testfile.\n");
			return 0;
		}

		switch (argv[1][0]) {
			//Simulate victory checking
			case 'v':
				testMethod = &testVictory;
				break;
			case 'o':
				testMethod = &testOpportunityCount;
				break;
			case 'm':
				testMethod = &testMovePick;
				break;
			default:
				printf("Unrecognized test mode %s.\n", argv[1]);
				return 0;
		}
		start = 2;
	}

	for (int i = start; i < argc; i++) {
		if (!(*testMethod)(argv[i])) {
			printf("Test case %s failed!\n", argv[i]);
			return 1;
		}
	}

	return 0;
}
