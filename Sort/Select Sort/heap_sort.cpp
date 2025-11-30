// 堆排序

// 1. 首先，将乱序数组构建成大根堆
// 构建方法：从最后一个非叶子节点开始，依次对每个节点进行下沉操作
// 下沉操作：将当前节点与其子节点中较大的一个交换，直到满足堆的性质
// 由于是自底向上调整，所以儿子先下沉，父亲后下沉；但父亲的下沉可能会影响儿子的下沉，所以需要重新自顶向下调整

// 2. 每次根结点与最后一个元素交换，然后下沉新根节点。共n-1次
// 每次交换下来的根节点要隐藏，不再参与堆的调整

// 建堆
// 时间复杂度: O(n)，数学证明比较复杂
// 排序
// 时间复杂度: 最差O(n log n)，最好O(n log n)，平均O(n log n)
// 空间复杂度: O(1)，原地排序
// 稳定性: 不稳定

#include <algorithm>    // 用于std::swap
#include <iostream>
#include <vector>

// 辅助函数：下沉操作，用于维护堆的性质
// array: 待调整的数组
// heapSize: 当前堆的大小（参与调整的元素范围）
// idx: 需要下沉的元素的索引
void heapify(std::vector<int> &array, int heapSize, int idx) {
    int largest = idx;               // 假设当前节点是最大的
    int leftChild = 2 * idx + 1;     // 左子节点的索引
    int rightChild = 2 * idx + 2;    // 右子节点的索引

    // 如果左子节点存在且大于当前最大值，则更新最大值索引
    if (leftChild < heapSize && array[leftChild] > array[largest]) {
        largest = leftChild;
    }

    // 如果右子节点存在且大于当前最大值，则更新最大值索引
    if (rightChild < heapSize && array[rightChild] > array[largest]) {
        largest = rightChild;
    }

    // 如果最大值不是当前节点，则交换并递归地对受影响的子树进行下沉操作
    if (largest != idx) {
        std::swap(array[idx], array[largest]);
        heapify(array, heapSize, largest);
    }
}

// 堆排序
void heapSort(std::vector<int> &array) {
    int n = array.size();
    if (n <= 1) {
        return;    // 0或1个元素的数组无需排序
    }

    // 1. 构建大根堆
    // 从最后一个非叶子节点开始，向上遍历所有非叶子节点，并对每个节点执行下沉操作
    // 最后一个非叶子节点的索引是 (n / 2) - 1
    for (int i = n / 2 - 1; i >= 0; --i) {
        heapify(array, n, i);
    }

    // 2. 堆排序
    // 每次将堆顶元素（最大值）与当前堆的最后一个元素交换
    // 然后将堆的大小减一，并对新的堆顶元素执行下沉操作，将已经下来的根节点排除（通过下沉的参数二来控制）
    // 这个思想很厉害！用函数参数来控制访问区域，而不是再增加空间复杂度来引入新数据结构
    for (int i = n - 1; i > 0; --i) {
        std::swap(array[0], array[i]);    // 将当前最大值（堆顶）与最后一个元素交换
        heapify(array, i, 0);             // 对剩余的i个元素（排除已排序的最大值）进行下沉操作
    }
}

// 辅助函数：打印数组
void printArray(const std::vector<int> &array) {
    for (int x : array) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

int main() {
    std::vector<int> arr = {12, 11, 13, 5, 6, 7};
    std::cout << "原始数组: ";
    printArray(arr);

    heapSort(arr);

    std::cout << "排序后的数组: ";
    printArray(arr);

    std::vector<int> arr2 = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
    std::cout << "原始数组2: ";
    printArray(arr2);

    heapSort(arr2);

    std::cout << "排序后的数组2: ";
    printArray(arr2);

    return 0;
}
