import ctypes as ct

def read_data(file):
    data = []
    with open(file) as fd:
        for line in fd:
            data.append(list(map(int, line.strip().split(' '))))
    return data

class SudokuSolver:
    def __init__(self):
        # load .so
        sudoku_so = ct.cdll.LoadLibrary
        self.sudoku_lib = sudoku_so('./libsudoku.so')
    
    def __format_transform(self, dst, src):
        for i in range(9):
            for j in range(9):
                dst[i][j] = src[i][j]
    
    def print(self, board):
        for i in range(9):
            print(' '.join(map(str, board[i])))
    
    def solve(self, board):
        # build ctypes container of sudoku
        sudoku_board = ct.c_int * 9
        sudoku_board = sudoku_board * 9
        sudoku_board = sudoku_board()
        # data transformation
        self.__format_transform(sudoku_board, board)
        # solve
        if self.sudoku_lib.solveSudoku(sudoku_board):
            self.__format_transform(board, sudoku_board)
            return True
        return False

board = read_data('input.txt')

solver = SudokuSolver()
if solver.solve(board):
    solver.print(board)
else:
    print("Sudoku unsolvable")
