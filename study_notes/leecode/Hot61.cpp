/*
    给你一个字符串 s，请你将 s 分割成一些子串，使每个子串都是 回文串 。返回 s 所有可能的分割方案。
    示例 1：
    输入：s = "aab"
    输出：[["a","a","b"],["aa","b"]]
*/
#include <vector>
#include <string>
using namespace std;

class Solution {
public:
    bool is_huiwen(int left, int right, string &s) {
        while (left < right) {
            if (s[left] != s[right]) return false;
            left++;
            right--;
        }
        return true;
    }

    void backtrack(int left, string &s, vector<vector<string>> &res, vector<string> &path) {
        if (left >= s.size()) {
            res.push_back(path);
            return;
        }
        for (int right = left; right < s.size(); ++right) {
            if (is_huiwen(left, right, s)) {
                string substr = s.substr(left, right - left + 1);
                path.push_back(substr);
                backtrack(right + 1, s, res, path);
                path.pop_back();
            }
        }
    }

    vector<vector<string>> partition(string s) {
        vector<vector<string>> result;
        vector<string> path;
        backtrack(0, s, result, path);
        return result;
    }
};