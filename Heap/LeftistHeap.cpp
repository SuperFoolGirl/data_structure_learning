#include <iostream>
using namespace std;

// 节点的零路径长：该节点到一个不具有两个儿子的节点的最短路径的长
// 任一节点的零路径长比它诸儿子节点的零路径长的最小值多1，null的零路径长是-1
// 左式堆性质：对于每一个节点 左儿子的零路径长至少与右儿子的零路径长一样。这使得树偏向左增加深度
template<class Comparable>
class LeftistHeap {
public:
    LeftistHeap() {}

    LeftistHeap(const LeftistHeap &rhs) {}

    ~LeftistHeap() {}

    bool isEmpty() {}

    const Comparable &findMin() {}

    void insert(const Comparable &x) {
        root = merge(new LeftistNode(x), root);    // 将插入视为合并的特例
    }

    void deleteMin() {
        if (isEmpty()) {
            throw UnderflowException();
        }
        LeftistNode *oldRoot = root;
        root = merge(root->left, root->right);
        delete oldRoot;
    }

    void deleteMin(Comparable &minItem) {
        minItem = findMin();
        deleteMin();
    }

    void makeEmpty() {}

    void merge(LeftistHeap &rhs) {
        if (this == rhs) {    // 合并同一个堆没有意义
            return;
        }
        root = merge(root, rhs.root);
        rhs.root = nullptr;    // 把传入堆rhs的根节点设为nullptr，这样rhs堆在逻辑上就变为空堆，避免后续误操作，也符合合并操作后一个堆被并入另一个堆的语义
    }

    const LeftistHeap &operator=(const LeftistHeap &rhs) {}

private:
    struct LeftistNode {
        Comparable element;
        LeftistNode *left;
        LeftistNode *right;
        int npl;

        LeftistNode(const Comparable &theElement, LeftistNode *l = nullptr, LeftistNode *r = nullptr, int np = 0)
            : element(theElement), left(l), right(r), npl(np) {}
    };

    LeftistNode *root;

    LeftistNode *merge(LeftistNode *h1, LeftistNode *h2) {
        if (h1 == nullptr) {
            return h2;
        }
        if (h2 == nullptr) {
            return h1;
        }
        // 根据左式堆的堆序性质（在最小左式堆里，父节点元素小于等于子节点元素），比较h1和h2的根节点元素值。如果h1->element <
        // h2->element，调用merge1函数，将h2合并到h1下；否则，把h1合并到h2下，这样能保证合并后的堆依然满足堆序性质。
        if (h1->element < h2->element) {
            return merge1(h1, h2);
        } else {
            return merge1(h2, h1);
        }
    }

    LeftistNode *merge1(LeftistNode *h1, LeftistNode *h2) {
        // 检查h1的左子树是否为空，如果为空，直接把h2作为h1的左子树。这是一种简单的合并起始情况，后续还需调整以满足左式堆性质
        if (h1->left == nullptr) {
            h1->left = h2;
        } else {
            h1->right = merge(
                h1->right,
                h2);    // 将h2与h1的右子树进行合并。这利用了递归，不断把小的子堆合并到合适位置，保证合并过程符合堆序性质.如果h1->right=nullptr,那h2就成为h1->right。这里注意是找h1的右子树
            if (h1->left->npl < h1->right->npl) {    // 维持左式堆的左偏性，即左子树的零路径长度至少和右子树的一样大。每次递归都要维持一次
                swapChildren(h1);
            }
            h1->npl = h1->right->npl + 1;            // 因为根据定义，节点的零路径长度比它诸子节点零路径长度的最小值多 1。
        }
        return h1;                                   // 最后返回合并后的节点h1，作为合并结果，这个节点将作为新的子树或者堆顶，融入到更大的左式堆结构中
    }

    void swapChildren(LeftistNode *t) {}

    void reclaimMemory(LeftistNode *t) {}

    LeftistNode *clone(LeftistNode *t) const {}
};

int main() {
    return 0;
}
