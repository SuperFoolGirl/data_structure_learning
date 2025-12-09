// BST的特点是，中序遍历结果是有序的
// 因此，可以通过中序遍历来判断一棵树是否为BST

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

    bool isBST() {
        TreeNode* prev = nullptr;  // 用于记录中序遍历的前一个节点
        return isBSTUtil(root, prev);
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

    void buildTree(TreeNode*& t) {
        string val;
        cin >> val;
        if (val == "#") {    // 约定#表示空节点
            t = nullptr;
            return;
        }
        t = new TreeNode(stoi(val));
        buildTree(t->left);
        buildTree(t->right);
    }

    bool isBSTUtil(TreeNode* t, TreeNode*& prev) {
        if (t == nullptr) {
            return true;
        }

        // 检查左子树
        if (!isBSTUtil(t->left, prev)) {
            return false;
        }

        // 检查当前节点
        if (prev != nullptr && t->val <= prev->val) {
            return false;
        }
        prev = t; // 更新prev为当前节点

        // 检查右子树
        return isBSTUtil(t->right, prev);
    }

private:
    TreeNode* root;
};

int main() {
    BinaryTree tree;

    if (tree.isBST()) {
        cout << "The tree is a BST." << endl;
    } else {
        cout << "The tree is not a BST." << endl;
    }

    return 0;
}
