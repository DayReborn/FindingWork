/*
    给你一个按照非递减顺序排列的整数数组 nums，和一个目标值 target。请你找出给定目标值在数组中的开始位置和结束位置。

    如果数组中不存在目标值 target，返回 [-1, -1]。

    你必须设计并实现时间复杂度为 O(log n) 的算法解决此问题。


    示例 1：

    输入：nums = [5,7,7,8,8,10], target = 8
    输出：[3,4]
*/

#include <vector>
#include <string>
using namespace std;

class Solution
{
public:
    int isExist(vector<int> &nums, int target)
    {
        int left = 0, right = nums.size();
        while (left < right)
        {
            int mid = left + (right - left) / 2;
            if (nums[mid] > target)
                right = mid;
            else if (nums[mid] < target)
                left = mid + 1;
            else
                return mid;
        }
        return -1;
    }

    vector<int> searchRange(vector<int> &nums, int target)
    {
        int target_index = isExist(nums, target);
        if (target_index == -1)
            return {-1, -1};

        // 查找左边界
        int left_bound = target_index;
        int left = 0, right = target_index;
        while (left < right)
        {
            int mid = left + (right - left) / 2;
            if (nums[mid] == target)
            { // 关键修改：只要等于target就继续向左找
                right = mid;
                left_bound = mid; // 更新左边界
            }
            else
            {
                left = mid + 1;
            }
        }

        // 查找右边界
        int right_bound = target_index;
        left = target_index;
        right = nums.size() - 1;
        while (left <= right)
        { // 关键修改：允许 left == right
            int mid = left + (right - left) / 2;
            if (nums[mid] == target)
            { // 关键修改：只要等于target就继续向右找
                left = mid + 1;
                right_bound = mid; // 更新右边界
            }
            else
            {
                right = mid - 1;
            }
        }

        return {left_bound, right_bound};
    }
};

// 我的写法还是太冗余了，其实可以直接如下判断来着

class Solution
{
public:
    vector<int> searchRange(vector<int> &nums, int target)
    {
        vector<int> result = {-1, -1};
        if (nums.empty())
            return result;

        // 寻找左边界
        int left = 0, right = nums.size();
        while (left < right)
        {
            int mid = left + (right - left) / 2;
            if (nums[mid] >= target)
            {
                right = mid;
            }
            else
            {
                left = mid + 1;
            }
        }
        if (left == nums.size() || nums[left] != target)
        {
            return result;
        }
        result[0] = left;

        // 寻找右边界
        right = nums.size();
        while (left < right)
        {
            int mid = left + (right - left) / 2;
            if (nums[mid] > target)
            {
                right = mid;
            }
            else
            {
                left = mid + 1;
            }
        }
        result[1] = right - 1;

        return result;
    }
};