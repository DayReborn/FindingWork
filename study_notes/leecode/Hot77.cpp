// 121. 买卖股票的最佳时机
// 给定一个数组 prices ，它的第 i 个元素 prices[i] 表示一支给定股票第 i 天的价格。

// 你只能选择 某一天 买入这只股票，并选择在 未来的某一个不同的日子 卖出该股票。设计一个算法来计算你所能获取的最大利润。

// 返回你可以从这笔交易中获取的最大利润。如果你不能获取任何利润，返回 0 。

// 示例 1：

// 输入：[7,1,5,3,6,4]
// 输出：5
// 解释：在第 2 天（股票价格 = 1）的时候买入，在第 5 天（股票价格 = 6）的时候卖出，最大利润 = 6-1 = 5 。
//      注意利润不能是 7-1 = 6, 因为卖出价格需要大于买入价格；同时，你不能在买入前卖出股票。

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <numeric>
#include <functional>
#include <cmath>
#include <climits>
#include <cstring>
#include <list>

using namespace std;

// @brief 暴力解法
// @param prices 股票价格数组
// @return 返回最大利润
// 时间复杂度O(n^2) 空间复杂度O(1)
class Solution {
    public:
        int maxProfit(vector<int>& prices) {
            int len = prices.size();
            if (len == 0) return 0;
            int max_price = 0;
            for(int i = 0; i < len; i++) {
                for(int j = i + 1; j < len; j++) {
                    max_price = max(max_price, prices[j] - prices[i]);
                }
            }
            return max_price;
        }
    };


// @brief 一次遍历
// @param prices 股票价格数组
// @return 返回最大利润
// 时间复杂度O(n) 空间复杂度O(n)
class Solution {
    public:
        int maxProfit(vector<int>& prices) {
            int len = prices.size();
            if (len == 0) return 0;
            int max_price = 0;
            for(int i = len - 1; i >= 0; i--) {
                max_price = max(max_price, prices[i]);
                if(prices[i] < max_price) {
                    prices[i] = max_price - prices[i];
                } else {
                    prices[i] = 0;
                }
            }
            return *max_element(prices.begin(), prices.end());
        }
    };

// @brief 动态规划
// @param prices 股票价格数组
// @return 返回最大利润
// 时间复杂度O(n) 空间复杂度O(1)
class Solution {
    public:
        int maxProfit(vector<int>& prices) {
            int min_price = INT_MAX;
            int max_profit = 0;
            for (int price : prices) {
                min_price = min(min_price, price);
                max_profit = max(max_profit, price - min_price);
            }
            return max_profit;
        }
    };