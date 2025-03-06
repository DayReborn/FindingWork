#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Solution
{
public:
    bool exist(vector<vector<char>> &board, string word)
    {
        int m = board.size();
        int n = board[0].size();
        for (char c : word)
        {
            int i = 0;
            int j = 0;
            for (i = 0; i < m; i++)
            {
                for (j = 0; j < n; j++)
                {
                    if (board[i][j] == c)
                        break;
                }
                if (j < n && board[i][j] == c)
                    break;
            }
            if (i == m && j == n)
                return false;
        }

        int len = word.size();
        bool res = false;
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (board[i][j] == word[0])
                {
                    backtrack(i, j, 0, m, n, len, res, word, board);
                    if (res)
                        return true;
                }
            }
        }
        return false;
    }
    void backtrack(int x, int y, int index, int m, int n, int len, bool &res, string word, vector<vector<char>> board)
    {
        if (index == len - 1)
        {
            res = true;
            return;
        }
        board[x][y] = ' ';
        ++index;
        if (!res && (x - 1) >= 0 && board[x - 1][y] == word[index])
            backtrack(x - 1, y, index, m, n, len, res, word, board);
        if (!res && (x + 1) < m && board[x + 1][y] == word[index])
            backtrack(x + 1, y, index, m, n, len, res, word, board);
        if (!res && (y - 1) >= 0 && board[x][y - 1] == word[index])
            backtrack(x, y - 1, index, m, n, len, res, word, board);
        if (!res && (y + 1) < n && board[x][y + 1] == word[index])
            backtrack(x, y + 1, index, m, n, len, res, word, board);
    }
};

class Solution2
{
public:
    bool exist(vector<vector<char>> &board, string word)
    {
        int m = board.size();
        if (m == 0)
            return false;
        int n = board[0].size();
        if (n == 0)
            return false;

        for (int i = 0; i < m; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (board[i][j] == word[0])
                {
                    if (backtrack(i, j, 0, word, board))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

private:
    bool backtrack(int x, int y, int index, const string &word, vector<vector<char>> &board)
    {
        if (index == word.size() - 1)
        {
            return true;
        }

        char original = board[x][y];
        board[x][y] = ' '; // 标记为已访问

        int dx[] = {-1, 1, 0, 0};
        int dy[] = {0, 0, -1, 1};
        bool found = false;

        for (int i = 0; i < 4; ++i)
        {
            int newx = x + dx[i];
            int newy = y + dy[i];
            if (newx >= 0 && newx < board.size() && newy >= 0 && newy < board[0].size() && board[newx][newy] == word[index + 1])
            {
                if (backtrack(newx, newy, index + 1, word, board))
                {
                    found = true;
                    break;
                }
            }
        }

        board[x][y] = original; // 恢复
        return found;
    }
};

void test()
{
    vector<vector<char>> board = {{'A', 'A', 'A', 'A', 'A', 'A'},
                                  {'A', 'A', 'A', 'A', 'A', 'A'},
                                  {'A', 'A', 'A', 'A', 'A', 'A'},
                                  {'A', 'A', 'A', 'A', 'A', 'A'},
                                  {'A', 'A', 'A', 'A', 'A', 'A'},
                                  {'A', 'A', 'A', 'A', 'A', 'A'}};
    string word = "AAAAAAAAAAAAAAB";
    Solution sol;
    if (sol.exist(board, word))
        cout << "yes" << endl;
    else
        cout << "no" << endl;
}
int main()
{
    test();
    return 0;
}