// 你是一个专业的小偷，计划偷窃沿街的房屋。每间房内都藏有一定的现金，影响你偷窃的唯一制约因素就是相邻的房屋装有相互连通的防盗系统，如果两间相邻的房屋在同一晚上被小偷闯入，系统会自动报警。

// 给定一个代表每个房屋存放金额的非负整数数组，计算你 不触动警报装置的情况下 ，一夜之内能够偷窃到的最高金额。

// 示例 1：

// 输入：[1,2,3,1]
// 输出：4
// 解释：偷窃 1 号房屋 (金额 = 1) ，然后偷窃 3 号房屋 (金额 = 3)。
//      偷窃到的最高金额 = 1 + 3 = 4 。

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// 为下面方法写个批注
// 1. 本题是一个动态规划问题，我们可以用一个数组dp来存储偷到每个房子的最大金额
// 2. dp[i]表示偷到第i个房子时的最大金额
// 3. dp[i] = max(dp[i-2], dp[i-3]) + nums[i]
// 4. 最后返回dp[size-1]和dp[size-2]的最大值即可
// 5. 注意边界条件，当房子数量为0，1，2，3时的情况
// 6. 本题的时间复杂度为O(n)，空间复杂度为O(n)
class Solution
{
public:
    int rob(vector<int> &nums)
    {
        if (nums.size() == 0)
        {
            return 0;
        }
        if (nums.size() == 1)
        {
            return nums[0];
        }
        if (nums.size() == 2)
        {
            return max(nums[0], nums[1]);
        }
        if (nums.size() == 3)
        {
            return max(nums[0] + nums[2], nums[1]);
        }
        int size = nums.size();
        int dp[size + 1];
        dp[0] = nums[0];
        dp[1] = nums[1];
        dp[2] = nums[0] + nums[2];
        for (int i = 3; i < size; i++)
        {
            dp[i] = max(dp[i - 2], dp[i - 3]) + nums[i];
        }
        return max(dp[size - 1], dp[size - 2]);
    }
};