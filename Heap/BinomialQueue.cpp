// 最小二项堆（二项队列）
// 二项队列是一种森林，由一组二项树组成

// 二项树定义：
// 二项树是一个递归定义的树结构，满足以下条件：
// B_0 (0阶二项树)：由一个单独的节点组成。
// B_k (k阶二项树)：由两个 k-1 阶二项树组成，其中一个作为另一个的左子树，且左子树的高度比右子树高1。
// 因此，二项树结点个数必须为2的平方，这一点与下面的性质2是吻合的
// 对于二项树结点，用两个指针来维护：first_child（指向左子树）和 sibling（指向右兄弟）

// 性质如下：
// 1. 每棵二项树都是最小堆性质
// 2. 一个二项队列中，至多只包含一棵高度为k的二项树。这个性质是支持合并操作的基础
//    这个性质让二项队列的结构非常类似于一个整数的二进制表示
//    例如，一个包含13个元素的二项队列，13 的二进制是 1101_2。
//    这意味着它包含三棵树：2^3 + 2^2 + 2^0 = 8 + 4 + 1 = 13

// 关于二项树的结构与二项式定理的关系，在deleteMin方法中有讲解

#include <algorithm>    // For std::max
#include <iostream>
#include <stdexcept>    // For std::underflow_error
#include <vector>

template<class Comparable>
class BinomialQueue {
private:
    // --- 内部 BinomialNode 结构体 ---
    // 定义二项树的节点结构。
    struct BinomialNode {
        Comparable element;           // 节点存储的元素值
        BinomialNode *leftChild;      // 指向最左子节点的指针
        BinomialNode *nextSibling;    // 指向下一个兄弟节点的指针（向右）

        // 节点构造函数
        BinomialNode(const Comparable &theElement, BinomialNode *lc = nullptr, BinomialNode *ns = nullptr)
            : element(theElement), leftChild(lc), nextSibling(ns) {}
    };

    // --- 私有成员变量 ---
    // 向量的索引 'i' 对应二项树的阶数（order）或高度（height）。
    // theTrees[i] 存储的是一棵阶数为 'i' 的二项树的根节点。
    // 如果二项队列中不存在某一阶数的树，对应的位置将是 nullptr。
    // 根据二项队列的独特性质，每个阶数至多只有一棵二项树。
    int currentSize;                         // 记录整个二项队列中所有二项树的节点总数，并不是二项树的数目
                                             // 给定currentSize，将其转换为二进制数，就可以判断出具体包含哪些二项树了
    std::vector<BinomialNode *> theTrees;    // 存储所有二项树的根节点
    enum class TreeNum {
        DEFAULT_TREES = 1
    };    // 默认的二项树数量

    // --- 私有辅助方法 ---

    // 查找包含最小元素的二项树的索引（即阶数/高度）。
    // 该函数会遍历所有存在的二项树的根节点，找出其中值最小的那个根节点所属的树的索引。
    // 前提：此函数不会在队列为空时被调用，因为 findMin() 和 deleteMin() 会在外部进行检查。
    // 每棵二项树的根节点都满足最小堆性质，因此可以直接比较根节点的值。
    int findMinTreeIndex() const {
        if (isEmpty()) {
            // 作为内部辅助函数，如果这里被调用说明外部调用逻辑有误，通常抛出逻辑错误而非运行时错误。
            throw std::logic_error("findMinTreeIndex called on an empty queue.");
        }

        int minTreeIdx = 0;
        // 找到第一个非空（即实际存在的）二项树的索引，作为初始的最小树索引。
        // 这是为了处理 theTrees 向量开头可能存在多个 nullptr 的情况（例如，队列只有 B3 树而没有 B0, B1, B2）。
        while (minTreeIdx < theTrees.size() && theTrees[minTreeIdx] == nullptr) {
            minTreeIdx++;
        }

        // 如果循环结束后 minTreeIdx 仍然等于 theTrees.size()，说明没有找到任何树，
        // 这意味着队列实际上是空的，但前面的 isEmpty() 检查应该已经捕获了这种情况。
        if (minTreeIdx == theTrees.size()) {
            throw std::logic_error("No trees found in a non-empty queue (internal error).");
        }

        // 遍历剩余的树，找到根节点值最小的树的索引。
        for (int i = minTreeIdx + 1; i < theTrees.size(); ++i) {
            if (theTrees[i] != nullptr && theTrees[i]->element < theTrees[minTreeIdx]->element) {
                minTreeIdx = i;
            }
        }
        return minTreeIdx;
    }

    // 计算二项队列所需的基础向量容量。
    // 一个包含 N 个元素的二项队列，其最高阶数约为 log2(N)。
    // 因此，theTrees 向量的大小需要能够容纳从 0 到 log2(N) 的所有阶数。
    int calculateMinRequiredCapacity() const {
        if (currentSize == 0) {
            return 1;    // 队列为空时，至少需要1个槽位（例如，为 B0）
        }
        int capacity = 0;
        int tempSize = currentSize;
        // 通过不断右移（除以2）来模拟 log2 操作，统计所需的最大阶数。
        // 这里计算得到的是最高位的二项树。例如10/2 = 5, 5/2 = 2, 2/2 = 1, 1/2 = 0，最高位是3
        // 而10的二进制是1010，表示有B3和B1两棵树，所以需要4个槽位
        while (tempSize > 0) {
            tempSize /= 2;
            capacity++;
        }
        return capacity;
    }

    // 合并两棵阶数相同的二项树 t1 和 t2，生成一棵阶数加 1 的新二项树。
    // 该函数会遵循最小堆性质：根节点值较小的树将成为新树的根，另一棵树则成为其最左子节点。
    // 返回新合并树的根节点。
    BinomialNode *combineTrees(BinomialNode *t1, BinomialNode *t2) {
        // 确保 t1 始终指向根节点值较小的树，它将成为合并后新树的根。
        if (t2->element < t1->element) {
            return combineTrees(t2, t1);    // 交换 t1 和 t2 的位置，递归调用确保 t1 较小
        }
        // 将 t2 (根节点值较大的树) 成为 t1 的新最左子节点。
        // t1 原来的最左子节点 (t1->leftChild) 现在成为了 t2 的下一个兄弟节点。
        // t2由于本来是一棵树的根节点，是没有兄弟的。所以这里不会产生冲突
        t2->nextSibling = t1->leftChild;
        t1->leftChild = t2;
        return t1;    // 返回新合并树的根节点
    }

    // 递归地清空（删除所有节点）一棵二项树，熟悉的后序遍历
    // 这是深层清理，用于析构函数和 makeEmpty() 方法。
    void makeEmpty(BinomialNode *&t) {
        if (t != nullptr) {
            makeEmpty(t->leftChild);      // 递归清空当前节点的左子树（即更高阶数的子树）
            makeEmpty(t->nextSibling);    // 递归清空当前节点的兄弟子树（即同层级的其他树）
            delete t;                     // 删除当前节点
            t = nullptr;                  // 将指针置空，避免悬空指针
        }
    }

    // 递归地克隆一棵二项树。
    // 用于拷贝构造函数和赋值运算符，实现深拷贝。
    // 根据递归树，只有空节点会进入递归出口，叶子结点可以顺利执行clone
    // 实际上，这是一个后序遍历的模型。也很符合直觉，必须现有孩子才能有父母
    BinomialNode *clone(BinomialNode *t) const {
        if (t == nullptr) {
            return nullptr;
        }
        // 创建一个新节点，并递归地克隆其子节点和兄弟节点。
        return new BinomialNode(t->element, clone(t->leftChild), clone(t->nextSibling));
    }

public:
    // --- 构造函数与析构函数 ---

    // 默认构造函数：创建一个空的二项队列。
    BinomialQueue() : currentSize(0) {
        // 预留一些初始空间，避免频繁的向量重新分配，但 `currentSize` 仍然是 0。
        // DEFAULT_TREES 设定为 1 或 2 通常足够。
        theTrees.reserve(static_cast<size_t>(TreeNum::DEFAULT_TREES));
    }

    // 带初始元素的构造函数：创建一个包含一个元素的二项队列。
    // 内部调用 insert 方法，因为它能正确处理队列的合并和结构维护。
    explicit BinomialQueue(const Comparable &item) : currentSize(0) {
        insert(item);
    }

    // 拷贝构造函数：执行深拷贝，复制另一个二项队列的所有树结构。
    BinomialQueue(const BinomialQueue &rhs) : currentSize(0) {
        // 利用拷贝赋值运算符来执行深拷贝，避免代码重复。
        *this = rhs;
    }

    // 析构函数：释放所有动态分配的节点内存。
    ~BinomialQueue() {
        makeEmpty();    // 调用公共的 makeEmpty 方法来清空所有树
    }

    // --- 公共接口方法 ---

    // 检查二项队列是否为空。
    bool isEmpty() const {
        return currentSize == 0;
    }

    // 返回二项队列中的最小元素。
    // 如果队列为空，则抛出 std::underflow_error 异常。
    const Comparable &findMin() const {
        if (isEmpty()) {
            throw std::underflow_error("BinomialQueue is empty, cannot find minimum.");
        }
        // 找到包含最小元素的树的根节点，并返回其元素。
        int minTreeIdx = findMinTreeIndex();
        return theTrees[minTreeIdx]->element;
    }

    // 向二项队列中插入一个新元素。
    // 实现方式是创建一个只包含新元素的临时二项队列（实际上是一棵 B0 树），
    // 然后将其与当前队列合并。
    // 注意合并方法不是把二项树合并到二项队列，而是把二项队列合并到二项队列
    void insert(const Comparable &x) {
        // 创建一个临时二项队列，只包含新插入的元素（作为一棵 B0 树）。
        BinomialQueue<Comparable> singleNodeQueue;
        singleNodeQueue.theTrees.resize(1, nullptr);    // B0 树位于索引 0
        singleNodeQueue.theTrees[0] = new BinomialNode(x);
        singleNodeQueue.currentSize = 1;

        // 将这个包含单个元素的队列与当前队列合并。
        merge(singleNodeQueue);
    }

    // 从二项队列中删除最小元素。
    // 如果队列为空，则抛出 std::underflow_error 异常。
    void deleteMin() {
        if (isEmpty()) {
            throw std::underflow_error("BinomialQueue is empty, cannot delete minimum.");
        }

        // 1. 找到包含最小元素的二项树的索引 (阶数)。
        int minTreeIdx = findMinTreeIndex();

        // 2. 准备删除这棵树的根节点。
        // 获取被删除树的根节点指针，以及其最左子节点的指针（这将是构成临时队列的起始树）。
        BinomialNode *oldRootNode = theTrees[minTreeIdx];
        BinomialNode *childrenHead = oldRootNode->leftChild;    // 被删除根节点的所有子树的链表头

        // 3. 将原队列中对应最小元素所在树的位置清空（将其移除）。
        // 不置空的话，后面delete掉后，这里要成野指针了，不允许二项队列中有野指针
        theTrees[minTreeIdx] = nullptr;
        // 更新原队列的元素总数：减去被删除的这棵二项树所包含的节点数量 (2^minTreeIdx)。
        // 之所以把一家老小全都删了，是因为子树们要自己搞一个新的二项队列，所以直接跟原来的saygoodbye了
        currentSize -= (1 << minTreeIdx);

        // 4. 创建一个临时二项队列，用于存放被删除根节点的所有子树。
        BinomialQueue<Comparable> tempQueueForChildren;
        // 调整临时队列的 theTrees 向量大小，使其能够容纳所有可能阶数的子树。
        // 一棵 B_k 树的子树阶数从 B_{k-1} 到 B_0，共 k 棵树。因此需要 k 个槽位（索引 0 到 k-1）。
        tempQueueForChildren.theTrees.resize(minTreeIdx);    // 如果 minTreeIdx 是 0 (B0树), 则 resize(0)
        tempQueueForChildren.currentSize = 0;                // 临时队列的元素数量将在后续填充时更新

        // 5. 遍历被删除根节点的所有子树（它们通过 nextSibling 连接），
        // 并将它们作为独立的二项树放入临时队列中。
        // 子树的阶数从高到低（从 minTreeIdx - 1 递减到 0）。
        for (int i = minTreeIdx - 1; i >= 0; --i) {
            // 这里可以举个例子
            // 比如要删除的是3阶二项树的根节点
            // 本来有8个节点，删掉一个后，变7个节点了。写成二进制是0111，因此新的森林要负责描绘这个结构
            // 由于1000-1=0111，所以从minTreeIdx-1开始，每个位置都有二项树，这是必然的
            // 当前子树的根节点是 childrenHead 指向的节点。

            // 对于二项树的结构，这里也补充以下
            // 对于3阶二项树，其根节点共有3个儿子，树的高度为3+1=4
            // 从上往下，各层节点数目为1331，相加为2^3（恭喜，成功与二项式定义联系起来了）
            // 当根节点消失后，这三个儿子只要彼此断开链接，就能成为独立的012阶二项树（画图验证）
            tempQueueForChildren.theTrees[i] = childrenHead;
            // 移动 childrenHead 到下一个兄弟节点，为下一轮循环做准备。
            BinomialNode *nextChild = childrenHead->nextSibling;
            // 断开当前子树与下一个兄弟子树之间的链接，使其成为临时队列中的一棵独立二项树的根。
            tempQueueForChildren.theTrees[i]->nextSibling = nullptr;
            childrenHead = nextChild;    // 更新 childrenHead

            // 更新临时队列的元素总数：加上当前子树的节点数量 (2^i)。
            // 7 = 2^3 - 1 = 2^2 + 2^1 + 2^0
            tempQueueForChildren.currentSize += (1 << i);
        }

        // 6. 删除原队列中的根节点本身（释放其内存）。
        delete oldRootNode;

        // 7. 将包含所有子树的临时队列与原队列合并。
        // 这样，被删除根节点的子树就被重新整合到队列中，并维护了二项队列的性质。
        merge(tempQueueForChildren);
    }

    // deleteMin 的重载版本：删除最小元素并将其值通过引用参数返回。
    // 内部调用无参的 deleteMin() 来执行删除逻辑，并提前获取最小值。
    void deleteMin(Comparable &minItem) {
        if (isEmpty()) {
            throw std::underflow_error("BinomialQueue is empty, cannot delete minimum.");
        }

        // 1. 提前获取最小元素的值。
        minItem = findMin();    // 调用 findMin() 来获取最小值，这也会检查空队列情况。

        // 2. 执行实际的删除逻辑，与无参 deleteMin() 相同。
        // 这里可以直接调用无参的 deleteMin() 来避免代码重复。
        // 或者，如果你选择复制逻辑，那需要确保逻辑完全一致。
        deleteMin();    // 调用上面已实现的无参 deleteMin
    }

    // 清空整个二项队列，释放所有节点内存。
    void makeEmpty() {
        // 遍历 theTrees 向量中的每一棵二项树，并调用辅助函数 makeEmpty() 递归清空它们。
        // 有参的 makeEmpty() 是用来清空所有二叉树的，二项队列是二叉树的集合
        for (int i = 0; i < theTrees.size(); ++i) {
            makeEmpty(theTrees[i]);    // theTrees[i] 传入的是指针的引用
        }
        currentSize = 0;               // 重置队列的总元素数量
    }

    // 将当前二项队列与另一个二项队列 (rhs) 合并。
    // 合并后，rhs 队列将变为空。
    void merge(BinomialQueue &rhs) {
        // 防止自合并：如果尝试将队列与自身合并，直接返回，不做任何操作。
        if (this == &rhs) {
            return;
        }

        // 核心合并逻辑，类似于二进制加法：
        // 抓住二进制加法的逻辑就很简单了，从低位到高位逐位相加
        // `carry` 指针用于暂存合并过程中产生的进位树（当同一阶数有两棵树合并成一棵更高阶数的树时）。
        BinomialNode *carry = nullptr;

        // 更新当前队列的总元素数量。
        currentSize += rhs.currentSize;

        // 调整当前队列的 theTrees 向量大小，使其能够容纳合并后的所有可能阶数的树。
        // 新的容量是两个队列中最大阶数 + 1（因为可能出现进位到新的最高阶数）。
        // 这个道理学高精度加法的时候就知道了，不多赘述。加法的极限就是进一位
        // 使用 calculateMinRequiredCapacity(currentSize) 也可以，但这里直接用 max 即可。
        int newCapacity = std::max(theTrees.size(), rhs.theTrees.size()) + 1;
        // resize 会填充新元素为默认值 (nullptr)，旧元素保持不变。
        theTrees.resize(newCapacity, nullptr);

        // 遍历所有可能的树阶数，从阶数 0 开始。
        // 循环终止条件通常是遍历完所有可能存在的最高阶数，或者当 currentSize 不再需要更高的阶数时。
        // 这里使用 theTrees.size() 是安全的，因为上面已经 resize 过。
        for (int i = 0; i < theTrees.size(); ++i) {
            // 开始每一位相加（对于可能产生进位的最高位，原二项队列已经扩容了）

            // 获取当前阶数 'i' 的三棵潜在树：
            // t1: 来自当前队列 (this)
            // t2: 来自 rhs 队列
            // carry: 来自上一阶数合并的进位
            BinomialNode *t1 = theTrees[i];
            // 待合并二项队列没扩容，有可能越界访问
            BinomialNode *t2 = (i < rhs.theTrees.size()) ? rhs.theTrees[i] : nullptr;

            // 根据 t1, t2, carry 的存在与否，判断是哪种合并情况（共 8 种）。
            // 8种由来：高中概率，2^3 = 8
            // 这种位模式编码是一种简洁的方式来表示所有组合：
            // 0为不存在，1/2/4为存在
            int whichCase = (t1 == nullptr ? 0 : 1);
            whichCase += (t2 == nullptr ? 0 : 2);
            whichCase += (carry == nullptr ? 0 : 4);

            // 使用 switch-case 语句处理这 8 种情况，模拟二进制加法逻辑。
            switch (whichCase) {
            // 情况 0: 三者都不存在。结果：0，进位：0。
            case 0:

            // 情况 1: 只有 t1 存在。结果：t1，进位：0。 (t1 保持在 theTrees[i])
            case 1:
                // theTrees[i] 已经包含 t1 (或 nullptr)，carry 已经是 nullptr，无需操作。
                break;

            // 情况 2: 只有 t2 存在。结果：t2，进位：0。
            case 2:
                theTrees[i] = t2;             // 将 t2 移动到当前队列的位置
                rhs.theTrees[i] = nullptr;    // 清空 rhs 队列的对应位置
                break;

            // 情况 4: 只有 carry 存在。结果：carry，进位：0。
            case 4:
                theTrees[i] = carry;    // 将 carry 移动到当前队列的位置
                carry = nullptr;        // 清空 carry，因为它已被放置
                break;

            // 情况 3: t1 和 t2 存在。结果：0，进位：t1 和 t2 合并后的树。
            // 在相同的阶数上有两棵树，1 + 1 + 0 = 10
            case 3:
                carry = combineTrees(t1, t2);    // 合并 t1 和 t2，结果成为进位
                theTrees[i] = nullptr;           // 当前位置清空
                rhs.theTrees[i] = nullptr;       // rhs 对应位置清空
                break;

            // 情况 5: t1 和 carry 存在。结果：0，进位：t1 和 carry 合并后的树。
            // 1 + 0 + 1 = 10
            case 5:
                carry = combineTrees(t1, carry);    // 合并 t1 和 carry，结果成为进位
                theTrees[i] = nullptr;              // 当前位置清空
                break;

            // 情况 6: t2 和 carry 存在。结果：0，进位：t2 和 carry 合并后的树。
            // 0 + 1 + 1 = 10
            case 6:
                carry = combineTrees(t2, carry);    // 合并 t2 和 carry，结果成为进位
                rhs.theTrees[i] = nullptr;          // rhs 对应位置清空
                break;

            // 情况 7: t1, t2 和 carry 都存在。结果：carry（当前位置放置 carry），进位：t1 和 t2 合并后的树。
            // 1 + 1 + 1 = 11
            case 7:
                theTrees[i] = carry;             // 将 carry 放置在当前位置
                carry = combineTrees(t1, t2);    // 合并 t1 和 t2，结果成为新的进位
                rhs.theTrees[i] = nullptr;       // rhs 对应位置清空
                break;
            }
        }

        // 下面这段代码是鲁棒性的体现，可以不要
        for (int k = 0; k < rhs.theTrees.size(); ++k) {
            rhs.theTrees[k] = nullptr;    // 将 rhs 队列的树指针置空
        }
        rhs.currentSize = 0;              // 重置 rhs 队列的元素数量
    }

    // 拷贝赋值运算符：实现深拷贝，将 rhs 的内容复制到当前对象。
    const BinomialQueue &operator=(const BinomialQueue &rhs) {
        if (this != &rhs) {    // 检查自赋值，防止自我拷贝导致内存问题
            makeEmpty();       // 先清空当前对象的所有内容

            // 复制 currentSize
            currentSize = rhs.currentSize;

            // 调整 theTrees 向量的大小，并进行深拷贝
            theTrees.resize(rhs.theTrees.size());    // 调整大小以匹配源队列
            for (int i = 0; i < theTrees.size(); ++i) {
                // 递归克隆每一棵二项树
                // clone函数是用来深拷贝二项树的
                // 如果这个位置没有二项树，clone函数会返回nullptr的
                theTrees[i] = clone(rhs.theTrees[i]);
            }
        }
        return *this;    // 返回当前对象的引用，支持链式赋值
    }
};

// --- 主函数 (示例用法) ---
int main() {
    BinomialQueue<int> bq1;
    std::cout << "bq1 is empty: " << (bq1.isEmpty() ? "true" : "false") << std::endl;

    std::cout << "Inserting 10, 20, 5, 30, 15, 2, 7 to bq1..." << std::endl;
    bq1.insert(10);
    bq1.insert(20);
    bq1.insert(5);
    bq1.insert(30);
    bq1.insert(15);
    bq1.insert(2);
    bq1.insert(7);

    std::cout << "bq1 is empty: " << (bq1.isEmpty() ? "true" : "false") << std::endl;
    std::cout << "bq1 min element: " << bq1.findMin() << std::endl;    // Should be 2

    BinomialQueue<int> bq2;
    std::cout << "\nInserting 4, 12, 1, 8 to bq2..." << std::endl;
    bq2.insert(4);
    bq2.insert(12);
    bq2.insert(1);
    bq2.insert(8);

    std::cout << "bq2 min element: " << bq2.findMin() << std::endl;    // Should be 1

    std::cout << "\nMerging bq2 into bq1..." << std::endl;
    bq1.merge(bq2);

    std::cout << "After merge: bq1 min element: " << bq1.findMin() << std::endl;    // Should be 1
    std::cout << "bq2 is empty after merge: " << (bq2.isEmpty() ? "true" : "false") << std::endl;

    std::cout << "\nDeleting min from bq1 (which is 1)..." << std::endl;
    int deletedMinVal;
    bq1.deleteMin(deletedMinVal);
    std::cout << "Deleted min value: " << deletedMinVal << std::endl;
    std::cout << "bq1 new min element: " << bq1.findMin() << std::endl;    // Should be 2

    std::cout << "\nInserting 3 to bq1..." << std::endl;
    bq1.insert(3);
    std::cout << "bq1 new min element: " << bq1.findMin() << std::endl;    // Should be 3

    std::cout << "\nMaking bq1 empty..." << std::endl;
    bq1.makeEmpty();
    std::cout << "bq1 is empty: " << (bq1.isEmpty() ? "true" : "false") << std::endl;

    // Test with more inserts and deletes to ensure stability
    std::cout << "\nTesting with multiple inserts and deletes:" << std::endl;
    for (int i = 0; i < 20; ++i) {
        bq1.insert(i * 3 % 20 + 1);    // Insert some values
    }
    std::cout << "bq1 min after multiple inserts: " << bq1.findMin() << std::endl;

    for (int i = 0; i < 10; ++i) {
        int val;
        bq1.deleteMin(val);
        std::cout << "Deleted " << val << ", new min: " << (bq1.isEmpty() ? "N/A" : std::to_string(bq1.findMin())) << std::endl;
    }

    return 0;
}
