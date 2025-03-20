// 假设你正在爬楼梯。需要 n 阶你才能到达楼顶。

// 每次你可以爬 1 或 2 个台阶。你有多少种不同的方法可以爬到楼顶呢？

// 示例 1：

// 输入：n = 2
// 输出：2
// 解释：有两种方法可以爬到楼顶。
// 1. 1 阶 + 1 阶
// 2. 2 阶

#include <iostream>
#include <vector>

using namespace std;

// 动态规划
// 什么是动态规划？
// 动态规划是一种分阶段求解决策问题的数学思想。动态规划的基本思想是将待求解问题分解成若干个子问题，
// 先求解并保存子问题的解，避免重复计算，使得每个子问题只求解一次，最终获得原问题的解。

class Solution
{
public:
    int climbStairs(int n)
    {
        int dp[n + 1];
        dp[0] = 1;
        dp[1] = 1;
        for (int i = 2; i <= n; i++)
        {
            dp[i] = dp[i - 1] + dp[i - 2];
        }
        return dp[n];
    }
};
