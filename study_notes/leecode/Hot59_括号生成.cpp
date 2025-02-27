#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class Solution {
    public:
        vector<string> generateParenthesis(int n) {
            vector<string> res;
            backtrack("", 0, 0, n, res);
            return res;
        }
        
        void backtrack(string current, int left, int right, int n, vector<string>& res) {
            if (current.length() == 2 * n) {
                res.push_back(current);
                return;
            }
            // 添加左括号的条件：左括号数量未达n
            if (left < n) {
                backtrack(current + "(", left + 1, right, n, res);
            }
            // 添加右括号的条件：右括号数量少于左括号
            if (right < left) {
                backtrack(current + ")", left, right + 1, n, res);
            }
        }
};

void test()
{
    Solution sol;
    for (string s : sol.generateParenthesis(3))
    {
        cout << s << endl;
    }
}

int main()
{
    test();
    return 0;
}
