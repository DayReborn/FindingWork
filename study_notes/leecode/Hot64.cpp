/*
    给你一个满足下述两条属性的 m x n 整数矩阵：

    每行中的整数从左到右按非严格递增顺序排列。
    每行的第一个整数大于前一行的最后一个整数。
    给你一个整数 target ，如果 target 在矩阵中，返回 true ；否则，返回 false 。

    

    示例 1：


    输入：matrix = [[1,3,5,7],[10,11,16,20],[23,30,34,60]], target = 3
    输出：true
*/

#include <vector>
#include <string>
using namespace std;


class Solution {
    public:
        bool searchMatrix(vector<vector<int>>& matrix, int target) {
            int row = matrix.size();
            int col = matrix[0].size();
            
            // 初始检查：目标是否在矩阵范围内
            if (matrix[0][0] > target || matrix[row-1][col-1] < target)
                return false;
            
            // 步骤1: 确定目标所在的行
            int left = 0, right = row - 1;
            while (left <= right) {
                int mid = left + (right - left) / 2; // 防止整数溢出
                if (matrix[mid][0] > target) {
                    right = mid - 1;
                } else if (matrix[mid][0] < target) {
                    left = mid + 1;
                } else {
                    return true; // 直接找到目标
                }
            }
            int targetRow = right; // 此时right指向最后一个不大于target的行
            
            // 检查该行是否存在可能
            if (matrix[targetRow][col-1] < target)
                return false;
            
            // 步骤2: 在该行中进行列搜索
            left = 0;
            right = col - 1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                if (matrix[targetRow][mid] > target) {
                    right = mid - 1;
                } else if (matrix[targetRow][mid] < target) {
                    left = mid + 1;
                } else {
                    return true;
                }
            }
            return false;
        }
    };