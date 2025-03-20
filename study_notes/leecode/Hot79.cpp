// 45. 跳跃游戏 II
// 给定一个长度为 n 的 0 索引整数数组 nums。初始位置为 nums[0]。

// 每个元素 nums[i] 表示从索引 i 向后跳转的最大长度。
// 换句话说，如果你在 nums[i] 处，你可以跳转到任意 nums[i + j] 处:

// 0 <= j <= nums[i]
// i + j < n
// 返回到达 nums[n - 1] 的最小跳跃次数。生成的测试用例可以到达 nums[n - 1]。

// 示例 1:

// 输入: nums = [2,3,1,1,4]
// 输出: 2
// 解释: 跳到最后一个位置的最小跳跃数是 2。
//      从下标为 0 跳到下标为 1 的位置，跳 1 步，然后跳 3 步到达数组的最后一个位置。

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

// 贪心算法
class Solution
{
public:
    int jump(vector<int> &nums)
    {
        int len = nums.size();
        int end = 0;
        int max_position = 0;
        int steps = 0;
        // 简单来说这边是在做什么呢
        // 1. 遍历数组，找到当前位置能跳到的最远位置
        // 2. 如果当前位置等于上一次跳跃的最远位置，说明需要再跳一次
        // 3. 更新最远位置
        // 4. 更新步数
        // 5. 重复1-4
        // 6. 返回步数
        // 其实我觉的就是在统计我每走一步的时候，我能走到的最远的位置
        // 然后在这个位置的时候，我再走一步，然后再统计我能走到的最远的位置
        // 如此反复，直到我走到终点
        for (int i = 0; i < len - 1; i++)
        {
            max_position = max(max_position, nums[i] + i);
            if (i == end)
            {
                end = max_position;
                steps++;
            }
        }
        return steps;
    }
};
