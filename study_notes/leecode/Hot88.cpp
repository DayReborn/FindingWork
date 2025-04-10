// 给你一个整数数组 nums ，请你找出数组中乘积最大的非空连续 子数组（该子数组中至少包含一个数字），并返回该子数组所对应的乘积。

// 测试用例的答案是一个 32-位 整数。

// 示例 1:

// 输入: nums = [2,3,-2,4]
// 输出: 6
// 解释: 子数组 [2,3] 有最大乘积 6。
#include <vector>

using namespace std;

class Solution {
    public:
        int maxProduct(vector<int>& nums) {
            int len = nums.size();
            vector<int> maxProduct(len, 0);
            vector<int> minProduct(len, 0);
            maxProduct[len-1] = nums[len-1];
            minProduct[len-1] = nums[len-1];
            for(int i = len-2; i >= 0; --i){
                maxProduct[i] = max(nums[i], max(nums[i] * maxProduct[i+1], nums[i] * minProduct[i+1]));
                minProduct[i] = min(nums[i], min(nums[i] * maxProduct[i+1], nums[i] * minProduct[i+1]));
            }
            int maxValue = maxProduct[0];
            for(int i = 1; i < len; ++i){
                maxValue = max(maxValue, maxProduct[i]);
            }
            return maxValue;
        }
    };