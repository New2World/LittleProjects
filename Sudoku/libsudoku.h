#pragma once
#include <vector>
#include <utility>
// #include <cmath>

class SudokuSolver {
    constexpr static int SUDOKU_MMASK = (1 << 10) - 2;
    int rows[9], cols[9], blks[9];
    std::vector<std::pair<int,int>> todo, allblank;
    #define getid(x,y) (((x)/3)*3+((y)/3))
    #define lowbit(x) (x&(-x))
    bool dfs(int(*)[9], int);
    void resume(int(*)[9]);
public:
    bool solve(int(*)[9]);
};
