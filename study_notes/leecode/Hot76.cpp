/**
 * 中位数是有序整数列表中的中间值。如果列表的大小是偶数，则没有中间值，中位数是两个中间值的平均值。
 *
 * 例如 arr = [2,3,4] 的中位数是 3 。
 * 例如 arr = [2,3] 的中位数是 (2 + 3) / 2 = 2.5 。
 *  实现 MedianFinder 类:
 * MedianFinder() 初始化 MedianFinder 对象。
 * void addNum(int num) 将数据流中的整数 num 添加到数据结构中。
 *
 * double findMedian() 返回到目前为止所有元素的中位数。与实际答案相差 10-5 以内的答案将被接受。
 * 示例 1：
 * 输入
 * ["MedianFinder", "addNum", "addNum", "findMedian", "addNum", "findMedian"]
 * [[], [1], [2], [], [3], []]
 *
 * 输出
 *
 * [null, null, null, 1.5, null, 2.0]
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;


class MedianFinder
{
public:
    MedianFinder()
    {
    }

    void addNum(int num)
    {
        if (maxHeapSize == 0)
        {
            maxHeap.push(num);
            maxHeapSize++;
            return;
        }
        if (num <= maxHeap.top() || minHeapSize == 0)
        {
            maxHeap.push(num);
            maxHeapSize++;
        }
        else
        {
            minHeap.push(num);
            minHeapSize++;
        }
        if (minHeapSize == maxHeapSize + 2)
        {
            maxHeap.push(minHeap.top());
            minHeap.pop();
            minHeapSize--;
            maxHeapSize++;
        }
        else if (maxHeapSize == minHeapSize + 2)
        {
            minHeap.push(maxHeap.top());
            maxHeap.pop();
            maxHeapSize--;
            minHeapSize++;
        }
    }

    double findMedian()
    {
        if (maxHeapSize == minHeapSize)
        {
            return (maxHeap.top() + minHeap.top()) / 2.0;
        }
        else if (maxHeapSize > minHeapSize)
        {
            return maxHeap.top();
        }
        else
        {
            return minHeap.top();
        }
    }

private:
    priority_queue<int, vector<int>, less<int>> maxHeap;
    priority_queue<int, vector<int>, greater<int>> minHeap;
    int maxHeapSize = 0;
    int minHeapSize = 0;
};

// 写一个简单版本的
class MedianFinder{ 
public:
    MedianFinder() {
    }
    void addNum(int num) {
        if (maxHeap.empty() || num <= maxHeap.top()) {
            maxHeap.push(num);
            if (maxHeap.size() > minHeap.size() + 1) {
                minHeap.push(maxHeap.top());
                maxHeap.pop();
            }
        } else {
            minHeap.push(num);
            if (minHeap.size() > maxHeap.size()) {
                maxHeap.push(minHeap.top());
                minHeap.pop();
            }
        }
    }
    double findMedian() {
        if (maxHeap.size() == minHeap.size()) {
            return (maxHeap.top() + minHeap.top()) / 2.0;
        } else {
            return maxHeap.top();
        }
    }
private:
    priority_queue<int> maxHeap;
    priority_queue<int, vector<int>, greater<int>> minHeap;
};


/**
 * Your MedianFinder object will be instantiated and called as such:
 * MedianFinder* obj = new MedianFinder();
 * obj->addNum(num);
 * double param_2 = obj->findMedian();
 */