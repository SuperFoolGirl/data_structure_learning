// 希尔排序
// 插入排序的改进

// 对于一个接近有序的数组，插入排序的效率很高
// 进行多轮插入排序，每次插入的间隔变为上一次的一半，除以2的结果按编程语言，自动向下取整
// 直到间隔为1时，实际上就是普通的插入排序了
// 但由于前面的间隔插入排序，已经使得数组接近有序，因此最后的插入排序效率会很高

#include <algorithm>    // 用于 std::swap
#include <iostream>
#include <vector>

// 希尔排序函数
void shellSort(std::vector<int> &arr) {
    int n = arr.size();

    // 从一个较大的间隔开始，逐渐缩小间隔
    // 通常选择 N/2, N/4 ... 1
    for (int gap = n / 2; gap > 0; gap /= 2) {
        // 对每个间隔内的元素进行插入排序
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j;
            // 将 arr[i] 插入到已排序的间隔子序列中
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
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
    std::vector<int> arr = {12, 34, 54, 2, 3, 45, 23, 9, 78, 56};

    std::cout << "原始数组: ";
    printArray(arr);

    shellSort(arr);

    std::cout << "排序后的数组: ";
    printArray(arr);

    // 另一个测试用例
    std::vector<int> arr2 = {5, 1, 4, 2, 8};
    std::cout << "\n原始数组2: ";
    printArray(arr2);
    shellSort(arr2);
    std::cout << "排序后的数组2: ";
    printArray(arr2);

    return 0;
}
