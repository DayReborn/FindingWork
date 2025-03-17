// 347. 前 K 个高频元素
// 给你一个整数数组 nums 和一个整数 k ，请你返回其中出现频率前 k 高的元素。你可以按 任意顺序 返回答案。

// 示例 1:

// 输入: nums = [1,1,1,2,2,3], k = 2
// 输出: [1,2]
// 示例 2:

// 输入: nums = [1], k = 1
// 输出: [1]

#include <vector>
#include <algorithm>
#include <string>
#include <stack>
#include <queue>
#include <cstdlib>
#include <ctime>

using namespace std;

#include <vector>
#include <unordered_map>
#include <queue>

using namespace std;

/*
================================================================================
方法一：最小堆法
时间复杂度：O(n log k) - 遍历n个元素，每次堆操作为O(log k)
空间复杂度：O(n + k)   - 哈希表O(n)，堆O(k)
适用场景：k远小于n时效率高，不需要知道最大频率
================================================================================
*/
class Solution {
public:
    vector<int> topKFrequent(vector<int>& nums, int k) {
        // 1. 统计频率
        unordered_map<int, int> freqMap;
        for (int num : nums) {
            freqMap[num]++;
        }

        // 2. 定义最小堆（比较函数为频率升序）
        using Pair = pair<int, int>; // pair格式: (频率, 元素)
        auto cmp = [](const Pair& a, const Pair& b) {
            return a.first > b.first; // 小顶堆用大于号
        };
        priority_queue<Pair, vector<Pair>, decltype(cmp)> minHeap(cmp);

        // 3. 维护大小为k的堆
        for (auto& [num, count] : freqMap) {
            minHeap.emplace(count, num);
            if (minHeap.size() > k) {
                minHeap.pop(); // 弹出最小频率元素
            }
        }

        // 4. 提取结果
        vector<int> res;
        while (!minHeap.empty()) {
            res.emplace_back(minHeap.top().second);
            minHeap.pop();
        }
        return res; // 结果顺序不影响答案要求
    }
};

/*
================================================================================
方法二：桶排序法
时间复杂度：O(n)        - 统计频率和收集结果均为线性时间
空间复杂度：O(n)        - 哈希表和桶占用的空间
适用场景：元素频率分布集中时效率极高
================================================================================
*/
class Solution {
public:
    vector<int> topKFrequent(vector<int>& nums, int k) {
        // 1. 统计频率并记录最大频率
        unordered_map<int, int> freqMap;
        int maxFreq = 0;
        for (int num : nums) {
            freqMap[num]++;
            maxFreq = max(maxFreq, freqMap[num]);
        }

        // 2. 创建频率桶（索引为频率，值为元素列表）
        vector<vector<int>> buckets(maxFreq + 1);
        for (auto& [num, count] : freqMap) {
            buckets[count].push_back(num);
        }

        // 3. 从高到低收集结果
        vector<int> res;
        for (int i = maxFreq; i >= 1 && res.size() < k; --i) {
            for (int num : buckets[i]) {
                res.push_back(num);
                if (res.size() == k) { // 提前终止
                    return res;
                }
            }
        }
        return res;
    }
};

/*
================================================================================
方法对比：
1. 最小堆法优势：
   - 不需要知道最大频率
   - 当k较小时空间效率更高
2. 桶排序法优势：
   - 严格线性时间复杂度
   - 代码更简洁直观
================================================================================
*/