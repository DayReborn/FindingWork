// 215. 数组中的第K个最大元素
// 中等
// 给定整数数组 nums 和整数 k，请返回数组中第 k 个最大的元素。

// 请注意，你需要找的是数组排序后的第 k 个最大的元素，而不是第 k 个不同的元素。

// 你必须设计并实现时间复杂度为 O(n) 的算法解决此问题。
// 示例 1:

// 输入: [3,2,1,5,6,4], k = 2
// 输出: 5

#include <vector>
#include <algorithm>
#include <string>
#include <stack>
#include <queue>
#include <cstdlib>
#include <ctime>

using namespace std;

/*
================================================================================
写法一：最大堆暴力解法 (不符合题目要求)
================================================================================
原理：
1. 将所有元素压入最大堆
2. 连续弹出k-1次堆顶元素
3. 当前堆顶即为第k大元素

时间复杂度：O(n log n + k log n)
空间复杂度：O(n)
问题：虽然代码逻辑正确，但时间复杂度不符合题目要求的O(n)
*/
class Solution {
    public:
        int findKthLargest(vector<int> &nums, int k) {
            priority_queue<int> temp;
            for (int value : nums) {
                temp.push(value); // O(n log n) 堆插入操作
            }
            while (k-- > 1) {     // 执行k-1次弹出
                temp.pop();       // 每次弹出O(log n)
            }
            return temp.top();    // 最终堆顶为答案
        }
    };
    
/*
================================================================================
写法二：最小堆优化解法 (不符合题目要求)
================================================================================
原理：
1. 维护大小为k的最小堆
2. 遍历时保留最大的k个元素
3. 堆顶始终为当前第k大元素

时间复杂度：O(n log k) 空间复杂度：O(k)
优势：比最大堆更节省空间(k较小时)
问题：时间复杂度仍含log因子，不符合O(n)要求
*/
int findKthLargest(vector<int>& nums, int k) {
    priority_queue<int, vector<int>, greater<int>> minHeap; // 最小堆定义
    
    for (int num : nums) {
        if (minHeap.size() < k) {          // 堆未满时直接插入
            minHeap.push(num);             // O(log k)
        } else if (num > minHeap.top()) {  // 发现更大元素时更新堆
            minHeap.pop();                 // 弹出最小值 O(log k)
            minHeap.push(num);             // 插入新值 O(log k)
        }
    }
    return minHeap.top(); // 堆顶即为第k大元素
}

/*
================================================================================
写法三：快速选择算法 (符合题目要求)
================================================================================
原理：
1. 基于快速排序的分区思想
2. 随机选择枢轴进行分区
3. 根据枢轴位置决定递归左/右区间

时间复杂度：平均O(n)，最坏O(n²) (通过随机化避免)
空间复杂度：O(1) (尾递归优化)
注意：原始代码存在语法错误，已修正下列问题：
1. nums.size()1 -> nums.size()-1
2. pivotIndex1 -> pivotIndex-1
*/
class Solution {
public:
    int findKthLargest(vector<int>& nums, int k) {
        srand(time(0)); // 初始化随机数种子
        return quickSelect(nums, 0, nums.size()-1, k); // 修正size()-1
    }

private:
    int quickSelect(vector<int>& nums, int left, int right, int k) {
        if (left == right) return nums[left]; // 递归终止条件
        
        int pivotIndex = partition(nums, left, right); // 获取枢轴位置
        int rank = pivotIndex - left + 1;  // 计算枢轴在子区间中的排名
        
        if (rank == k) { // 找到目标元素
            return nums[pivotIndex];
        } else if (rank > k) { // 在左区间继续查找
            return quickSelect(nums, left, pivotIndex-1, k); // 修正pivotIndex-1
        } else { // 在右区间查找
            return quickSelect(nums, pivotIndex+1, right, k-rank);
        }
    }

    // 分区函数 (降序分区)
    int partition(vector<int>& nums, int left, int right) {
        int pivotIndex = left + rand() % (right - left + 1); // 随机选择枢轴
        int pivot = nums[pivotIndex];
        
        swap(nums[pivotIndex], nums[right]); // 将枢轴移至末尾
        int storeIndex = left; // 记录大于枢轴的元素存放位置
        
        for (int i = left; i < right; i++) {
            if (nums[i] >= pivot) { // 降序排列(找第k大)
                swap(nums[i], nums[storeIndex]);
                storeIndex++;
            }
        }
        swap(nums[storeIndex], nums[right]); // 将枢轴移至最终位置
        return storeIndex; // 返回枢轴的正确索引
    }
};

/*
================================================================================
算法选择建议：
1. 面试场景：必须实现快速选择算法以满足O(n)要求
2. 工程场景：推荐最小堆解法，更稳定且代码简洁
3. 特殊场景：若输入规模较小，最大堆暴力解法更易实现
================================================================================
*/