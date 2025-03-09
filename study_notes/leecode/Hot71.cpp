#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include <stack>

using namespace std;

class Solution
{
public:
    string backtrack(string s, int left, int right)
    {
        int new_left = left;
        int new_right = right;
        int times = 0;
        string left_s = "";
        string mid_s = "";
        string right_s = "";
        while (s[new_left] >= 'a' && s[new_left] <= 'z')
        {
            left_s.push_back(s[new_left]);
            if (new_left == new_right)
                return left_s;
            new_left++;
        }
        while (s[new_right] >= 'a' && s[new_right] <= 'z')
        {
            new_right--;
        }
        for (int i = new_right + 1; i <= right; ++i)
        {
            right_s.push_back(s[i]);
        }
        while (s[new_left] >= '1' && s[new_left] <= '9')
        {
            times = 10 * times + int(s[new_left]);
            new_left++;
        }
        string repeat_s = backtrack(s, new_left + 1, new_right + 1);
        for (int i = 0; i < times; ++i)
        {
            mid_s += repeat_s;
        }
        return left_s + mid_s + right_s;
    }
    string decodeString(string s)
    {
        int left = 0;
        int right = s.size() - 1;
        return backtrack(s, left, right);
    }
};

class Solution
{
public:
    string decodeString(string s)
    {
        return backtrack(s, 0, s.size() - 1);
    }

private:
    string backtrack(string &s, int left, int right)
    {
        if (left > right)
            return "";

        string left_part;
        int i = left;
        // 收集左侧的连续字母
        while (i <= right && isalpha(s[i]))
        {
            left_part += s[i];
            i++;
        }
        if (i > right)
            return left_part;

        // 解析数字
        int num = 0;
        while (i <= right && isdigit(s[i]))
        {
            num = num * 10 + (s[i] - '0');
            i++;
        }

        // 找到匹配的括号
        int bracketStart = i;
        int bracketEnd = findMatchingBracket(s, bracketStart, right);

        // 递归处理括号内的内容
        string mid_part = backtrack(s, bracketStart + 1, bracketEnd - 1);
        string repeated;
        for (int j = 0; j < num; ++j)
        {
            repeated += mid_part;
        }

        // 递归处理剩余部分
        string right_part = backtrack(s, bracketEnd + 1, right);

        return left_part + repeated + right_part;
    }

    int findMatchingBracket(string &s, int start, int end)
    {
        stack<int> stk;
        for (int i = start; i <= end; ++i)
        {
            if (s[i] == '[')
            {
                stk.push(i);
            }
            else if (s[i] == ']')
            {
                stk.pop();
                if (stk.empty())
                {
                    return i;
                }
            }
        }
        return -1; // 输入保证有效，不会执行到
    }
};