#include <iostream>

using namespace std;

class BinarySearchTree {
private:
    struct BinaryNode {
        int val;
        BinaryNode* left;
        BinaryNode* right;

        BinaryNode(int val, BinaryNode* l, BinaryNode* r)
            : val(val)
            , left(l)
            , right(r) {}
    };

    BinaryNode* root;

public:
    BinarySearchTree()
        : root(nullptr) {}

    ~BinarySearchTree() {
        makeEmpty();
    }

    const BinaryNode* findMin() const {
        return findMin(root);
    }

    const BinaryNode* findMax() const {
        return findMax(root);
    }

    bool contains(int x) const {
        return contains(x, root);
    }

    bool isEmpty() const {
        return root == nullptr;
    }

    void makeEmpty() {
        makeEmpty(root);
    }

    void insert(int x) {
        insert(x, root);
    }

    void remove(int x) {
        remove(x, root);
    }

    void preOrder() const {
        preOrder(root);
    }

    void inOrder() const {
        inOrder(root);
    }

private:
    void makeEmpty(BinaryNode*& t) {
        if (t != nullptr) {
            makeEmpty(t->left);
            makeEmpty(t->right);
            delete t;
            t = nullptr;
        }
    }

    void insert(int x, BinaryNode*& t) {
        if (t == nullptr) {
            t = new BinaryNode(x, nullptr, nullptr);
        } else if (x < t->val) {
            insert(x, t->left);
        } else if (t->val < x) {
            insert(x, t->right);
        } else {
            return;
        }
    }

    void remove(int x, BinaryNode*& t) {
        if (t == nullptr) {
            return;
        }

        if (x < t->val) {
            remove(x, t->left);
        } else if (t->val < x) {
            remove(x, t->right);
        } else if (t->left != nullptr && t->right != nullptr) {
            // 已经递归到要删除的结点了 但该结点有两个儿子，来到了情况3
            // 找到右子树的最小值结点（后继），来代替要删除的结点
            // 解释：中序遍历后，左子树max和右子树min分别为待删点的前驱和后继
            // 然后删掉原本的右子树最小值结点
            t->val = findMin(t->right)->val;
            remove(t->val, t->right);
        } else {
            // 情况1和情况2：要删除的结点是叶子结点或只有一个子结点（情况3转化为情况1了）
            // 如果只有一个儿子，那就直接让那个儿子代替自己，不管左右（情况2）
            // 如果是叶子结点，那就直接删除自己（情况1）
            BinaryNode* oldNode = t;
            // 如果有左子树，就让t指向左子树，否则就让t指向右子树
            t = (t->left != nullptr) ? t->left : t->right;
            delete oldNode;
        }
    }

    const BinaryNode* findMin(const BinaryNode* t) const {
        if (t == nullptr) {
            return nullptr;
        }
        if (t->left == nullptr) {
            return t;
        }
        return findMin(t->left);
    }

    const BinaryNode* findMax(const BinaryNode* t) const {
        if (t == nullptr) {
            return nullptr;
        }
        if (t->right == nullptr) {
            return t;
        }
        return findMax(t->right);
    }

    bool contains(int x, const BinaryNode* t) const {
        if (t == nullptr) {
            return false;
        } else if (x < t->val) {
            return contains(x, t->left);
        } else if (t->val < x) {
            return contains(x, t->right);
        } else {
            return true;
        }
    }

    void preOrder(const BinaryNode* t) const {
        if (t == nullptr) {
            return;
        }

        cout << t->val << " ";
        preOrder(t->left);
        preOrder(t->right);
    }

    void inOrder(const BinaryNode* t) const {
        if (t == nullptr) {
            return;
        }

        inOrder(t->left);
        cout << t->val << " ";
        inOrder(t->right);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    BinarySearchTree bst;

    int t;
    cin >> t;
    while (t--) {
        string op;
        int x;
        cin >> op >> x;

        if (op[0] == 'I') {
            bst.insert(x);
        } else if (op[0] == 'R') {
            bst.remove(x);
        }
    }

    bst.inOrder();
    cout << endl << endl;
    bst.preOrder();

    return 0;
}
