// 根据规范写法，应该把root设为私有成员
// 涉及到root的操作，都通过公有方法委托给重载私有方法
// 其中公有方法不含参，然后内部调用重载的私有方法，传入root
// root的初始化放在构造函数中，构造函数委托给buildTree方法

#include <iostream>

using namespace std;

class BinaryTree {
private:
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
    BinaryTree() {
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

    // 删除子树
    // 子树被删除后，必须保证其父结点的对应指向设为nullptr
    bool deleteSubtree(int val) {
        TreeNode* parent_of_del = findParent(root, val);
        if (parent_of_del == nullptr) {
            // 特例：根节点被删除
            if (root != nullptr && root->val == val) {
                makeEmpty(root);
                return true;
            } else {
                return false;
            }
        }
        // 找到待删除子树是左子树还是右子树，断开父节点指向
        // 注意时刻加上判空条件
        TreeNode* to_del = nullptr;
        if (parent_of_del->left != nullptr && parent_of_del->left->val == val) {
            to_del = parent_of_del->left;
            parent_of_del->left = nullptr;
        } else if (parent_of_del->right != nullptr && parent_of_del->right->val == val) {
            to_del = parent_of_del->right;
            parent_of_del->right = nullptr;
        }
        if (to_del != nullptr) {
            makeEmpty(to_del);
            return true;
        }
        return false;
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

    TreeNode* findParent(TreeNode* t, int val) {
        if (t == nullptr) {
            return nullptr;
        }

        if ((t->left != nullptr && t->left->val == val) || (t->right != nullptr && t->right->val == val)) {
            return t;
        }
        TreeNode* left_res = findParent(t->left, val);
        if (left_res != nullptr) {
            return left_res;
        }
        return findParent(t->right, val);
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
