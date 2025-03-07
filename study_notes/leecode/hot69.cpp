/*
    给定一个只包括 '('，')'，'{'，'}'，'['，']' 的字符串 s ，判断字符串是否有效。

    有效字符串需满足：

    左括号必须用相同类型的右括号闭合。
    左括号必须以正确的顺序闭合。
    每个右括号都有一个对应的相同类型的左括号。


    示例 1：

    输入：s = "()"

    输出：true
*/
#include <vector>
#include <string>
#include <stack>

#include <algorithm> // max函数定义在此头文件中

using namespace std;


class Solution
{
public:
    bool isValid(string s)
    {
        stack<char> my_stack;
        for (char c : s)
        {
            if (c == '(' || c == '[' || c == '{')
            {
                my_stack.push(c);
            }
            else if (my_stack.empty())
                return false;
            else if (c == ')')
            {
                if (my_stack.top() == '(')
                    my_stack.pop();
                else
                    return false;
            }
            else if (c == ']')
            {
                if (my_stack.top() == '[')
                    my_stack.pop();
                else
                    return false;
            }
            else if (c == '}')
            {
                if (my_stack.top() == '{')
                    my_stack.pop();
                else
                    return false;
            }
        }
        if (my_stack.empty())
            return true;
        else
            return false;
    }
};