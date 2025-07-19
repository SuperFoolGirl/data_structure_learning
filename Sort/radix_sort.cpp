// 基数排序
// 又称桶排序，是一种非比较型整数排序算法

// 这里提一个初学时用到的排序算法：
// 确定数据范围，开一个对应大小的数组
// 如数据为 3 15 3 92 14 15 14
// 则需要开一个大小为 93 的数组（0-92）
// 然后遍历数组，将每个元素放入对应的桶中。桶就是3、15、92、14
// 遍历到3，则arr[3]++，遍历到15，则arr[15]++
// 最后遍历这个桶数组，将每个桶中的元素按顺序输出即可
// 上述算法与哈希表的分离连接法类似
// 显然，如果数据范围很大，这种方法就不合适了，且只能排序非负整数。
// 而基数排序是一种更有效的解决方案

// 下面讲解基数排序
// 基数排序的思想是将整数按位分组进行排序
// 先按个位数排序，再按十位数排序，依次类推，直到最高位
// 所有的数要统一成相同的位数，不足的补前导零

// 对于十进制数，基数r为10
// 所以需要10个桶。为了节省空间，桶用链表来实现；数组的话需要预留大量空间
// 对于每个桶，先进去的元素要先拿出来，因此要使用链式队列
// 整个桶集合，就是r个链式队列组成的数组

// 第一轮链完个位后，从0到9，依次取出每个链表中的元素，把他们放到一个新链表中链起来
// 这样，所有的个位就都有序了
// 重复上述操作，排好十位。此时个位的顺序被打破了，但是，后两位整体上是有序的
// 重复上述操作直到最高位，所有的数就都有序了

#include <algorithm>    // 用于std::max, std::to_string
#include <iostream>
#include <string>
#include <vector>


// 为了方便描述桶，这里使用std::vector<std::vector<int>>来模拟桶和队列
// 理论上，为了“节省空间”和“先进先出”，使用链表实现的队列数组更符合描述
// 但实际编程中，std::vector 的性能优化和易用性使其成为一个不错的选择
// 并且对于相对较小的基数（如10），内存开销通常可接受。
// 如果数据量巨大且内存敏感，则需要自定义链表队列。

// 辅助函数：获取一个整数的指定位上的数字
// num: 要获取位数的整数
// exp: 表示当前位的权重，例如，1表示个位，10表示十位，100表示百位
int getDigit(int num, int exp) {
    // 取到exp位上的数字
    // 例如，num=1234, exp=10 (十位)，则返回3
    return (num / exp) % 10;
}

// 辅助函数：获取数组中最大值的位数，用于确定排序轮数
int getMax(const std::vector<int> &arr) {
    if (arr.empty()) {
        return 0;
    }
    int maxVal = arr[0];
    for (int i = 1; i < arr.size(); ++i) {
        if (arr[i] > maxVal) {
            maxVal = arr[i];
        }
    }
    return maxVal;
}

// 基数排序函数
void radixSort(std::vector<int> &arr) {
    if (arr.empty()) {
        return;
    }

    // 1. 找到数组中的最大值，以确定需要处理多少位
    int maxVal = getMax(arr);

    // 从个位开始，依次对每一位进行计数排序（或桶排序）
    // exp 代表当前处理的位数：1 (个位), 10 (十位), 100 (百位), ...
    for (int exp = 1; maxVal / exp > 0; exp *= 10) {
        // 创建10个桶（vector模拟队列）
        std::vector<std::vector<int>> buckets(10);    // 10个桶，每个桶是一个vector

        // 2. 将数组元素根据当前位的值放入对应的桶中
        for (int num : arr) {
            int digit = getDigit(num, exp);
            buckets[digit].push_back(num);    // 放入桶中，vector的push_back模拟队列的入队
        }

        // 3. 从0到9，依次取出每个桶中的元素，重新放回原数组
        // 这样就完成了当前位的排序
        int index = 0;
        for (int i = 0; i < 10; ++i) {      // 遍历每个桶
            for (int num : buckets[i]) {    // 遍历当前桶中的所有元素
                arr[index++] = num;         // 从桶中取出元素（vector的遍历模拟队列的出队，是先进先出的）
            }
        }
    }
}

// 辅助函数：打印数组
void printArray(const std::vector<int> &arr) {
    for (int x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

int main() {
    std::vector<int> arr1 = {170, 45, 75, 90, 802, 24, 2, 66};
    std::cout << "原始数组1: ";
    printArray(arr1);
    radixSort(arr1);
    std::cout << "排序后的数组1: ";
    printArray(arr1);

    std::cout << "---" << std::endl;

    std::vector<int> arr2 = {543, 12, 100, 321, 999, 1};
    std::cout << "原始数组2: ";
    printArray(arr2);
    radixSort(arr2);
    std::cout << "排序后的数组2: ";
    printArray(arr2);

    std::cout << "---" << std::endl;

    std::vector<int> arr3 = {12, 11, 13, 5, 6, 7};
    std::cout << "原始数组3: ";
    printArray(arr3);
    radixSort(arr3);
    std::cout << "排序后的数组3: ";
    printArray(arr3);

    return 0;
}
