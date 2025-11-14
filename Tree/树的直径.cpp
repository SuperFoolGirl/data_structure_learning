// 思路：借助树的深度算法
// 遍历整棵树，经过一个点的直径等于左子树高度+右子树高度+1

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

    int getDiameter() {
        int diameter = 0;
        getDiameter(root, diameter);
        return diameter;
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

    // 建树方法
    // 这里提供根据前序遍历来建树的方法，边输入边建树
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

    void printPreorder(TreeNode* t) {
        if (t == nullptr) {
            return;
        }

        cout << t->val << ' ';
        printPreorder(t->left);
        printPreorder(t->right);
    }

    int getDiameter(TreeNode* t, int& diameter) {
        if (t == nullptr) {
            return 0;
        }

        int left_height = getDiameter(t->left, diameter);
        int right_height = getDiameter(t->right, diameter);

        // 更新直径
        diameter = max(diameter, left_height + right_height + 1);

        // 返回当前根节点对应子树的高度
        return max(left_height, right_height) + 1;
    }

private:
    TreeNode* root;
};

int main() {
    BinaryTree tree;
    // 默认构造函数会调用buildTree，接受输入来建树


    return 0;
}
