#include "libsudoku.hpp"

bool SudokuSolver::dfs(int (*board)[9], int t) {
    if(t >= todo.size())
        return true;
    int x = todo[t].first, y = todo[t].second;
    int id, digit, mask = rows[x] & cols[y] & blks[id=getid(x,y)];
    int count;
    while(mask) {
        // digit = (int)log2f((float)lowbit(mask));
        digit = [mask]() mutable {
            int count = 0;
            while(mask && (mask & 1) == 0) {
                mask >>= 1;
                ++count;
            }
            return count;
        }();
        board[x][y] = digit;
        digit = 1 << digit;
        rows[x] ^= digit, cols[y] ^= digit, blks[id] ^= digit;
        if(dfs(board, t + 1))
            return true;
        rows[x] ^= digit, cols[y] ^= digit, blks[id] ^= digit;
        mask -= lowbit(mask);
    }
    return false;
}

void SudokuSolver::resume(int (*board)[9]) {
    int x, y;
    for(int i = 0;i < allblank.size();++i) {
        x = allblank[i].first, y = allblank[i].second;
        board[x][y] = 0;
    }
}

bool SudokuSolver::solve(int (*board)[9]) {
    int digit, id;
    bool flag = true;
    todo.clear();
    for(int i = 0;i < 9;++i)
        rows[i] = cols[i] = blks[i] = SUDOKU_MMASK;
    for(int i = 0;flag && i < 9;++i) {
        for(int j = 0;flag && j < 9;++j) {
            if(board[i][j] < 1 || board[i][j] > 9)
                allblank.push_back(std::make_pair(i,j));
            else {
                digit = 1 << board[i][j];
                if(!((rows[i] & digit) && (cols[j] & digit) && (blks[id=getid(i,j)] & digit))) {
                    flag = false;   // redundant
                    break;
                }
                rows[i] ^= digit, cols[j] ^= digit, blks[id] ^= digit;
            }
        }
    }
    for(auto it = allblank.begin();flag && it < allblank.end();++it) {
        digit = rows[it->first] & cols[it->second] & blks[id=getid(it->first, it->second)];
        if(0 != (digit = [digit, &flag]() mutable {
            int count = 0;
            while(digit && (digit & 1) == 0) {
                digit >>= 1;
                ++count;
            }
            if(digit == 0)
                flag = false;       // full
            if(digit <= 1)
                return count;
            return 0;
        }())) {                     // single
            board[it->first][it->second] = digit;
            digit = 1 << digit;
            rows[it->first] ^= digit, cols[it->second] ^= digit, blks[id] ^= digit;
        }
        else
            todo.push_back(*it);
    }
    if(!flag || !dfs(board, 0)) {   // unsolvable
        resume(board);
        return false;
    }
    return true;
}

extern "C" {
    SudokuSolver sudokuSolver;
    bool solveSudoku(int (*board)[9]) {
        return sudokuSolver.solve(board);
    }
}
