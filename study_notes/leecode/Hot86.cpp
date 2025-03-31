// 给你一个字符串 s 和一个字符串列表 wordDict 作为字典。如果可以利用字典中出现的一个或多个单词拼接出 s 则返回 true。

// 注意：不要求字典中出现的单词全部都使用，并且字典中的单词可以重复使用。

// 示例 1：

// 输入: s = "leetcode", wordDict = ["leet", "code"]
// 输出: true
// 解释: 返回 true 因为 "leetcode" 可以由 "leet" 和 "code" 拼接成。

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Solution
{
public:
    // 回溯法
    bool wordBreak(string s, vector<string> &wordDict)
    {
        return backtrack(s, wordDict, 0);
    }
    bool backtrack(string s, vector<string> &wordDict, int start)
    {
        if (start == s.size())
        {
            return true;
        }
        for (int end = start + 1; end <= s.size(); end++)
        {
            if (find(wordDict.begin(), wordDict.end(), s.substr(start, end - start)) != wordDict.end() && backtrack(s, wordDict, end))
            {
                return true;
            }
        }
        return false;
    }

};

// 动态规划
    bool wordBreak(string s, vector<string> &wordDict)
    {
        vector<bool> dp(s.size() + 1, false);
        dp[0] = true;
        for (int i = 1; i <= s.size(); i++)
        {
            for (int j = 0; j < i; j++)
            {
                if (dp[j] && find(wordDict.begin(), wordDict.end(), s.substr(j, i - j)) != wordDict.end())
                {
                    dp[i] = true;
                    break;
                }
            }
        }
        return dp[s.size()];
    }

// 记忆化搜索
    bool wordBreak(string s, vector<string> &wordDict)
    {
        vector<bool> memo(s.size(), -1);
        return backtrack(s, wordDict, 0, memo);
    }
    bool backtrack(string s, vector<string> &wordDict, int start, vector<bool> &memo)
    {
        if (start == s.size())
        {
            return true;
        }
        if (memo[start] != -1)
        {
            return memo[start];
        }
        for (int end = start + 1; end <= s.size(); end++)
        {
            if (find(wordDict.begin(), wordDict.end(), s.substr(start, end - start)) != wordDict.end() && backtrack(s, wordDict, end, memo))
            {
                memo[start] = true;
                return true;
            }
        }
        memo[start] = false;
        return false;
    }