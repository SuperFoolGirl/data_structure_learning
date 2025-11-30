// 归并排序

// 所谓归并，就是对于两个有序数组，合并成一个有序数组，这个早就已经掌握了
// 每次选择数组里最小的元素放入结果数组；如果其中一个数组空了，就将另一个数组剩余的元素全部放入结果数组

// 每轮都对相邻子序列两两归并
// 第一轮中，长度为1的子序列两两归并，得到长度为2的有序子序列；如果数组长度为奇数，则最后一个元素单独成一个子序列
// 第二轮中，长度为2的子序列两两归并，得到长度为4的有序子序列
// 以此类推，直到整个数组归并为一个有序序列

// 归并排序的核心思想是“分而治之”：

// 分：将待排序的数组不断地一分为二，直到每个子数组只包含一个元素（或为空），此时认为它们是已排序的。
// 治（归并）：将这些小的、有序的子数组两两合并，形成更大的、有序的子数组，直到所有子数组合并成一个完整的有序数组。

// 在代码实现上，需要用到递归
// 递下去的时候，将数组分成两半，直到每个子数组只包含一个元素
// 然后在递归返回时，将两个有序子数组合并成一个
// 对于这个递归算法的设计，好好掌握思想。另外可以借助树的前中后序遍历来理解
// 将树的前中后序遍历对应到递归树上，是非常精妙、对递归理解非常有帮助的

// 归并排序是稳定的。稳定与否取决于合并操作是否保持相同元素的相对顺序
// 具体操作上，与大小比较是否取等有关。如果取等，即相同元素比较时，控制二者相对位置不变，即可保证稳定性

// 时间复杂度: 最差O(n log n)，最好O(n log n)，平均O(n log n)
// 空间复杂度: O(n)，需要额外的临时数组来存储合并结果
// 稳定性: 稳定

#include <algorithm>
#include <iostream>
#include <vector>

// 辅助函数：合并两个有序子数组
// array: 原始数组
// left: 左子数组的起始索引
// mid: 左子数组的结束索引 / 右子数组的起始索引 - 1
// right: 右子数组的结束索引
void merge(std::vector<int> &array, int left, int mid, int right) {
    // mid本身算在左子数组中，mid+1是右子数组的起始位置
    int n1 = mid - left + 1;    // 左子数组的长度
    int n2 = right - mid;       // 右子数组的长度

    // 创建临时数组来存储左右子数组的元素
    std::vector<int> left_array(n1);
    std::vector<int> right_array(n2);

    // 将数据复制到临时数组
    for (int i = 0; i < n1; ++i) {
        left_array[i] = array[left + i];
    }
    for (int j = 0; j < n2; ++j) {
        right_array[j] = array[mid + 1 + j];
    }

    // 合并临时数组回原始数组的相应位置
    int i = 0;       // 左子数组的当前索引
    int j = 0;       // 右子数组的当前索引
    int k = left;    // 原始数组的当前写入索引，不一定是0！

    // 比较两个子数组的元素，将较小的放入原始数组
    while (i < n1 && j < n2) {
        if (left_array[i] <= right_array[j]) {
            array[k++] = left_array[i++];
        } else {
            array[k++] = right_array[j++];
        }
    }

    // 将左子数组中剩余的元素全部放入原始数组
    while (i < n1) {
        array[k++] = left_array[i++];
    }

    // 将右子数组中剩余的元素全部放入原始数组
    while (j < n2) {
        array[k++] = right_array[j++];
    }
}

// 这个递归过程，可以用后序遍历来模拟。自己要想象一棵递归树
// 注意这个递归函数的出口条件，对应到递归树，就是说，如果这个结点是叶子结点或空节点，直接返回
// 当left==right时，表示当前子数组只有一个元素，这在递归树上就是叶子节点，它本身就是有序的，所以直接返回。
// 当left>right时（这通常发生在偶数长度数组的分解末端，或者说是空子数组的情况），也是直接返回。

// 而后序遍历的出口条件是：空节点直接返回，但叶子结点不返回（而是继续向左右子树遍历，直到左右子树是空）
// 下面merge函数就相当于打印操作，但上面说了，在后序遍历中，叶子结点不返回，所以可以执行到自己的打印
// 但这里的递归，叶子结点需要返回，所以除了递归树的叶子结点，其他结点都能执行到merge函数
// 那么第一次执行merge函数的结点，就是最左边的父节点
void mergeSortRecursive(std::vector<int> &array, int left, int right) {
    if (left >= right) {
        return;                       // 如果子数组只有一个或没有元素，则直接返回（已经有序）
    }

    int mid = (left + right) >> 1;    // 计算中间索引，向下取整

    // 递归地对左半部分进行排序
    mergeSortRecursive(array, left, mid);
    // 递归地对右半部分进行排序
    mergeSortRecursive(array, mid + 1, right);

    // 如果以上两个递归函数都返回了，说明此时两部分是有序的（不再满足分治条件）
    // 合并已排序的左右两部分
    merge(array, left, mid, right);
}

// 归并排序的入口函数
// 为了代码健壮性，增加对空数组的判断。因此添加一个新入口辅助函数
// 这种设计模式在实际项目中很常见，在面向对象中还可以结合重载、访问控制等特性
void mergeSort(std::vector<int> &array) {
    if (array.empty()) {
        return;
    }
    mergeSortRecursive(array, 0, array.size() - 1);
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

    mergeSort(arr);

    std::cout << "排序后的数组: ";
    printArray(arr);

    std::vector<int> arr2 = {38, 27, 43, 3, 9, 82, 10};
    std::cout << "原始数组2: ";
    printArray(arr2);

    mergeSort(arr2);

    std::cout << "排序后的数组2: ";
    printArray(arr2);

    std::vector<int> arr3 = {1};
    std::cout << "原始数组3: ";
    printArray(arr3);

    mergeSort(arr3);

    std::cout << "排序后的数组3: ";
    printArray(arr3);

    std::vector<int> arr4 = {};
    std::cout << "原始数组4: ";
    printArray(arr4);

    mergeSort(arr4);

    std::cout << "排序后的数组4: ";
    printArray(arr4);

    return 0;
}
