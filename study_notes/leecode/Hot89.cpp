// 给你一个 只包含正整数 的 非空 数组 nums 。请你判断是否可以将这个数组分割成两个子集，使得两个子集的元素和相等。

// 示例 1：

// 输入：nums = [1,5,11,5]
// 输出：true
// 解释：数组可以分割成 [1, 5, 5] 和 [11] 。
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

class Solution {
    public:
        bool canPartition(vector<int>& nums) {
            int n = nums.size();
            if (n < 2) {
                return false;
            }
            int sum = 0, maxNum = 0;
            for (auto& num : nums) {
                sum += num;
                maxNum = max(maxNum, num);
            }
            if (sum & 1) {
                return false;
            }
            int target = sum / 2;
            if (maxNum > target) {
                return false;
            }
            vector<int> dp(target + 1, 0);
            dp[0] = true;
            for (int i = 0; i < n; i++) {
                int num = nums[i];
                for (int j = target; j >= num; --j) {
                    dp[j] |= dp[j - num];
                }
            }
            return dp[target];
        }
    };
    

int main(){
    Solution sol;
    vector<int> list = {1,5,11,5};
    if(sol.canPartition(list)) cout<<"true"<<endl;
    else cout<<"0"<<endl;
    return 0;
}