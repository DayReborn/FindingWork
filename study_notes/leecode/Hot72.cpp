/**
 * 给定一个整数数组 temperatures ，表示每天的温度，返回一个数组 answer ，其中 answer[i] 是指对于第 i 天，下一个更高温度出现在几天后。如果气温在这之后都不会升高，请在该位置用 0 来代替。
 *
 * 示例 1:
 *
 * 输入: temperatures = [73,74,75,71,69,72,76,73]
 * 输出: [1,1,4,2,1,1,0,0]
 */

#include <vector>
#include <algorithm>
#include <string>
#include <stack>

using namespace std;

// 栈方法
class Solution
{
public:
    vector<int> dailyTemperatures(vector<int> &temperatures)
    {
        stack<int> t_stack;
        int len = temperatures.size();
        for (int i = 0; i < len; i++)
        {
            while (!t_stack.empty() && temperatures[i] > temperatures[t_stack.top()])
            {
                temperatures[t_stack.top()] = i - t_stack.top();
                t_stack.pop();
            }
            t_stack.push(i);
        }
        while (!t_stack.empty())
        {
            temperatures[t_stack.top()] = 0;
            t_stack.pop();
        }
        return temperatures;
    }
};

// 动态规划方法
class Solution
{
public:
    vector<int> dailyTemperatures(vector<int> &temperatures)
    {
        int n = temperatures.size();
        vector<int> ans(n, 0);
        for (int i = n - 2; i >= 0; --i)
        {
            int j = i + 1;
            // 检查相邻的下一天是否温度更高
            if (temperatures[j] > temperatures[i])
            {
                ans[i] = 1;
            }
            else
            {
                // 跳跃式查找直到找到更高的温度或确定无更高温度
                while (j < n && temperatures[j] <= temperatures[i] && ans[j] != 0)
                {
                    j += ans[j];
                }
                if (j < n && temperatures[j] > temperatures[i])
                {
                    ans[i] = j - i;
                }
                else
                {
                    ans[i] = 0;
                }
            }
        }
        return ans;
    }
};