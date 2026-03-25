#include <iostream>

using namespace std;

class AVLTree {
private:
    struct AVLNode {
        int val;
        AVLNode* left;
        AVLNode* right;
        int height;

        AVLNode(int val, AVLNode* l, AVLNode* r, int h = 0)
            : val(val)
            , left(l)
            , right(r)
            , height(h) {}
    };

    AVLNode* root;
    string message;    // 记录本次操作的平衡信息

public:
    AVLTree()
        : root(nullptr)
        , message("") {}

    ~AVLTree() {
        makeEmpty(root);
    }

    int height(const AVLNode* t) const {
        return t == nullptr ? -1 : t->height;
    }

    void insert(int x) {
        insert(x, root, 0);    // 参数3为level深度。不额外统计，而是在递归的过程中维护深度
    }

    void remove(int x) {
        remove(x, root, 0, x);
    }

    void preOrder() const {
        preOrder(root);
        cout << endl;
    }

    void inOrder() const {
        inOrder(root);
        cout << endl;
    }

    void clearMessage() {
        message.clear();
    }

    const string& getMessage() const {
        return message;
    }

private:
    void makeEmpty(AVLNode*& t) {
        if (t != nullptr) {
            makeEmpty(t->left);
            makeEmpty(t->right);
            delete t;
            t = nullptr;
        }
    }

    void updateHeight(AVLNode* t) {
        if (t != nullptr) {
            t->height = max(height(t->left), height(t->right)) + 1;
        }
    }

    void balance(AVLNode*& t, const string& op, int x, int level) {
        if (t == nullptr) {
            return;
        }

        updateHeight(t);

        int lh = height(t->left);
        int rh = height(t->right);
        int unbalanced_node = t->val;

        if (lh - rh > 1) {
            if (height(t->left->left) >= height(t->left->right)) {
                rotateRight(t);        // LL型，右旋
                setMessage(op, x, unbalanced_node, level, "right rotation");
            } else {
                rotateLeftRight(t);    // LR型
                setMessage(op, x, unbalanced_node, level, "left rotation and right rotation");
            }
        } else if (rh - lh > 1) {
            if (height(t->right->right) >= height(t->right->left)) {
                rotateLeft(t);         // RR型，左旋
                setMessage(op, x, unbalanced_node, level, "left rotation");
            } else {
                rotateRightLeft(t);    // RL型
                setMessage(op, x, unbalanced_node, level, "right rotation and left rotation");
            }
        }
    }

    // 一次插入操作只会导致从插入点到根节点的路径上，最多一个节点失衡。
    // 并且，一旦这个失衡的节点通过单旋转或双旋转得到修复，那么整个树就会恢复平衡。
    void insert(int x, AVLNode*& t, int level) {
        // 递归终点，单独开一个if分支
        if (t == nullptr) {
            t = new AVLNode(x, nullptr, nullptr);
            return;
        }

        // 向上回溯时检查平衡因子
        // 失衡结点只有可能出现在查找的路径上，正好借助递归往回找
        if (x < t->val) {
            insert(x, t->left, level + 1);
        } else if (t->val < x) {
            insert(x, t->right, level + 1);
        } else {
            return;    // 不允许有重复元素
        }

        balance(t, "Insert", x, level);
    }

    // 单左旋：RR型
    void rotateLeft(AVLNode*& curr) {
        AVLNode* right_son = curr->right;
        curr->right = right_son->left;    // 若右孩子的左子树存在，把冲突的左孩子变成自己的右孩子
        right_son->left = curr;           // 把自己变成右孩子的左孩子

        // 更新高度，只有旋转点和旋转中心点需要更新
        curr->height = max(height(curr->left), height(curr->right)) + 1;
        right_son->height = max(curr->height, height(right_son->right)) + 1;
        curr = right_son;
    }

    // 单右旋：LL型
    void rotateRight(AVLNode*& curr) {
        AVLNode* left_son = curr->left;
        curr->left = left_son->right;
        left_son->right = curr;

        curr->height = max(height(curr->left), height(curr->right)) + 1;
        left_son->height = max(height(left_son->left), curr->height) + 1;
        curr = left_son;
    }

    // 双旋转：LR型
    void rotateLeftRight(AVLNode*& curr) {
        rotateLeft(curr->left);    // 左孩子先左旋
        rotateRight(curr);         // 自己再右旋
    }

    // 双旋转：RL型
    void rotateRightLeft(AVLNode*& curr) {
        rotateRight(curr->right);    // 右孩子先右旋
        rotateLeft(curr);            // 自己再左旋
    }

    const AVLNode* findMin(const AVLNode* t) const {
        if (t == nullptr) {
            return nullptr;
        }
        if (t->left == nullptr) {
            return t;
        }
        return findMin(t->left);
    }

    void remove(int x, AVLNode*& t, int level, int ori_x) {
        if (t == nullptr) {
            return;
        }

        if (x < t->val) {
            remove(x, t->left, level + 1, ori_x);
        } else if (t->val < x) {
            remove(x, t->right, level + 1, ori_x);
        } else if (t->left != nullptr && t->right != nullptr) {
            t->val = findMin(t->right)->val;
            remove(t->val, t->right, level + 1, ori_x);
        } else {
            AVLNode* oldNode = t;
            t = (t->left != nullptr) ? t->left : t->right;
            delete oldNode;
        }

        if (t != nullptr) {
            balance(t, "Remove", ori_x, level);
        }
    }

    void preOrder(const AVLNode* t) const {
        if (t == nullptr) {
            return;
        }

        cout << t->val << " ";
        preOrder(t->left);
        preOrder(t->right);
    }

    void inOrder(const AVLNode* t) const {
        if (t == nullptr) {
            return;
        }

        inOrder(t->left);
        cout << t->val << " ";
        inOrder(t->right);
    }

    void setMessage(const string& op, int x, int unbalanced_node, int level, const string& type) {
        if (message.empty()) {
            message += op + ' ' + to_string(x) + ": Rebalance subtree rooted at node " + to_string(unbalanced_node) +
                       " on level " + to_string(level) + " with " + type + '.' + ' ';
        } else {
            message += "Rebalance subtree rooted at node " + to_string(unbalanced_node) + " on level " +
                       to_string(level) + " with " + type + '.' + ' ';
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int cnt = 0, t;
    while (cin >> t) {
        AVLTree tree;
        bool flag = false;    // 标记本次操作是否导致了平衡调整
        cnt++;
        if (cnt > 1) {
            cout << endl;     // 每个测试用例之间空一行
        }
        cout << "Case " << cnt << ':' << endl;

        for (int i = 1; i <= t; i++) {
            string op;
            int x;
            cin >> op >> x;
            tree.clearMessage();    // 每次操作前清空上次的平衡信息

            if (op[0] == 'I') {
                tree.insert(x);
            } else if (op[0] == 'R') {
                tree.remove(x);
            }

            // 如果本次操作导致了平衡调整，打印平衡信息，同时标记
            if (!tree.getMessage().empty()) {
                cout << tree.getMessage() << endl;
                flag = true;
            }
        }

        if (flag) {
            cout << endl;
        }

        tree.inOrder();
        cout << endl;
        tree.preOrder();
    }

    return 0;
}
