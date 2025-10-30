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

    // 根据规范写法，应该把root设为私有成员
    // 涉及到root的操作，都通过公有方法委托给重载私有方法
    // 其中公有方法不含参，然后内部调用重载的私有方法，传入root
    // 为了避免代码量过大，这里就不提供重载版本了，并对外开放root
    TreeNode* root;

public:
    BinaryTree()
        : root(nullptr) {}
    ~BinaryTree() {
        makeEmpty(root);
    }

    // 建树方法
    // 这里提供根据前序遍历来建树的方法，边输入边建树
    void buildTreePreOrder(TreeNode*& t) {
        int val;
        cin >> val;
        if (val == 0) { // 约定0表示空节点
            t = nullptr;
            return;
        }

        t = new TreeNode(val);
        buildTreePreOrder(t->left);
        buildTreePreOrder(t->right);
    }

private:
    void makeEmpty(TreeNode*& t) {
        if (t != nullptr) {
            makeEmpty(t->left);
            makeEmpty(t->right);
            delete t;
            t = nullptr;
        }
    }
};
