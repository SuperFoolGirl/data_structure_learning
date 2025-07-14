#include <iostream>

template<class Comparable>
class AvlTree {
public:
    struct AvlNode {
        Comparable &element;
        AvlNode *left;
        AvlNode *right;
        int height;

        AvlNode(const Comparable &theElement, AvlNode *l, AvlNode *r, int h = 0)
            : element(theElement), left(l), right(r), height(h) {}
    };

    AvlTree() : root(nullptr) {}

    AvlTree(const AvlTree &rhs) {}

    ~AvlTree() {}

    int height(AvlNode *t) const {
        return t == nullptr ? -1 : t->height;
    }

    void insert(const Comparable &x) {
        insert(x, root)
    }

private:
    AvlNode *root;

    void insert(const Comparable &x, AvlNode *&t) {
        if (t == nullptr) {    // 递归终点 只要不重复就有能插入的地方
            t = new AvlNode(x, nullptr, nullptr);
        } else if (x < t->element) {
            insert(x, t->left);
            if (height(t->left) - height(t->right) ==
                2) {    // 这里找的是k1(t) 从插入点开始往上找 就是从栈顶不断往下剥的过程 注意总是先找k1 再顺势找k2 k3
                if (x < t->left->element) {    // x与k3的值进行比较 看看是四种情形的哪一个
                    rotateWithLeftChild(t);
                } else {
                    doubleWithLeftChild(t);
                }
            }
        } else if (t->element < x) {
            insert(x, t->right) if (height(t->right) - height(t->left) == 2) if (t->right->element < x)
                rotateWithRightChild(t);
            else doubleWithRightChild(t);
        } else                                                     // 不允许重复
            ;                                                      // 不能return 否则下面的更新走不了
        t->height = max(height(t->left), height(t->right)) + 1;    // 如果执行了旋转
    }

    void rotateWithLeftChild(AvlNode *&k1) {
        AvlNode *k3 = k1->left;    // 找到k3
        k1->left = k3->right;      // k3的另一颗子树给k1
        k3->right = k1;
        k1->height = max(height(k1->left), height(k1->right)) + 1;
        k3->height = max(height(k3->left), k1->height) + 1;
        k1 = k3;    // 这一步才是实现了旋转，前面只是连连看。感性地讲，k1的含义是根节点，将k1修改为k3，是修改了根节点
        // 将 k1 赋值为
        // k3，就意味着更新了外部指向这棵子树的指针，使得调用者后续访问该子树时，是从新的、已经平衡的根节点（也就是 k3）
        // 开始，维持了 AVL 树整体结构的连贯性与正确性，确保后续对这棵子树的操作（如继续插入元素、查询高度等）
        // 都基于新的平衡结构展开。 简单说 形参k1是指针引用，所以可以直接对k1进行修改，把根节点修改成k3
    }

    void doubleWithLeftChild(AvlNode *&k1) {
        rotateWithRightChild(k1->left);
        rotateWithLeftChild(k1);
    }

    void rotateWithRightChild(AvlNode *&k1) {
        AvlNode *k3 = k1->right;
        k1->right = k3->left;
        k3->left = k1;
        k1->height = max(height(k1->left), height(k1->right)) + 1;
        k3->height = max(k1->height, height(k3->right)) + 1;
        k1 = k3;
    }

    void doubleWithRightChild(AvlNode *&k1) {
        rotateWithLeftChild(k1->right);
        rotateWithRightChild(k1);
    }
};

int main() {
    return 0;
}
