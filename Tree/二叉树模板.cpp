// 根据规范写法，应该把root设为私有成员
// 涉及到root的操作，都通过公有方法委托给重载私有方法
// 其中公有方法不含参，然后内部调用重载的私有方法，传入root
// root的初始化放在构造函数中，构造函数委托给buildTree方法

#include <iostream>

using namespace std;

class BinaryTree {
public:
    struct TreeNode {
        int val;
        TreeNode* left;
        TreeNode* right;

        TreeNode(int x)
            : val(x)
            , left(nullptr)
            , right(nullptr) {}
    };

public:
    BinaryTree()
        : root(nullptr) {
        buildTree(root);
    }

    ~BinaryTree() {
        makeEmpty(root);
    }

    int getHeight() {
        return getHeight(root);
    }

    void printPreorder() {
        printPreorder(root);
    }

private:
    void makeEmpty(TreeNode*& t) {
        if (t == nullptr) {
            return;
        }

        makeEmpty(t->left);
        makeEmpty(t->right);
        delete t;
        t = nullptr;
    }

    int getHeight(TreeNode* t) {
        if (t == nullptr) {
            return -1;    // 空树高度定义为-1
        }
        return max(getHeight(t->left), getHeight(t->right)) + 1;
    }

    // 建树方法
    // 这里提供根据前序遍历来建树的方法，边输入边建树
    void buildTree(TreeNode*& t) {
        int val;
        cin >> val;
        if (val == 0) {    // 约定0表示空节点
            t = nullptr;
            return;
        }
        t = new TreeNode(val);
        buildTree(t->left);
        buildTree(t->right);
    }

    void printPreorder(TreeNode* t) {
        if (t == nullptr) {
            return;
        }

        cout << t->val << ' ';
        printPreorder(t->left);
        printPreorder(t->right);
    }

private:
    TreeNode* root;
};

int main() {
    BinaryTree tree;
    // 默认构造函数会调用buildTree，接受输入来建树

    // 获取树的高度
    cout << tree.getHeight() << endl;

    // 前序打印树的节点值
    tree.printPreorder();

    return 0;
}
