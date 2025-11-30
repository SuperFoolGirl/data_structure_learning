// 插入排序
// 插入排序的思想在B树代码中有所体现

// 核心思想：依次把未排序的元素插入到已排序的部分
// 对于无序区，每次取出一个元素，在有序区中从后往前查找插入位置
// 如果比有序区最后一个还大，就直接放在最后
// 否则，依次将有序区比它大的元素向后移动一位，直到找到合适的位置

// 时间复杂度：最差O(n^2)（完全逆序），最好有序O(n)（已经有序），平均O(n^2)
// 空间复杂度：O(1)，原地排序，只需要用单变量缓存作为辅助空间
// 稳定性：稳定

#include <iostream>
#include <vector>

void insertionSort(std::vector<int> &arr) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        // 待插入的当前元素
        int key = arr[i];
        // 已排序部分的最后一个元素的索引
        int j = i - 1;

        // 将比 key 大的元素向右移动一个位置
        while (j >= 0 && arr[j] > key) {    // arr[j]>key 取等则不稳定
            // 向后移动，直接拷贝到后面去
            arr[j + 1] = arr[j];
            j--;
        }
        // 将 key 插入到正确的位置
        // 如果跳出了while循环，说明此时key >= arr[j]
        // 则key应当插到arr[j + 1]的位置
        // 而原本的 arr[j+1] 已经被向后移动了，在上一轮已经令arr[j+2] = arr[j+1]了
        // 因此这里直接把 key 放到 arr[j+1] 的位置进行覆盖即可
        arr[j + 1] = key;
    }
}

int main() {
    std::vector<int> arr = {12, 11, 13, 5, 6};

    std::cout << "原始数组: ";
    for (int x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    insertionSort(arr);

    std::cout << "排序后的数组: ";
    for (int x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    return 0;
}
