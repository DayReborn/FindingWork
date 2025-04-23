// 给你一个整数数组 arr ，以及 a、b 、c 三个整数。请你统计其中好三元组的数量。

// 如果三元组 (arr[i], arr[j], arr[k]) 满足下列全部条件，则认为它是一个 好三元组 。

// 0 <= i < j < k < arr.length
// |arr[i] - arr[j]| <= a
// |arr[j] - arr[k]| <= b
// |arr[i] - arr[k]| <= c
// 其中 |x| 表示 x 的绝对值。

// 返回 好三元组的数量 。

// 示例 1：

// 输入：arr = [3,0,1,1,9,7], a = 7, b = 2, c = 3
// 输出：4
// 解释：一共有 4 个好三元组：[(3,0,1), (3,0,1), (3,1,1), (0,1,1)] 。
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;



class Solution
{
public:
    int countGoodTriplets(vector<int> &arr, int a, int b, int c)
    {
        int len = arr.size();
        int res = 0;
        for(int i = 0; i<len-2;++i){
            for(int j = i+1; j<len-1;++j){
                if(abs(arr[i]-arr[j]) > a) continue;
                for(int k = j+1; k<len;++k){
                    if(abs(arr[j]-arr[k])<=b && abs(arr[i]-arr[k])<=c) res++;                    
                }
            }
        }
        return res;
    }
};


class Solution
{
public:
    int countGoodTriplets(vector<int> &arr, int a, int b, int c)
    {
        int len = arr.size();
        int res = 0;
        for(int i = 0; i<len-2;++i){
            for(int j = i+1; j<len-1;++j){
                if(abs(arr[i]-arr[j]) > a) continue;
                for(int k = j+1; k<len;++k){
                    if(abs(arr[j]-arr[k])<=b && abs(arr[i]-arr[k])<=c) res++;                    
                }
            }
        }
        return res;
    }
};



