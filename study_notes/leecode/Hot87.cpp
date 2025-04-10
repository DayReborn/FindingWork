// 给你一个整数数组 nums ，找到其中最长严格递增子序列的长度。
// 子序列 是由数组派生而来的序列，删除（或不删除）数组中的元素而不改变其余元素的顺序。例如，[3,6,2,7] 是数组 [0,3,1,6,2,2,7] 的子序列。
 
// 示例 1：

// 输入：nums = [10,9,2,5,3,7,101,18]
// 输出：4
// 解释：最长递增子序列是 [2,3,7,101]，因此长度为 4 。
#include <vector>
#include <algorithm>
#include <string>
#include <stack>
#include <iostream>

using namespace std;

class Solution {
    public:
        int lengthOfLIS(vector<int>& nums) {
            int len = nums.size();
            int maxLength = 1;
            vector<int> dp(len, 1); // dp[i]表示以nums[i]结尾的最长递增子序列的长度
            for(int i = len-2; i >=0 ;--i){
                for(int j  = i+1;j<len;++j){
                    if(nums[j] > nums[i]){
                        dp[i] = max(dp[i],dp[j] + 1);
                    }
                }
                if(maxLength < dp[i]){
                    maxLength = dp[i]; // 更新最大长度
                }
            }
            return maxLength;
        }
    };


class Solution {
    public:
        int lengthOfLIS(vector<int>& nums) {
            vector<int> dp; // dp[i]表示长度为i+1的递增子序列的末尾元素
            for(int i = 0; i < nums.size(); ++i){
                auto it = lower_bound(dp.begin(),dp.end(),nums[i]); // 找到第一个大于等于nums[i]的元素
                if(it == dp.end()){
                    dp.push_back(nums[i]); // 如果没有找到，则添加到末尾
                }else{
                    *it = nums[i]; // 否则替换掉这个元素
                }
            }
            return dp.size(); // 返回长度即为最长递增子序列的长度
        }
    };
// 讲讲上面这个算法
// 1. dp[i]表示长度为i+1的递增子序列的末尾元素
// 2. lower_bound函数用于查找第一个大于等于nums[i]的元素，如果没有找到，则添加到末尾，否则替换掉这个元素
