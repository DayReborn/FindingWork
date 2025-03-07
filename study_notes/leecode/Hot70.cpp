#include <vector>
#include <algorithm>
#include <utility>

using namespace std;

class MinStack
{
private:
    vector<int> nums;      // 主栈
    vector<int> min_stack; // 辅助栈，记录每个位置的最小值

public:
    MinStack()
    {
        // 无需显式初始化，vector会自动管理
    }

    void push(int val)
    {
        nums.push_back(val);
        if (min_stack.empty())
        {
            min_stack.push_back(val);
        }
        else
        {
            min_stack.push_back(min(val, min_stack.back()));
        }
    }

    void pop()
    {
        nums.pop_back();
        min_stack.pop_back();
    }

    int top()
    {
        return nums.back();
    }

    int getMin()
    {
        return min_stack.back();
    }
};

class MinStack
{
public:
    MinStack()
    {
        length = 0;
    }

    void push(int val)
    {
        if (length == 0)
            nums.emplace_back(val, val);
        else
            nums.emplace_back(val, min(val, nums[length - 1].second));
        length++;
    }

    void pop()
    {
        nums.pop_back();
        length--;
    }

    int top()
    {
        return nums[length - 1].first;
    }

    int getMin()
    {
        return nums[length - 1].second;
    }

    vector<pair<int, int>> nums;
    int length;
};

/**
 * Your MinStack object will be instantiated and called as such:
 * MinStack* obj = new MinStack();
 * obj->push(val);
 * obj->pop();
 * int param_3 = obj->top();
 * int param_4 = obj->getMin();
 */