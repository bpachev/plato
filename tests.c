#include "stdio.h"
#include "stdlib.h"
#include "board.h"
#include "stdbool.h"

#define MAX_LINE_LEN 80

//Run a test game - ensure victory testing is correct
int testGame(const char* filename) {
	FILE * fp;
	if (!(fp = fopen(filename, "r"))) {
		printf("Cannot read from file %s!\n", filename);
		return 0;
	}
	printf("Testing %s\n", filename);
	int n = MAX_LINE_LEN + 1;
	char buf[n];
	PlatoBoard board;
	init_board(&board);

	//read first line, get expected result
	fgets(buf, n+1, fp);

	char * p = buf;
	int expectedResult = *p - '0';
	int actualResult = 0;
	printf("Expected result %d\n", expectedResult);

	while ((p = fgets(buf, n+1, fp))) {
		int x = p[0] - '0';
		//not a digit
		if (x >= 10 || x < 0) break;

		int y = p[2] - '0';
		int stackNum = LENGTH * x + y;
		const char * color = (board.whiteTurn) ? "White" : "Black";
		printf("%s move: %d %d, stacknum %d\n", color, x, y, stackNum);
		doMove(&board, stackNum);
		if (actualResult) {
			printf("Error - false early positive for victory!\n");
			return false;
		}

		if (checkVictory(&board)) {
			actualResult = (board.whiteTurn) ? 2 : 1;
			if (actualResult != expectedResult) {
				printf("Incorrect result! Computed %d, expected %d\n", actualResult, expectedResult);
				return false;
			}
			printf("Game Over!\n");
		}
	}

	if (actualResult != expectedResult) {
		printf("Incorrect result! Computed %d, expected %d\n", actualResult, expectedResult);
		for (int k =0; k < 4; k++) printf("White level %d: %x\n", k, board.whitePos[k]);
		for (int k =0; k < 4; k++) printf("Black level %d: %x\n", k, board.blackPos[k]);
		return false;
	}

	fclose(fp);
	return true;
}

int main(int argc, const char** argv) {
//	printf("AI for Pillars of Plato (3-D Connect4).\n");
	printf("Test script for Pillars of Plato AI.\n");

	if (argc < 2) {
		printf("Usage testfile1 [testfile 2]\n");
		return 0;
	}

	for (int i = 1; i < argc; i++) {
		if (!testGame(argv[i])) {
			printf("Test case %s failed!\n", argv[i]);
			return 1;
		}
	}

	return 0;
}
