// 279. 完全平方数

// 给你一个整数 n ，返回 和为 n 的完全平方数的最少数量 。

// 完全平方数 是一个整数，其值等于另一个整数的平方；换句话说，其值等于一个整数自乘的积。例如，1、4、9 和 16 都是完全平方数，而 3 和 11 不是。

// 示例 1：

// 输入：n = 12
// 输出：3 
// 解释：12 = 4 + 4 + 4

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
    public:
        int numSquares(int n) {
            
            vector<int> dp(n+1, 0);


            int label = 1;
            for(int i = 1; i <= n; i++) {
                if(i == label * label) {
                    dp[i] = 1;
                    label++;
                } else {
                    int minn = n;
                    // 从1开始遍历，找到最小的平方数个数
                    for(int j = 1; j * j < i; j++) {
                        minn = min(minn, dp[i - j * j] + 1);
                    }
                    dp[i] = minn;
                }
            }
            return dp[n];
        }
    };
