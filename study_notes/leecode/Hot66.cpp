/*
    整数数组 nums 按升序排列，数组中的值 互不相同 。

    在传递给函数之前，nums 在预先未知的某个下标 k（0 <= k < nums.length）上进行了 旋转，使数组变为 [nums[k], nums[k+1], ..., nums[n-1], nums[0], nums[1], ..., nums[k-1]]（下标 从 0 开始 计数）。例如， [0,1,2,4,5,6,7] 在下标 3 处经旋转后可能变为 [4,5,6,7,0,1,2] 。

    给你 旋转后 的数组 nums 和一个整数 target ，如果 nums 中存在这个目标值 target ，则返回它的下标，否则返回 -1 。

    你必须设计一个时间复杂度为 O(log n) 的算法解决此问题。



    示例 1：

    输入：nums = [4,5,6,7,0,1,2], target = 0
    输出：4
*/

#include <vector>
#include <string>
using namespace std;

class Solution
{
public:
    int search(vector<int> &nums, int target)
    {
        int left = 0, right = nums.size();
        if (target < nums[left] && target > nums[right - 1])
            return -1;
        else if (target == nums[left])
            return 0;
        else if (target == nums[right - 1])
            return right - 1;
        else if (target > nums[left])
        {
            while (left < right)
            {
                int mid = left + (right - left) / 2;
                if (nums[mid] < nums[left] || target < nums[mid])
                    right = mid;
                else if (target > nums[mid])
                    left = mid + 1;
                else
                    return mid;
            }
        }
        else
        {
            while (left < right)
            {
                int mid = left + (right - left) / 2;
                if (nums[mid] > nums[right - 1] || target > nums[mid])
                    left = mid + 1;
                else if (target < nums[mid])
                    right = mid;
                else
                    return mid;
            }
        }
        return -1;
    }
};
