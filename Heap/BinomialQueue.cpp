// 最小二项堆（二项队列）

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

template<class Comparable>
class BinomialQueue {
public:
    BinomialQueue() {}

    BinomialQueue(const Comparable &item) {}

    BinomialQueue(const BinomialQueue &rhs) {}

    ~BinomialQueue() {}

    bool isEmpty() const {}

    const Comparable &findMin() const {}

    void insert(const Comparable &x) {}

    void deleteMin() {}

    void deleteMin(const Comparable &minItem) {
        if (isEmpty()) {
            throw std::underflow_error("Underflow error occurred");
        }
        int minIndex = findMinIndex();
        minItem = theTrees[minIndex]->element;

        // 后续要利用左子树重构一个临时队列（构建一个临时队列存放被删除树的子树），然后删除根节点以移除最小元素。
        // 重构临时队列的目的应该是为了合并，合并的操作对象必须是两个BQ类
        // 函数的作用是删除最小节点 但只改变一棵树可不行 我们得维护二项队列的结构 就像树的插入一样 整体可能都要随之改变
        BinomialNode *oldRoot = theTrees[minIndex];
        BinomialNode *deletedTree = oldRoot->leftChild;
        delete oldRoot;

        // 创建一个BinomialQueue对象deletedQueue，把从原树分离出来的子树按高度依次放入这个临时队列。先调整theTrees向量大小，再通过循环把子树逐个添加进去，同时断开它们之间原有的兄弟链接。
        BinomialQueue deletedQueue;

        // minIndex是包含最小元素的那棵二项树的高度。我们要把从这棵被删除树拆解下来的所有子树，重新组织到一个临时队列deletedQueue
        // 中。这些子树的高度范围是从0到minIndex -
        // 1，一共minIndex个不同高度的子树。为了能把所有这些不同高度的子树都恰当地存放到deletedQueue的theTrees向量里，向量的大小就需要调整为能容纳这些高度的树，所以大小设为minIndex
        // + 1 注意 是要把这个树拆了
        // 塞到一个新队列。举个例子，minIndex是3（8个节点），就得把8-1=7拆成1+2+4（高度为3的待删二项树有三棵子树，结点数分别为4，2，1；再加上根节点回到8个）
        deletedQueue.theTrees.resize(minIndex + 1);    // 实参可以改 保证能放下所有子树就行

        // 高度为n的二项树出去根节点，包含2^n - 1个元素，计算出子树总共包含的元素数量，赋值给deletedQueue的currentSize
        deletedQueue.currentSize = (1 << minIndex) - 1;

        // 子树：对于任一结点，其子树就是它下面所有节点形成的树。例如在二叉树里，根节点有左、右子节点，那么分别以左、右子节点为根的树，就是根节点的两棵子树。谈论子树时，不包括本结点
        // 兄弟子树：当一个节点有多个子节点时，这些子节点各自作为根节点所形成的子树之间互为 “兄弟子树”。它们拥有共同的父节点，在树的层次结构里处于同一层级
        // 回忆一下同高度的二项树是如何合并的。t1根节点小于t2,那么整个t2就会作为t1的左子树，插入进去。这样一来，每个二项树根节点的左右子树其实是不等的，左比右高1
        // 二项树只能以1，2，4，8...的形式存在，否则就要合并。当二项队列复杂起来时，根节点的兄弟子树就有很多了。比如13=8+4+1，二项队列中有三棵树，其中8有三棵子树（4+2+1）+1，括号里是三棵子树的节点数，再加1根节点，得到一共八个节点。二项树的子树数目是对数关系，子树之间是横向的关系，也就是兄弟子树
        // 如果认为单节点树高度为0，那么对于二项树来说，高度为n <==>
        // 节点个数为2^n，n棵子树，每个子树分别引领着高度为n-1到0的树（最左边的最高n-1，最右边的兄弟子树高度为0）
        for (int i = minIndex - 1; i >= 0; i--)    // 开始处理根节点的子树 最左边的子树高度比父亲小1 从这开始。i其实正好对应了所有子树的高度
        {
            // 这里deletedTree最初指向最高的兄弟子树，后续通过nextSibling指针遍历其他兄弟子树，并依次将它们放入临时队列，同时切断它们之间原有的兄弟连接。
            deletedQueue.theTrees[i] = deletedTree;
            deletedTree = deletedTree->nextSibling;    // 横向找下一棵子树
            deletedQueue.theTrees[i]->nextSibling =
                nullptr;                               // 切断兄弟关系。注意
                            // 我们是在把一棵树的子树，拆成若干完整的树。子树之间是有nextSibling指针相连的，我们把子树拆成若干完整树后，他们各自都成为了新的根节点。根节点没有兄弟，所以要切断。dQ虽然是新实例化的对象，但已经被赋值为dT了，所以自然继承了dT的属性
        }

        theTrees[minIndex] = nullptr;    // 将原队列中对应最小元素所在树的位置清空
        currentSize -= deletedQueue.currentSize +
                       1;    // 更新原队列的元素数量，减去临时队列中的元素数以及被删除的最小元素这一个。curentSize统计的是整个二项队列的节点，即13=8+4+1

        // 调用merge函数，将临时队列deletedQueue与原队列合并，使得原队列在删除最小元素后，依然保持二项队列的结构和性质
        merge(deletedQueue);
    }

    void makeEmpty() {}

    void merge(BinomialQueue &rhs) {
        if (this == &rhs) {
            return;
        }
        currentSize += rhs.currentSize;
        if (currentSize > capacity()) {
            int oldNumTrees = theTrees.size();
            int newNumTrees = max(theTrees.size(), rhs.theTrees.size()) + 1;    // 合并两棵树的新节点数是两棵树的较大值+1 log
            theTrees.resize(newNumTrees);
            for (int i = oldNumTrees; i < newNumTrees; i++) {
                theTrees[i] = nullptr;                                          // 扩容部分置空 由于不是开辟了新空间 原来的值不变
            }
        }
        BinomialNode *carry = nullptr;                           // 暂存合并过程中产生的额外树（当同一高度有三棵树时），即进位节点（carry习惯用于表示进位变量）
        for (int i = 0, j = 1; j <= currentSize; i++, j *= 2)    // 外层for循环遍历所有可能的树高度，从高度 0 开始，每次高度翻倍（j *=
                                                                 // 2），直到覆盖所有可能包含元素的高度。i作为索引对应theTrees向量的位置。因此
                                                                 // 每次操作都取高度相同的树（t2存在该高度的树的话），因此可以调用combineTrees函数来处理
        {
            // 每次循环获取当前高度的两棵树t1（当前队列的树）和t2（rhs队列的树，如果存在的话）
            BinomialNode *t1 = theTrees[i];
            BinomialNode *t2 = i < rhs.theTrees.size() ? rhs.theTrees[i] : nullptr;

            // 通过位运算组合出 8 种可能的情况（因为涉及 3 棵树：t1、t2、carry，每种树有存在或不存在两种状态，共8种组合）
            // 位模式：分别占用1，2，4
            // 其实下面的过程就是在做二进制加法竖式计算
            int whichCase = (t1 == nullptr) ? 0 : 1;
            whichCase += (t2 == nullptr) ? 0 : 2;
            whichCase += (carry == nullptr) ? 0 : 4;

            switch (whichCase) {
            // 情况 0 和 1：没有树或者只有t1树，无需操作，直接跳过
            case 0:
            case 1:
                break;

            // 情况 2：只有t2树，将t2移到当前队列的对应位置，清空rhs队列的对应位置
            case 2:
                theTrees[i] = t2;
                rhs.theTrees[i] = nullptr;
                break;

            // 情况 4：只有carry树，将carry移到当前队列的对应位置，清空carry。进位是上一次循环产生的
            case 4:
                theTrees[i] = carry;
                carry = nullptr;
                break;

            // 情况 3：有t1和t2树，合并它们，结果存到carry，清空当前队列和rhs队列的对应位置。
            case 3:
                carry = combineTrees(t1, t2);    // 合并了就一定按进位算，比如两个4节点树合成一个8，且上一次没有进位，那竖式落下来0，然后进1
                theTrees[i] = rhs.theTrees[i] = nullptr;
                break;

            // 其他情况：类似情况 3，处理涉及carry的不同组合，确保合并后队列结构正确。
            case 5:
                carry = combineTrees(t1, carry);
                theTrees[i] = nullptr;
                break;
            case 6:
                carry = combineTrees(t2, carry);
                rhs.theTrees[i] = nullptr;
                break;
            // 1+1+1，落1进1 把carry落下来，t1和t2合并后进上去。改顺序的话可能发生一些意想不到的情况 而且这样写很顺滑
            case 7:
                theTrees[i] = carry;
                carry = combineTrees(t1, t2);
                rhs.theTrees[i] = nullptr;
                break;
            }
        }
        // 最后，遍历rhs队列的所有树，将它们设置为nullptr，并把rhs队列的currentSize设为 0，完成合并操作，此时rhs队列变为空队列。
        /*
        从逻辑上来说，后续遍历rhs.theTrees并将其元素置空、把rhs.currentSize设为0的操作，看起来有些冗余。不过，额外的这一步操作也有它的好处：
        代码健壮性：确保即使后续对合并逻辑有修改，导致按位合并循环没能完全清空rhs队列，这段代码依然能保证rhs队列被彻底清空，避免潜在的悬空指针等错误。
        清晰语义：明确表达了 “合并完成后，rhs队列应该为空” 这一语义，让阅读代码的人能更直观地理解意图，无需仔细推导按位合并循环的所有细节来确认rhs队列最终状态。
        */
        for (int k = 0; k < rhs.theTrees.size(); k++) {
            rhs.theTrees[k] = nullptr;
        }
        rhs.currentSize = 0;
    }

    const BinomialQueue &operator=(const BinomialQueue &rhs) {}

private:
    struct BinomialNode {
        Comparable element;
        BinomialNode *leftChild;
        BinomialNode *nextSibling;

        BinomialNode(const Comparable &theElement, BinomialNode *l, BinomialNode *r) : element(theElement), leftChild(l), nextSibling(r) {}
    };

    enum {
        DEFAULT_TREES = 1
    };

    int currentSize;                    // 记录二项队列中所有二项树的节点总数
    vector<BinomialNode *> theTrees;    // 存储所有二项树的根节点，数组索引可以对应二项树的高度，通常，索引为i的位置存放的是高度为i的二项树的根节点。注意
                                        // 二项队列里不可能存在多棵高度相同的树，原因就是每个数子对应的二进制表示有且仅有一个。13个节点用二项树表示应该是8，4，1
                                        // 而不可能是4，4，4，1。每当出现相同高度的树，都会即时合并它们

    int findMinIndex() const {
        // 这个函数找的是拥有最小根节点的二项树的索引（高度），而不是最小高度的树
        int i;
        int minIndex;
        // 第一个for循环跳过theTrees向量开头的所有空指针位置。因为二项队列在维护过程中，可能有些高度的树不存在，对应位置就是空指针，这个循环找到第一个实际存在树的位置
        for (i = 0; theTrees[i] == nullptr; i++) {
            ;
        }
        for (minIndex = i; i < theTrees.size(); i++)    // 注意初始化语句 先假设第一个非空指针是最小的 如果它就是最小的 那直接返回它 类似oj上初始化ans=-1
        {
            if (theTrees[i] != nullptr && theTrees[i]->element < theTrees[minIndex]->element) {
                minIndex = i;
            }
        }
        return minIndex;
    }

    int capacity() const {}

    // 合并两棵高度相同的树
    BinomialNode *combineTrees(BinomialNode *t1, BinomialNode *t2) {
        if (t2->element < t1->element) {    // 统一让t1形参为较小节点，即新的根节点
            return combineTrees(t2, t1);
        }
        t2->nextSibling = t1->leftChild;    // t2是根节点 必然没有兄弟
        t1->leftChild = t2;
        return t1;
    }

    void makeEmpty(BinomialNode *&t) {}

    BinomialNode *clone(BinomialNode *&t) const {}
};

int main() {
    return 0;
}
