// 选择排序
// 在未排序的序列中找到最小（或最大）元素，存放到排序序列的起始位置，然后再从剩余未排序元素中继续寻找最小（或最大）元素，然后放到已排序序列的末尾
// 每轮找最小元素的方法：擂台的方式记录最小元素下标，不断更新

#include <algorithm>    // 引入 std::swap 函数
#include <iostream>     // 引入输入输出流库
#include <vector>       // 引入动态数组库

// 简单选择排序函数
void selectionSort(std::vector<int> &arr) {
    int n = arr.size();    // 获取数组（向量）的长度

    // 外层循环：控制需要进行多少轮的选择
    // 每进行一轮，就有一个元素被放到正确的位置
    for (int i = 0; i < n - 1; ++i) {
        // 假设当前未排序部分的第一个元素是最小值
        int min_idx = i;

        // 内层循环：在未排序部分查找真正的最小值
        for (int j = i + 1; j < n; ++j) {
            // 如果找到比当前 min_idx 处元素更小的值，更新 min_idx
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }

        // 如果最小值不是当前位置的元素，则交换它们
        // 这样，当前位置 (i) 就存放了未排序部分中最小的元素
        if (min_idx != i) {
            std::swap(arr[i], arr[min_idx]);
        }
    }
}

// 打印数组的辅助函数
void printArray(const std::vector<int> &arr) {
    for (int num : arr) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
}

int main() {
    // 示例用法 1：无序数组
    std::vector<int> arr1 = {64, 25, 12, 22, 11};
    std::cout << "原始数组 1: ";
    printArray(arr1);
    selectionSort(arr1);
    std::cout << "排序后的数组 1: ";
    printArray(arr1);

    // 示例用法 2：更大的数组
    std::vector<int> arr2 = {5, 1, 4, 2, 8, 0, 9, 3, 7, 6};
    std::cout << "\n原始数组 2: ";
    printArray(arr2);
    selectionSort(arr2);
    std::cout << "排序后的数组 2: ";
    printArray(arr2);

    // 示例用法 3：已排序的数组
    std::vector<int> arr3 = {1, 2, 3, 4, 5};
    std::cout << "\n原始数组 3 (已排序): ";
    printArray(arr3);
    selectionSort(arr3);
    std::cout << "排序后的数组 3: ";
    printArray(arr3);

    return 0;
}
