// 二叉最小堆（普通堆）
// 堆是一种特殊的完全二叉树，满足堆的性质：每个节点的值都小于或等于其子节点的值（对于最大堆则相反）
// 最下面一层不一定满，但需要从左到右（完全二叉树定义）
// 堆的实现通常使用数组来存储元素，利用数组的索引关系来表示父子节点的关系，而不是依赖树结构

// 对于最小堆，层序遍历的结果，不一定是从小到大排列的！
// 层序遍历形成的数组，对于二叉树来说，有一个特点：
// 对于一个节点 i，其左子节点的索引为 2*i，右子节点的索引为 2*i + 1，父节点的索引为 i/2

// 应用1：优先队列（Priority Queue）
// 每次取根结点，然后将最后一个元素放到根结点位置，然后下沉

// 应用2：堆排序（Heap Sort）
// 对于已经满足性质的大根堆：
// 依次将根节点弹出，放入一个数组中，最后得到的数组就是有序的
// 但考虑到空间复杂度，通常会选择原地堆排序，把弹出的结点放到数组的末尾
// 这里用大根堆，堆排序后得到的数组是从大到小的顺序
// 根节点与末尾元素交换，然后下沉新根节点，直到堆的性质满足，然后重复执行，直到堆为空，序列就是有序的
// 但如果处理乱序数组，需要先把数组构建成一个堆，然后再进行排序

#include <algorithm>    // 用于 std::swap
#include <iostream>
#include <vector>

// 定义MinHeap类
class MinHeap {
private:
    std::vector<int> heap;    // 使用vector来存储堆元素

    // 上浮操作：当新元素插入或元素值减小时，维持堆的性质
    void heapifyUp(int index) {
        // 当当前节点不是根节点且小于其父节点时，进行交换
        while (index > 0 && heap[index] < heap[parent(index)]) {
            std::swap(heap[index], heap[parent(index)]);
            index = parent(index);    // 更新索引到父节点的位置
        }
    }

    // 下沉操作：当根元素被删除或元素值增大时，维持堆的性质
    void heapifyDown(int index) {
        int smallest = index;    // 假设当前节点是最小的
        int leftChild = left(index);
        int rightChild = right(index);

        // 要找到左右子节点中最小的一个与当前父节点交换，所以下面两个if是合法的

        // 如果左子节点存在且小于当前最小节点
        if (leftChild < heap.size() && heap[leftChild] < heap[smallest]) {
            smallest = leftChild;
        }

        // 如果右子节点存在且小于当前最小节点
        if (rightChild < heap.size() && heap[rightChild] < heap[smallest]) {
            smallest = rightChild;
        }

        // 如果最小节点不是当前节点，则交换并递归下沉
        if (smallest != index) {
            std::swap(heap[index], heap[smallest]);
            heapifyDown(smallest);    // 递归调用，继续下沉
        }
    }

    // 获取父节点的索引
    int parent(int i) {
        return (i - 1) / 2;
    }

    // 获取左子节点的索引
    // 注意：这里的左子节点索引是从0开始的，所以是2*i + 1
    // 如果是从1开始的完全二叉树，则是2*i
    int left(int i) {
        return (2 * i + 1);
    }

    // 获取右子节点的索引
    // 注意：这里的右子节点索引是从0开始的，所以是2*i + 2
    // 如果是从1开始的完全二叉树，则是2*i + 1
    int right(int i) {
        return (2 * i + 2);
    }

public:
    // 构造函数
    MinHeap() {}

    // 插入元素
    // 插入到vector的末尾，然后进行上浮操作。上浮比下浮更简单
    void insert(int value) {
        heap.push_back(value);         // 将新元素添加到vector末尾
        heapifyUp(heap.size() - 1);    // 对新元素进行上浮操作
    }

    // 获取堆中最小元素（即根元素）
    int peek() {
        if (isEmpty()) {
            throw std::runtime_error("Heap is empty. Cannot peek.");
        }
        return heap[0];    // 最小元素总是在根部
    }

    // 删除并返回堆中最小元素
    int extractMin() {
        if (isEmpty()) {
            throw std::runtime_error("Heap is empty. Cannot extract min.");
        }
        int min_val = heap[0];    // 保存最小元素
        // 将最后一个元素移动到根部，然后删除最后一个元素
        heap[0] = heap.back();
        heap.pop_back();
        // 如果堆不为空，对新的根元素进行下沉操作
        if (!isEmpty()) {
            heapifyDown(0);
        }
        return min_val;
    }

    // 检查堆是否为空
    bool isEmpty() {
        return heap.empty();
    }

    // 获取堆中元素的数量
    int size() {
        return heap.size();
    }

    // 打印堆的所有元素 (按内部存储顺序)
    // 常量引用才能给默认参数，普通左值引用不可以
    void printHeap(const std::string &info = "Heap elements (internal order):") {
        std::cout << info;
        for (int val : heap) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    // 堆排序
    // 升序数组一定满足最小堆，降序数组一定满足最大堆
    // 这里需要将heap数组调整为有序的，虽然会破坏堆结构，但会保证堆性质
    // 最小堆层序遍历不一定是升序数组，但升序数组一定满足最小堆
    // 不过，在排序之后，一旦执行新的插入操作等，升序性质就会破坏
    // 本来这里的堆排序就是演示性质而已，升序性质本来就是不用维护的
    void heapSort() {
        std::vector<int> sorted;
        while (!isEmpty()) {
            sorted.push_back(extractMin());    // 依次提取最小元素
        }
        heap = sorted;                         // 将排序后的结果赋值回堆
        // 打印排序后的结果
        printHeap("Sorted elements:");
    }
};

int main() {
    MinHeap minHeap;

    std::cout << "--- MinHeap 功能测试 ---" << std::endl;

    // 1. 检查初始状态
    std::cout << "Heap is empty initially: " << (minHeap.isEmpty() ? "Yes" : "No") << std::endl;
    std::cout << "Heap size initially: " << minHeap.size() << std::endl;

    // 2. 插入元素
    std::cout << "\nInserting elements: 10, 5, 20, 2, 8, 15" << std::endl;
    minHeap.insert(10);
    minHeap.insert(5);
    minHeap.insert(20);
    minHeap.insert(2);
    minHeap.insert(8);
    minHeap.insert(15);
    minHeap.printHeap();    // 打印内部存储顺序
    std::cout << "Heap size after insertions: " << minHeap.size() << std::endl;
    std::cout << "Min element after insertions (peek): " << minHeap.peek() << std::endl;

    // 执行堆排序
    minHeap.heapSort();

    // 3. 连续提取最小元素
    std::cout << "\nExtracting min elements:" << std::endl;
    while (!minHeap.isEmpty()) {
        int min_val = minHeap.extractMin();
        std::cout << "Extracted: " << min_val << std::endl;
        if (!minHeap.isEmpty()) {
            std::cout << "Current min element: " << minHeap.peek() << std::endl;
        }
        minHeap.printHeap();
        std::cout << std::endl;
    }

    // 4. 再次检查空堆状态
    std::cout << "\nHeap is empty after extraction: " << (minHeap.isEmpty() ? "Yes" : "No") << std::endl;
    std::cout << "Heap size after extraction: " << minHeap.size() << std::endl;

    // 5. 测试空堆时的异常处理
    try {
        minHeap.peek();
    } catch (const std::runtime_error &e) {
        std::cerr << "Caught exception when peeking empty heap: " << e.what() << std::endl;
    }

    try {
        minHeap.extractMin();
    } catch (const std::runtime_error &e) {
        std::cerr << "Caught exception when extracting from empty heap: " << e.what() << std::endl;
    }

    // 6. 插入更多元素以进行更多测试
    std::cout << "\nInserting more elements: 7, 3, 12, 1" << std::endl;
    minHeap.insert(7);
    minHeap.insert(3);
    minHeap.insert(12);
    minHeap.insert(1);
    minHeap.printHeap();
    std::cout << "Current min element: " << minHeap.peek() << std::endl;

    minHeap.extractMin();    // 提取 1
    std::cout << "After extracting one element: " << std::endl;
    minHeap.printHeap();
    std::cout << "Current min element: " << minHeap.peek() << std::endl;

    return 0;
}
