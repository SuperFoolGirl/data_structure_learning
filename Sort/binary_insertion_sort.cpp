// 折半插入排序
// 折半插入排序是插入排序的一种优化，它利用二分查找来确定待插入元素在已排序部分的正确位置，从而减少了比较的次数

#include <algorithm>    // 包含 std::upper_bound 用于比较，但我们自己实现二分查找
#include <iostream>
#include <vector>


// 封装的二分查找函数
// 在 arr[low...high] 范围内查找 key 应该插入的位置
// 返回第一个大于等于 key 的元素的索引
int binarySearchForInsertion(const std::vector<int> &arr, int low, int high, int key) {
    int insertPos = high + 1;    // 默认插入位置为当前搜索范围的右边界+1 (即比所有元素都大)

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] > key) {
            insertPos = mid;    // 找到一个可能的插入位置
            high = mid - 1;     // 继续在左半部分查找更靠前的插入位置
        } else {
            low = mid + 1;      // arr[mid] <= key，说明 key 应该插入到 mid 的右边
        }
    }
    return insertPos;
}

void binaryInsertionSort(std::vector<int> &arr) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        int key = arr[i];    // 待插入的当前元素

        // 调用封装的二分查找函数，在已排序部分 (arr[0...i-1]) 找到 key 的正确插入位置
        // 注意：这里 high 是 i - 1
        int insertPos = binarySearchForInsertion(arr, 0, i - 1, key);

        // 将从 insertPos 到 i-1 的元素向右移动，为 key 腾出位置
        // 注意：这里是从 i-1 开始，避免覆盖 key
        for (int j = i - 1; j >= insertPos; --j) {
            arr[j + 1] = arr[j];
        }

        // 插入 key
        arr[insertPos] = key;
    }
}

int main() {
    std::vector<int> arr = {12, 11, 13, 5, 6, 7, 9, 1};

    std::cout << "原始数组: ";
    for (int x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    binaryInsertionSort(arr);

    std::cout << "排序后的数组: ";
    for (int x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    return 0;
}
