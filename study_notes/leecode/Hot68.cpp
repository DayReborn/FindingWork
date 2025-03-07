/*
    给定两个大小分别为 m 和 n 的正序（从小到大）数组 nums1 和 nums2。请你找出并返回这两个正序数组的 中位数 。

    算法的时间复杂度应该为 O(log (m+n))  。



    示例 1：

    输入：nums1 = [1,3], nums2 = [2]
    输出：2.00000
    解释：合并数组 = [1,2,3] ，中位数 2
*/
#include <vector>
#include <string>

#include <algorithm> // max函数定义在此头文件中

using namespace std;

class Solution1
{
public:
    double findMedianSortedArrays(vector<int> &nums1, vector<int> &nums2)
    {
        int loc1 = 0, loc2 = 0;
        int count = 0;
        double result = 0.0;
        int total = nums1.size() + nums2.size();
        int left = (total - 1) / 2;
        int right = total / 2;

        while (loc1 < nums1.size() && loc2 < nums2.size())
        {
            if (nums1[loc1] <= nums2[loc2])
            {
                if (count == left)
                {
                    result += nums1[loc1];
                }
                if (count == right)
                {
                    result += nums1[loc1];
                    return result / 2.0;
                }
                count++;
                loc1++;
            }
            else
            {
                if (count == left)
                {
                    result += nums2[loc2];
                }
                if (count == right)
                {
                    result += nums2[loc2];
                    return result / 2.0;
                }
                count++;
                loc2++;
            }
        }

        // 处理nums1剩余的元素
        while (loc1 < nums1.size())
        {
            if (count == left)
            {
                result += nums1[loc1];
            }
            if (count == right)
            {
                result += nums1[loc1];
                return result / 2.0;
            }
            count++;
            loc1++;
        }

        // 处理nums2剩余的元素
        while (loc2 < nums2.size())
        {
            if (count == left)
            {
                result += nums2[loc2];
            }
            if (count == right)
            {
                result += nums2[loc2];
                return result / 2.0;
            }
            count++;
            loc2++;
        }

        return 0.0;
    }
};

class Solution2
{
public:
    double findMedianSortedArrays(vector<int> &nums1, vector<int> &nums2)
    {
        // 确保nums1是较短的数组，减少二分次数
        if (nums1.size() > nums2.size())
        {
            return findMedianSortedArrays(nums2, nums1);
        }

        int m = nums1.size();
        int n = nums2.size();
        int left = 0, right = m;
        int totalLeft = (m + n + 1) / 2; // 左半部分的总元素数

        while (left <= right)
        {
            int i = (left + right) / 2; // nums1的分割点
            int j = totalLeft - i;      // nums2的分割点

            // 处理nums1的分割点i的边界情况
            int nums1LeftMax = (i == 0) ? INT_MIN : nums1[i - 1];
            int nums1RightMin = (i == m) ? INT_MAX : nums1[i];

            // 处理nums2的分割点j的边界情况
            int nums2LeftMax = (j == 0) ? INT_MIN : nums2[j - 1];
            int nums2RightMin = (j == n) ? INT_MAX : nums2[j];

            if (nums1LeftMax <= nums2RightMin && nums2LeftMax <= nums1RightMin)
            {
                // 找到正确的分割点
                if ((m + n) % 2 == 1)
                {
                    return max(nums1LeftMax, nums2LeftMax);
                }
                else
                {
                    return (max(nums1LeftMax, nums2LeftMax) + min(nums1RightMin, nums2RightMin)) / 2.0;
                }
            }
            else if (nums1LeftMax > nums2RightMin)
            {
                right = i - 1; // i需要减小
            }
            else
            {
                left = i + 1; // i需要增大
            }
        }

        return 0.0; // 不会执行到此，输入保证有效性
    }
};