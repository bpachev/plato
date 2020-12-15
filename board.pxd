cdef extern from "board.h":
    ctypedef struct PlatoBoard:
        pass

    void init_board(PlatoBoard* board)
    int doMove(PlatoBoard * board, int stackNum)
    int checkVictory(PlatoBoard* board)

cdef extern from "search.h":
    int pickMove(PlatoBoard* board, int searchDepth)

