// 快速排序
// 任取一个元素作为基准，将数组分为两部分，小于基准的在左边，大于基准的在右边，然后递归地对左右两部分进行排序。
// 直到空或只有一个元素的子数组被认为是有序的。

// 时间复杂度: 最差O(n^2)，最好O(n log n)，平均O(n log n)
// 最好情况分析： 每次划分都能将数组均匀分成两半，递归树很平衡，高度为 log n，每层处理 n 个元素，总时间复杂度为 O(n log n)
// 最差情况分析： 每次划分都极度不均匀（例如已经有序的数组，每次选择的基准是最大或最小元素），递归树退化成链表，高度为 n，每层处理 n 个元素，总时间复杂度为 O(n^2)

// 空间复杂度: O(log n)（递归栈空间）
// 稳定性: 不稳定

#include <algorithm>    // 引入 std::swap 函数 (虽然填坑法中不直接用，但保留以防万一)
#include <iostream>     // 引入输入输出流库
#include <vector>       // 引入动态数组库

// 打印数组的辅助函数
void printArray(const std::vector<int> &arr) {
    for (int num : arr) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
}

// 分区函数：使用左基准和填坑法
// 返回基准最终的索引
int partition(std::vector<int> &arr, int low, int high) {
    // 1. 选择最左边的元素作为基准
    int pivot = arr[low];

    // 2. 左右指针
    int left = low;
    int right = high;

    // 填坑过程
    // 坑在哪边，哪边就不动；另一边往中间靠拢
    // 内部两个小while循环一定可以控制在left==right时结束
    while (left < right) {
        // 一开始坑在左边，所以左边不动，右边往中间靠
        // 从右向左找第一个小于基准的元素，并将其填入 left 指向的坑
        while (left < right && arr[right] >= pivot) {
            right--;
        }
        if (left < right) {
            arr[left] = arr[right];    // 填坑
            left++;                    // 左指针右移，形成新的坑
        }

        // 坑转移到右边去了，所以右边不动，左边往右边靠
        // 从左向右找第一个大于基准的元素，并将其填入 right 指向的坑
        while (left < right && arr[left] <= pivot) {
            left++;
        }
        if (left < right) {
            arr[right] = arr[left];    // 填坑
            right--;                   // 右指针左移，形成新的坑
        }
    }

    // 左右指针重合时，循环结束，此时 left（或 right）就是基准的最终位置
    arr[left] = pivot;    // 将基准填入最终的坑中

    return left;          // 返回基准的最终索引
}

// 快速排序主函数
void quickSort(std::vector<int> &arr, int low, int high) {
    // 这个if条件是递归的终止条件
    // 如果不满足 low < high，说明子数组为空或只有一个元素，已经是有序的
    if (low < high) {
        // pi 是分区索引，arr[pi] 现在在它的正确位置
        int pi = partition(arr, low, high);

        // 递归地对左右两个子数组进行排序
        quickSort(arr, low, pi - 1);     // 排序左子数组
        quickSort(arr, pi + 1, high);    // 排序右子数组
    }
}

int main() {
    // 示例用法 1：无序数组
    std::vector<int> arr1 = {10, 7, 8, 9, 1, 5};
    std::cout << "原始数组 1: ";
    printArray(arr1);
    quickSort(arr1, 0, arr1.size() - 1);
    std::cout << "排序后的数组 1: ";
    printArray(arr1);

    // 示例用法 2：更大的数组
    std::vector<int> arr2 = {2, 8, 7, 1, 3, 5, 6, 4, 9, 0};
    std::cout << "\n原始数组 2: ";
    printArray(arr2);
    quickSort(arr2, 0, arr2.size() - 1);
    std::cout << "排序后的数组 2: ";
    printArray(arr2);

    // 示例用法 3：已排序的数组（展示最坏情况之一）
    std::vector<int> arr3 = {1, 2, 3, 4, 5};
    std::cout << "\n原始数组 3 (已排序): ";
    printArray(arr3);
    quickSort(arr3, 0, arr3.size() - 1);
    std::cout << "排序后的数组 3: ";
    printArray(arr3);

    return 0;
}
