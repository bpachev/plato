
cimport board

cdef class PlatoAI:
    cdef board.PlatoBoard _board
    cdef int depth

    def __cinit__(self):
        board.init_board(&(self._board))
        self.depth = 2

    def set_depth(self, int depth):
        self.depth = depth

    def move(self, int row, int col):
        board.doMove(&(self._board), row*4+col)

    def game_over(self):
        return board.checkVictory(&(self._board))

    def pick_move(self):
        cdef int move = board.pickMove(&(self._board), self.depth)
        board.doMove(&(self._board), move)
        cdef int x = move / 4
        cdef int y = move % 4
        return x, y
