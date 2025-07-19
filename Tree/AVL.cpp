// 平衡二叉树
// 基本操作与二叉搜索树相同，只是增加了维持平衡的调节机制。所以可以继承BST
// 执行插入/删除操作后，通过调节机制来维持平衡
// 当构建二叉搜索树时，可能会出现不平衡的情况
// 如果原序列是有序的，那么插入的顺序会导致树变成一条链表

// 平衡二叉树的特点是：每个结点的左子树和右子树的高度差不超过1
// 这个高度差称为平衡因子，平衡因子可以是-1、0或1
// 通过旋转操作来保持平衡
// 旋转操作分为四种情况：左单旋、右单旋、左双旋、右双旋

// 左旋：逆时针旋转，把自己变成 右孩子的左孩子；若右孩子的左子树 存在，把冲突的左孩子变成自己的 右孩子
// 右旋：顺时针旋转，把自己变成 左孩子的右孩子；若左孩子的右子树 存在，把冲突的右孩子变成自己的 左孩子

// 需要旋转的情况
// 1. LL型：失衡结点 平衡因子为2，其左孩子 平衡节点为1。此时 失衡结点 需要进行右旋转
// 2. RR型：失衡结点 平衡因子为-2，其右孩子 平衡节点为-1。此时 失衡结点 需要进行左旋转
// 3. LR型：失衡结点 平衡因子为2，其左孩子 平衡节点为-1。此时 失衡结点的左孩子 需要先进行左旋转，然后 失衡节点
// 再进行右旋转
// 4. RL型：失衡结点 平衡因子为-2，其右孩子 平衡节点为1。此时 失衡结点的右孩子 需要先进行右旋转，然后 失衡节点
// 再进行左旋转

// 判断失衡类型：看新插入的结点在失衡结点的哪个孩子的哪个子树上。一一对应，很好记忆
// LL型：新结点在失衡结点的 左孩子的左子树上
// RR型：新结点在失衡结点的 右孩子的右子树上
// LR型：新结点在失衡结点的 左孩子的右子树上
// RL型：新结点在失衡结点的 右孩子的左子树上

// 插入结点后，如果造成多个祖先结点失衡，调整最靠下的失衡结点即可，其他失衡结点会自动平衡
// 但删除结点后，可能会导致多个祖先结点失衡，需要从删除结点的父结点开始向上调整，直到根结点

#include <iostream>

template<class Comparable>
class AvlTree {
public:
    struct AvlNode {
        Comparable &element;
        AvlNode *left;
        AvlNode *right;
        int height;

        AvlNode(const Comparable &theElement, AvlNode *l, AvlNode *r, int h = 0) : element(theElement), left(l), right(r), height(h) {}
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

    void updateHeight(AvlNode *t) {
        if (t != nullptr) {
            t->height = std::max(height(t->left), height(t->right)) + 1;
        }
    }

    // 一次插入操作只会导致从插入点到根节点的路径上，最多一个节点失衡。
    // 并且，一旦这个失衡的节点通过单旋转或双旋转得到修复，那么整个树就会恢复平衡。

    // 这里通过递归自底向上来实现平衡调节，并没有借助父亲指针
    // 事实上，许多树都可以加入父亲指针来优化操作
    void insert(const Comparable &x, AvlNode *&t) {
        // 递归终点，单独开一个if分支
        if (t == nullptr) {
            t = new AvlNode(x, nullptr, nullptr);
            return;
        }

        if (x < t->element) {
            insert(x, t->left);
            // 向上回溯时检查平衡因子
            // 失衡结点只有可能出现在查找的路径上，正好借助递归往回找
            if (height(t->left) - height(t->right) == 2) {
                if (x < t->left->element) {
                    rotateRight(t);        // LL型，右旋
                } else {
                    rotateLeftRight(t);    // LR型
                }
            }
        } else if (t->element < x) {
            insert(x, t->right);

            if (height(t->right) - height(t->left) == 2) {
                if (t->right->element < x) {
                    rotateLeft(t);         // RR型，左旋
                } else {
                    rotateRightLeft(t);    // RL型
                }
            }
        } else {
            ;               // 不允许结点元素重复
        }
        updateHeight(t);    // 更新当前结点的高度
    }

    // 旋转操作，传入的参数是失衡结点
    // 左旋：逆时针旋转，把自己变成 右孩子的左孩子；若右孩子的左子树 存在，把冲突的左孩子变成自己的 右孩子
    // 右旋：顺时针旋转，把自己变成 左孩子的右孩子；若左孩子的右子树 存在，把冲突的右孩子变成自己的 左孩子

    // 单左旋：RR型
    void rotateLeft(AvlNode *&curr) {
        AvlNode *right_son = curr->right;
        curr->right = right_son->left;    // 若右孩子的左子树存在，把冲突的左孩子变成自己的右孩子
        right_son->left = curr;           // 把自己变成右孩子的左孩子

        // 更新高度，只有旋转点和旋转中心点需要更新
        curr->height = std::max(height(curr->left), height(curr->right)) + 1;
        right_son->height = std::max(curr->height, height(right_son->right)) + 1;

        // 这条语句，看了视频会有更深刻的理解
        // 直接解释的话，还是要回到树的结构。这里传参是指针引用，直接修改原树结构
        // 旋转后，本身应该在这个位置的结点变成了右孩子
        // 这里再补充一下指针赋值的含义：两个指针指向了同一个内存地址
        curr = right_son;
    }

    // 单右旋：LL型
    void rotateRight(AvlNode *&curr) {
        AvlNode *left_son = curr->left;
        curr->left = left_son->right;
        left_son->right = curr;

        curr->height = std::max(height(curr->left), height(curr->right)) + 1;
        left_son->height = std::max(height(left_son->left), curr->height) + 1;
        curr = left_son;
    }

    // 双旋转：LR型
    void rotateLeftRight(AvlNode *&curr) {
        rotateLeft(curr->left);    // 左孩子先左旋
        rotateRight(curr);         // 自己再右旋
    }

    // 双旋转：RL型
    void rotateRightLeft(AvlNode *&curr) {
        rotateRight(curr->right);    // 右孩子先右旋
        rotateLeft(curr);            // 自己再左旋
    }
};

int main() {
    return 0;
}
