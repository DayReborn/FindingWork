// 给定一个非负整数 numRows，生成「杨辉三角」的前 numRows 行。

// 在「杨辉三角」中，每个数是它左上方和右上方的数的和。

// 示例 1:

// 输入: numRows = 5
// 输出: [[1],[1,1],[1,2,1],[1,3,3,1],[1,4,6,4,1]]
#include <iostream>
#include <vector>

using namespace std;

class Solution
{
public:
    vector<vector<int>> generate(int numRows)
    {
        vector<vector<int>> res;
        for(int i= 0; i<numRows; i++)
        {
            vector<int> row(i+1, 1);
            for(int j = 1; j<i; j++)
            {
                row[j] = res[i-1][j-1] + res[i-1][j];
            }
            res.push_back(row);
        }
        return res;
    }
};