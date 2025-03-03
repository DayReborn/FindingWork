/*
    按照国际象棋的规则，皇后可以攻击与之处在同一行或同一列或同一斜线上的棋子。

    n 皇后问题 研究的是如何将 n 个皇后放置在 n×n 的棋盘上，并且使皇后彼此之间不能相互攻击。

    给你一个整数 n ，返回所有不同的 n 皇后问题 的解决方案。

    每一种解法包含一个不同的 n 皇后问题 的棋子放置方案，该方案中 'Q' 和 '.' 分别代表了皇后和空位。
*/
#include <vector>
#include <string>
using namespace std;

class Solution
{
public:
    vector<string> del_grid(int n, int row, int col, vector<string> grid)
    {
        for (int j = 0; j < n; ++j) {
            if (j != col && grid[row][j] == 'A') {
                grid[row][j] = '.';
            }
        }
        // 标记同一列
        for (int i = 0; i < n; ++i) {
            if (i != row && grid[i][col] == 'A') {
                grid[i][col] = '.';
            }
        }
        // 标记主对角线（左上到右下）
        for (int i = 0; i < n; ++i) {
            int j = col + (i - row);
            if (i != row && j >= 0 && j < n && grid[i][j] == 'A') {
                grid[i][j] = '.';
            }
        }
        // 标记副对角线（右上到左下）
        for (int i = 0; i < n; ++i) {
            int j = row + col - i;
            if (i != row && j >= 0 && j < n && grid[i][j] == 'A') {
                grid[i][j] = '.';
            }
        }
        return grid;
    }
    void backtrack(int index, int n, vector<vector<string>> &res, vector<string> path)
    {
        for (int j = 0; j < n; ++j)
        {
            if (path[index][j] == 'A')
            {
                path[index][j] = 'Q';
                if (index == n - 1)
                {
                    res.push_back(path);
                    return;
                }
                backtrack(index + 1, n, res, del_grid(n, index, j, path));
                path[index][j] = 'A';
            }
        }
    }
    vector<vector<string>> solveNQueens(int n)
    {
        vector<string> path(n, string(n, 'A'));
        vector<vector<string>> res;
        backtrack(0, n, res, path);
        return res;
    }
};


// 改进版本
class Solution1 {
    public:
        vector<vector<string>> solveNQueens(int n) {
            vector<vector<string>> result;
            vector<int> queenCol(n, -1); // 每行皇后所在的列
            vector<bool> colUsed(n, false); // 列是否被占用
            vector<bool> mainDiag(2 * n - 1, false); // 主对角线：row - col + n - 1
            vector<bool> antiDiag(2 * n - 1, false); // 副对角线：row + col
            
            backtrack(0, n, queenCol, colUsed, mainDiag, antiDiag, result);
            return result;
        }
        
    private:
        void backtrack(int row, int n, vector<int>& queenCol, vector<bool>& colUsed, 
                       vector<bool>& mainDiag, vector<bool>& antiDiag, vector<vector<string>>& result) {
            if (row == n) {
                // 生成棋盘布局
                vector<string> board(n, string(n, '.'));
                for (int i = 0; i < n; ++i) {
                    board[i][queenCol[i]] = 'Q';
                }
                result.push_back(board);
                return;
            }
            
            for (int col = 0; col < n; ++col) {
                int mainIdx = row - col + n - 1;
                int antiIdx = row + col;
                if (!colUsed[col] && !mainDiag[mainIdx] && !antiDiag[antiIdx]) {
                    // 放置皇后并更新标记
                    queenCol[row] = col;
                    colUsed[col] = true;
                    mainDiag[mainIdx] = true;
                    antiDiag[antiIdx] = true;
                    
                    backtrack(row + 1, n, queenCol, colUsed, mainDiag, antiDiag, result);
                    
                    // 回溯，恢复标记
                    colUsed[col] = false;
                    mainDiag[mainIdx] = false;
                    antiDiag[antiIdx] = false;
                    queenCol[row] = -1;
                }
            }
        }
    };