// 冒泡排序
// 每轮都从前往后比较相邻的两个元素，如果前一个元素大于后一个元素，则交换它们。

#include <algorithm>    // 引入 std::swap 函数
#include <iostream>     // 引入输入输出流库
#include <vector>       // 引入动态数组库


// 冒泡排序函数
void bubbleSort(std::vector<int> &arr) {
    int n = arr.size();    // 获取数组（向量）的长度

    // 外层循环控制遍历次数。n-1 次遍历就足够了。
    for (int i = 0; i < n - 1; ++i) {
        // 内层循环负责每一轮的比较和交换
        // 每次遍历后，最大的元素都会“冒泡”到数组的末尾，
        // 所以下一轮比较的范围可以缩小。
        // -i 是为了排除已经排好序的末尾元素。
        for (int j = 0; j < n - 1 - i; ++j) {
            // 如果当前元素大于下一个元素，则交换它们
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);    // 使用 std::swap 进行交换
            }
        }
    }
}

#include <algorithm>    // 引入 std::swap 函数
#include <iostream>     // 引入输入输出流库
#include <vector>       // 引入动态数组库


// 改进的冒泡排序函数
void bubbleSortOptimized(std::vector<int> &arr) {
    int n = arr.size();    // 获取数组（向量）的长度
    bool swapped;          // 标志，用于判断本趟是否发生交换

    // 外层循环使用 while 循环，只要发生过交换就继续
    // 同时也限制了最大遍历次数为 n-1
    for (int i = 0; i < n - 1; ++i) {    // 理论上最多需要 n-1 趟
        swapped = false;                 // 每趟开始前，重置交换标志为 false

        // 内层循环负责本趟的比较和交换
        // 每次遍历后，最大的元素都会“冒泡”到数组的末尾
        // -i 是为了排除已经排好序的末尾元素
        for (int j = 0; j < n - 1 - i; ++j) {
            // 如果当前元素大于下一个元素，则交换它们
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);    // 使用 std::swap 进行交换
                swapped = true;                   // 发生了交换，设置标志为 true
            }
        }
        // 如果在一整趟内都没有发生任何交换，即每两两元素之间都是顺序的，说明数组已经有序，提前终止
        if (!swapped) {
            break;    // 退出外层循环
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
    std::vector<int> arr1 = {64, 34, 25, 12, 22, 11, 90};
    std::cout << "原始数组 1: ";
    printArray(arr1);
    bubbleSortOptimized(arr1);
    std::cout << "排序后的数组 1: ";
    printArray(arr1);

    std::cout << std::endl;    // 添加空行以便区分

    // 示例用法 2：部分有序或基本有序的数组，可以体现优化效果
    std::vector<int> arr2 = {1, 2, 5, 4, 3, 6, 7};
    std::cout << "原始数组 2: ";
    printArray(arr2);
    // 观察这里，排序会更快完成，因为在倒数第二趟时，3和4交换后，下一趟就不会再发生交换
    bubbleSortOptimized(arr2);
    std::cout << "排序后的数组 2: ";
    printArray(arr2);

    std::cout << std::endl;    // 添加空行以便区分

    // 示例用法 3：已排序的数组
    std::vector<int> arr3 = {10, 20, 30, 40, 50};
    std::cout << "原始数组 3 (已排序): ";
    printArray(arr3);
    // 此时第一趟循环后 swapped 仍为 false，排序会立即终止
    bubbleSortOptimized(arr3);
    std::cout << "排序后的数组 3: ";
    printArray(arr3);

    return 0;
}

// 打印数组的辅助函数
void printArray(const std::vector<int> &arr) {
    for (int num : arr) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
}

int main() {
    // 示例用法
    std::vector<int> myVector = {64, 34, 25, 12, 22, 11, 90};

    std::cout << "原始数组: ";
    printArray(myVector);

    bubbleSort(myVector);    // 调用冒泡排序函数

    std::cout << "排序后的数组: ";
    printArray(myVector);

    // 另一个测试用例
    std::vector<int> anotherVector = {5, 1, 4, 2, 8};
    std::cout << "\n原始数组2: ";
    printArray(anotherVector);
    bubbleSort(anotherVector);
    std::cout << "排序后的数组2: ";
    printArray(anotherVector);

    return 0;    // 程序成功执行
}
