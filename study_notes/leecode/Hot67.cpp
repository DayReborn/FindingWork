#include <vector>
#include <string>
using namespace std;

class Solution
{
public:
    int findMin(vector<int> &nums)
    {
        int left = 0, right = nums.size() - 1; // 左闭右闭区间
        while (left < right)
        {
            int mid = left + (right - left) / 2;
            if (nums[mid] > nums[right])
            {
                left = mid + 1; // 最小值在右侧
            }
            else
            {
                right = mid; // 最小值在左侧
            }
        }
        return nums[left]; // 结束时 left == right
    }
};