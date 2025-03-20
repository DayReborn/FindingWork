// 给你一个字符串 s 。我们要把这个字符串划分为尽可能多的片段，同一字母最多出现在一个片段中。
// 如，字符串 "ababcc" 能够被分为 ["abab", "cc"]，但类似 ["aba", "bcc"] 或 ["ab", "ab", "cc"] 的划分是非法的。

// 注意，划分结果需要满足：将所有划分结果按顺序连接，得到的字符串仍然是 s 。

// 返回一个表示每个字符串片段的长度的列表。

// 示例 1：
// 输入：s = "ababcbacadefegdehijhklij"
// 输出：[9,7,8]
// 解释：
// 划分结果为 "ababcbaca"、"defegde"、"hijhklij" 。
// 每个字母最多出现在一个片段中。
// 像 "ababcbacadefegde", "hijhklij" 这样的划分是错误的，因为划分的片段数较少。

#include <iostream>
#include <vector>

using namespace std;
/**
 * @brief 贪心算法
 * @param s 字符串
 * @return 返回划分结果
 */
class Solution
{
public:
    vector<int> partitionLabels(string s)
    {
        int len = s.size();
        vector<int> begin(26, -1);
        vector<int> last(26, -1);
        for (int i = 0; i < len; i++)
        {
            last[s[i] - 'a'] = i;
        }
        vector<int> res;
        int start = 0;
        int end = 0;
        // 遍历字符串
        // 其实这道题感觉跟hot79很像，都是贪心算法
        // 因为我们的每个子串要包含一个字母的所有集合
        // 所以我们从第一个字母开始，到最后一个这个字母的位置结束
        // 中间如果包含了其他字母，我们就要更新end的位置
        // 当i == end时，说明我们找到了一个子串
        for (int i = 0; i < len; i++)
        {
            end = max(end, last[s[i] - 'a']);
            if (i == end)
            {
                res.push_back(end - start + 1);
                start = end + 1;
            }
        }
        return res;
    }
};