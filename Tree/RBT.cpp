// 红黑树
// 是另外一种平衡二叉搜索树，与AVL树相比，红黑树的平衡性稍弱（导致查找操作相对较慢），但插入和删除操作更快
// 红黑树的性质：
// 1. 结点是红色或黑色
// 2. 根结点是黑色
// 3. 每个叶子结点（NIL）是黑色（注意，红黑树中，叶子结点的定义是空结点——将现有的缺儿子的结点全部补齐）
// 4. 如果一个结点是红色，则它的两个子结点都是黑色（没有两个连续的红色结点）
// 5. 从任何结点到其每个叶子结点的所有路径都包含相同数量的黑色结点
// 总结：左根右，根叶黑，不红红，黑路同

// 由性质5，可推导出最长路径不超过最短路径的两倍，即任意结点左右子树高度差不超过2倍
// 因此，在控制树高方面，红黑树不如AVL树严格
// 对于最长路径，一定是黑红相间的（满足不红红）

// 插入结点默认为红色
// 红黑树性质被破坏的插入分为三种情况，只有可能会违反“不红红”（情况23）和“根叶黑”（情况1）
// 因此，对于情况23，父亲都是红色；如果父亲是黑色，那直接插入即可，属于正常插入
// 1. 插入根结点，将其设为黑色即可
// 2. 插入结点的叔叔结点（父亲的兄弟结点）是红色，叔父爷变色，爷爷变插入结点，然后重新进行新一轮判断，直到满足红黑树性质
// 3. 插入结点的叔叔结点是黑色（可能是空结点），需要判断四种类型，法则与AVL树相同：
//   a. LL型：插入结点在爷爷的左子树的左子树上，进行右旋转
//   b. RR型：插入结点在爷爷的右子树的右子树上，进行左旋转
//   c. LR型：插入结点在爷爷的左子树的右子树上，先左旋转父亲，再右旋转爷爷
//   d. RL型：插入结点在爷爷的右子树的左子树上，先右旋转父亲，再左旋转爷爷
// 旋转完后，对旋转点（爷爷）和旋转中心点（父亲/自己）变色
// 如果是LL型或RR型，旋转中心点是父亲；如果是LR型或RL型，旋转中心点是自己（因为父亲第一次执行旋转后，与自己换位了）

#include <algorithm>    // For std::max
#include <iostream>
#include <queue>        // For level-order traversal (optional, but good for visualization)
#include <stdexcept>    // For std::runtime_error in case T doesn't have default constructor
#include <string>       // For printing node colors

template<typename T>
class RedBlackTree {
private:
    // 定义结点颜色
    enum Color {
        RED,
        BLACK
    };

    // 结点结构
    struct Node {
        T data;
        Color color;
        Node *parent;
        Node *left;
        Node *right;

        // 结点构造函数
        Node(T val) : data(val), color(RED), parent(nullptr), left(nullptr), right(nullptr) {}
    };

    Node *root;
    Node *NIL;    // 哨兵结点，所有叶子结点和空子树都指向它

    // 辅助函数：获取结点颜色
    Color getColor(Node *node) const {
        if (node == nullptr || node == NIL) {
            return BLACK;    // 空结点或NIL结点视为黑色
        }
        return node->color;
    }

    // 辅助函数：设置结点颜色
    void setColor(Node *node, Color color) {
        if (node != nullptr && node != NIL) {
            node->color = color;
        }
    }

    // 辅助函数：获取父结点
    Node *parent(Node *node) const {
        return (node == nullptr || node == NIL) ? nullptr : node->parent;
    }

    // 辅助函数：获取祖父结点
    Node *grandparent(Node *node) const {
        Node *p = parent(node);
        return (p == nullptr || p == NIL) ? nullptr : parent(p);
    }

    // 辅助函数：获取兄弟结点
    Node *sibling(Node *node) const {
        Node *p = parent(node);
        if (p == nullptr || p == NIL) {
            return nullptr;    // 没有父结点，就没有兄弟结点
        }
        if (node == p->left) {
            return p->right;
        } else {
            return p->left;
        }
    }

    // 辅助函数：获取叔叔结点
    Node *uncle(Node *node) const {
        Node *gp = grandparent(node);
        if (gp == nullptr || gp == NIL) {
            return nullptr;    // 没有祖父结点，就没有叔叔结点
        }
        if (parent(node) == gp->left) {
            return gp->right;
        } else {
            return gp->left;
        }
    }

    // 左旋 (Left-Rotate)
    //         curr           right_son
    //        /    \         /       \
    //      L     right_son curr     RR_son
    //           /     \    /   \
    //         RL_son   RR_son L   RL_son
    void leftRotate(Node *curr) {
        Node *right_son = curr->right;          // right_son 将成为新的子树根

        curr->right = right_son->left;          // 将 right_son 的左子树挂到 curr 的右边
        if (right_son->left != NIL) {
            right_son->left->parent = curr;     // 更新 right_son 左子树的父指针
        }

        right_son->parent = curr->parent;       // 更新 right_son 的父指针
        if (curr->parent == nullptr) {
            root = right_son;                   // 如果 curr 是根结点，则 right_son 成为新根
        } else if (curr == curr->parent->left) {
            curr->parent->left = right_son;     // curr 是其父结点的左子，则 right_son 也成为左子
        } else {
            curr->parent->right = right_son;    // curr 是其父结点的右子，则 right_son 也成为右子
        }

        right_son->left = curr;                 // curr 成为 right_son 的左子
        curr->parent = right_son;               // 更新 curr 的父指针
    }

    // 右旋 (Right-Rotate)
    //         curr           left_son
    //        /    \         /       \
    //   left_son   R     LL_son     curr
    //  /       \                 /   \
    // LL_son   LR_son          LR_son  R
    void rightRotate(Node *curr) {
        Node *left_son = curr->left;           // left_son 将成为新的子树根

        curr->left = left_son->right;          // 将 left_son 的右子树挂到 curr 的左边
        if (left_son->right != NIL) {
            left_son->right->parent = curr;    // 更新 left_son 右子树的父指针
        }

        left_son->parent = curr->parent;       // 更新 left_son 的父指针
        if (curr->parent == nullptr) {
            root = left_son;                   // 如果 curr 是根结点，则 left_son 成为新根
        } else if (curr == curr->parent->left) {
            curr->parent->left = left_son;     // curr 是其父结点的左子，则 left_son 也成为左子
        } else {
            curr->parent->right = left_son;    // curr 是其父结点的右子，则 left_son 也成为右子
        }

        left_son->right = curr;                // curr 成为 left_son 的右子
        curr->parent = left_son;               // 更新 curr 的父指针
    }

    // 插入修复 (Fix-up after insertion)
    void insertFixup(Node *k) {
        // 当k的父结点是红色时才需要修复 (性质4：红色结点的子结点必须是黑色)
        // 根结点必须是黑色 (性质2)
        while (k != root && getColor(parent(k)) == RED) {
            Node *p = parent(k);
            Node *gp = grandparent(k);

            // 情况1: 父结点是祖父结点的左子
            if (p == gp->left) {
                Node *u = uncle(k);    // 叔叔结点

                // 情况1.1: 叔叔结点是红色 (父结点和叔叔结点都是红色)
                if (getColor(u) == RED) {
                    setColor(p, BLACK);    // 父结点变黑
                    setColor(u, BLACK);    // 叔叔结点变黑
                    setColor(gp, RED);     // 祖父结点变红
                    k = gp;                // 将k指向祖父结点，继续向上检查
                }
                // 情况1.2: 叔叔结点是黑色 (单旋转或双旋转)
                else {
                    // 情况1.2.1: k 是父结点的右子 (LR情况)
                    if (k == p->right) {
                        leftRotate(p);    // 对父结点进行左旋，转换为LL情况
                        k = p;            // k现在是原来的父结点
                        p = parent(k);    // 更新p
                    }
                    // 情况1.2.2: k 是父结点的左子 (LL情况)
                    rightRotate(gp);       // 对祖父结点进行右旋
                    setColor(p, BLACK);    // 父结点变黑 (新根)
                    setColor(gp, RED);     // 祖父结点变红 (新根的右子)
                    // 旋转后，p成为新的子树根，k的平衡性已经解决，循环结束
                }
            }
            // 情况2: 父结点是祖父结点的右子 (与情况1对称)
            else {
                Node *u = uncle(k);

                // 情况2.1: 叔叔结点是红色
                if (getColor(u) == RED) {
                    setColor(p, BLACK);
                    setColor(u, BLACK);
                    setColor(gp, RED);
                    k = gp;
                }
                // 情况2.2: 叔叔结点是黑色
                else {
                    // 情况2.2.1: k 是父结点的左子 (RL情况)
                    if (k == p->left) {
                        rightRotate(p);    // 对父结点进行右旋，转换为RR情况
                        k = p;
                        p = parent(k);
                    }
                    // 情况2.2.2: k 是父结点的右子 (RR情况)
                    leftRotate(gp);        // 对祖父结点进行左旋
                    setColor(p, BLACK);    // 父结点变黑 (新根)
                    setColor(gp, RED);     // 祖父结点变红 (新根的左子)
                    // 旋转后，p成为新的子树根，k的平衡性已经解决，循环结束
                }
            }
        }
        setColor(root, BLACK);    // 确保根结点始终是黑色
    }

    // 插入操作 (私有辅助函数)
    void insert(Node *node) {
        Node *y = nullptr;    // y 是 node 的父结点
        Node *x = root;       // x 用于遍历树

        while (x != NIL) {
            y = x;
            if (node->data < x->data) {
                x = x->left;
            } else if (node->data > x->data) {    // 防止插入重复元素
                x = x->right;
            } else {                              // 元素重复，不插入新结点，直接删除新创建的结点并返回
                delete node;
                return;
            }
        }

        node->parent = y;       // 设置 node 的父结点
        if (y == nullptr) {
            root = node;        // 如果树为空，node 成为根结点
        } else if (node->data < y->data) {
            y->left = node;     // node 成为 y 的左子
        } else {                // node->data > y->data
            y->right = node;    // node 成为 y 的右子
        }

        node->left = NIL;       // 新插入结点的左右子都指向NIL
        node->right = NIL;      // 确保新插入结点是红色，其子结点NIL是黑色 (满足性质4)
        setColor(node, RED);    // 新插入结点默认为红色 (可能违反性质2和性质4)

        insertFixup(node);      // 修复红黑树性质
    }

    // 查找最小结点
    Node *minimum(Node *node) const {
        while (node->left != NIL) {
            node = node->left;
        }
        return node;
    }

    // 查找后继结点 (用于删除)
    Node *successor(Node *node) const {
        if (node->right != NIL) {
            return minimum(node->right);    // 如果有右子树，后继是右子树的最小结点
        }

        // 如果没有右子树，后继是第一个比node大的祖先结点
        Node *p = node->parent;
        while (p != nullptr && node == p->right) {
            node = p;
            p = p->parent;
        }
        return p;    // 如果是最大结点，返回nullptr
    }

    // 辅助函数：将子树 u 替换为子树 v
    void transplant(Node *u, Node *v) {
        if (u->parent == nullptr) {
            root = v;                // u 是根结点，v 成为新根
        } else if (u == u->parent->left) {
            u->parent->left = v;     // u 是父结点的左子，v 成为左子
        } else {
            u->parent->right = v;    // u 是父结点的右子，v 成为右子
        }
        // 更新 v 的父指针，NIL结点的父指针通常不重要，但为了保持一致性
        if (v != NIL) {
            v->parent = u->parent;
        }
    }

    // 删除修复 (Fix-up after deletion)
    void deleteFixup(Node *x) {
        while (x != root && getColor(x) == BLACK) {    // x 是双黑结点，或者替代结点是黑色且其子结点是红色
            Node *p = parent(x);
            Node *s = sibling(x);                      // 兄弟结点

            // 情况1: 兄弟结点 s 是红色
            if (getColor(s) == RED) {
                setColor(s, BLACK);
                setColor(p, RED);
                if (x == p->left) {
                    leftRotate(p);
                } else {
                    rightRotate(p);
                }
                s = sibling(x);    // 旋转后，兄弟结点改变，重新获取兄弟结点
            }

            // 情况2: 兄弟结点 s 是黑色，且 s 的两个子结点都是黑色
            if (getColor(s->left) == BLACK && getColor(s->right) == BLACK) {
                setColor(s, RED);    // 兄弟结点变红
                x = p;               // 将 x 向上移动到父结点，继续循环
            }
            // 情况3: 兄弟结点 s 是黑色，s 的一个子结点红色，另一个黑色 (单旋转或双旋转)
            else {
                // 情况3.1: x 是父结点的左子，s 的右子是黑色 (LR/LL情况)
                if (x == p->left) {
                    if (getColor(s->right) == BLACK) {    // s 的左子是红色
                        setColor(s->left, BLACK);
                        setColor(s, RED);
                        rightRotate(s);                   // 对兄弟结点进行右旋，转换为情况3.2
                        s = sibling(x);                   // 再次更新兄弟结点
                    }
                    // 情况3.2: x 是父结点的左子，s 的右子是红色 (LL情况)
                    setColor(s, getColor(p));     // 兄弟结点继承父结点的颜色
                    setColor(p, BLACK);           // 父结点变黑
                    setColor(s->right, BLACK);    // s 的右子变黑
                    leftRotate(p);                // 对父结点进行左旋
                    x = root;                     // 修复完成，跳出循环 (将x设置为root，下次循环条件x!=root为false)
                }
                // 情况4: x 是父结点的右子 (与情况3对称)
                else {
                    if (getColor(s->left) == BLACK) {    // s 的右子是红色
                        setColor(s->right, BLACK);
                        setColor(s, RED);
                        leftRotate(s);                   // 对兄弟结点进行左旋，转换为情况4.2
                        s = sibling(x);                  // 再次更新兄弟结点
                    }
                    // 情况4.2: x 是父结点的右子，s 的左子是红色 (RR情况)
                    setColor(s, getColor(p));    // 兄弟结点继承父结点的颜色
                    setColor(p, BLACK);          // 父结点变黑
                    setColor(s->left, BLACK);    // s 的左子变黑
                    rightRotate(p);              // 对父结点进行右旋
                    x = root;                    // 修复完成，跳出循环
                }
            }
        }
        setColor(x, BLACK);    // 确保最后处理的结点是黑色 (根结点或修复终点)
    }

    // 删除操作 (私有辅助函数)
    // z 是要删除的结点
    void remove(Node *z) {
        Node *y = z;                             // y 是实际要删除或移动的结点
        Node *x;                                 // x 是 y 的替代结点，将用于修复

        Color y_original_color = getColor(y);    // 记录 y 的原始颜色

        if (z->left == NIL) {                    // z 没有左子树，直接用右子树替换 z
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == NIL) {            // z 没有右子树，直接用左子树替换 z
            x = z->left;
            transplant(z, z->left);
        } else {                                 // z 有两个子树
            y = minimum(z->right);               // y 是 z 的后继（z右子树的最小结点）
            y_original_color = getColor(y);      // 记录 y 的原始颜色
            x = y->right;                        // x 是 y 的右子（因为y是最小结点，所以它没有左子）

            // 这里需要特殊处理x的parent，因为如果y是z的直接右子，transplant(y, y->right)不会改变y->right的parent为y。
            // 实际上，CLR的实现中x的parent在transplant(y, y->right)之后会被设置为y的parent，
            // 但如果y就是z的right child, x的parent应该还是y。
            // 这里为了简化，直接在后续的deleteFixup中通过parent(x)获取p，
            // 所以transplant函数中v->parent = u->parent; 已经覆盖了。

            if (y->parent == z) {    // 如果 y 是 z 的直接右子
                x->parent = y;       // 这一行是 CLR 书中的一个技巧，为了在后续deleteFixup中简化对x->parent的获取，
                                     // 但对于本实现，x的parent会被transplant正确设置，所以此处不是严格必须
            } else {                 // y 不是 z 的直接右子
                transplant(y, y->right);    // 用 y 的右子树替换 y 的位置
                y->right = z->right;        // 将 z 的右子树挂到 y 的右边
                y->right->parent = y;       // 更新 z 原右子树的父指针为 y
            }
            transplant(z, y);               // 用 y 替换 z 的位置
            y->left = z->left;              // 将 z 的左子树挂到 y 的左边
            y->left->parent = y;            // 更新 z 原左子树的父指针为 y
            setColor(y, getColor(z));       // y 继承 z 的颜色
        }
        delete z;                           // 释放被删除的结点 z

        // 如果被删除结点或其替代结点y的原始颜色是黑色，则可能破坏红黑性质，需要修复
        if (y_original_color == BLACK) {
            deleteFixup(x);
        }
    }

    // 中序遍历 (私有辅助函数)
    void inorderTraversal(Node *node) const {
        if (node != NIL) {
            inorderTraversal(node->left);
            std::cout << node->data << "(" << (node->color == RED ? "R" : "B") << ") ";
            inorderTraversal(node->right);
        }
    }

    // 查找 (私有辅助函数)
    Node *search(Node *node, T key) const {
        if (node == NIL || node->data == key) {
            return node;
        }
        if (key < node->data) {
            return search(node->left, key);
        } else {
            return search(node->right, key);
        }
    }

    // 清除树 (私有辅助函数，用于析构)
    void clear(Node *node) {
        if (node != NIL) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

public:
    // 公共接口

    // 构造函数：初始化NIL结点和根结点
    RedBlackTree() : root(nullptr) {
        // 创建NIL结点，需要一个T的默认构造函数
        try {
            NIL = new Node(T());    // Value can be anything, as its data won't be accessed
        } catch (const std::exception &e) {
            // Handle case where T does not have a default constructor
            std::cerr << "Error: T must have a default constructor for NIL node. " << e.what() << std::endl;
            // Optionally, rethrow or use a different initialization strategy
            throw std::runtime_error("T must have a default constructor for NIL node.");
        }

        NIL->color = BLACK;
        NIL->left = nullptr;
        NIL->right = nullptr;
        NIL->parent = nullptr;    // NIL's parent is generally irrelevant

        root = NIL;               // Initially, the root points to NIL
    }

    // 析构函数：释放所有结点内存
    ~RedBlackTree() {
        clear(root);    // 清除所有结点
        delete NIL;     // 最后删除NIL结点
    }

    // 插入公共接口
    void insert(T key) {
        Node *newNode = new Node(key);
        insert(newNode);
    }

    // 删除公共接口
    void remove(T key) {
        Node *nodeToRemove = search(root, key);
        if (nodeToRemove != NIL) {
            remove(nodeToRemove);
        } else {
            std::cout << "Error: Key " << key << " not found for deletion." << std::endl;
        }
    }

    // 查找公共接口
    bool search(T key) const {
        return search(root, key) != NIL;
    }

    // 中序遍历公共接口
    void inorderTraversal() const {
        inorderTraversal(root);
        std::cout << std::endl;
    }

    // 判断树是否为空
    bool isEmpty() const {
        return root == NIL;
    }

    // 打印树（层序遍历，用于调试）
    void printTree() const {
        if (root == NIL) {
            std::cout << "Tree is empty." << std::endl;
            return;
        }

        std::queue<Node *> q;
        q.push(root);

        int level = 0;
        while (!q.empty()) {
            int levelSize = q.size();
            std::cout << "Level " << level << ": ";
            for (int i = 0; i < levelSize; ++i) {
                Node *curr = q.front();
                q.pop();

                std::cout << curr->data << "(" << (curr->color == RED ? "R" : "B") << ") ";
                if (curr->left != NIL) {
                    q.push(curr->left);
                }
                if (curr->right != NIL) {
                    q.push(curr->right);
                }
            }
            std::cout << std::endl;
            level++;
        }
    }
};

// 示例用法
int main() {
    // 对于某些类型T，T()可能不会默认构造。
    // 如果您的T类型没有默认构造函数，需要为NIL结点提供一个有效的初始值
    // 或专门化模板以处理无默认构造函数的情况。
    // 对于int，默认构造为0，所以是安全的。
    RedBlackTree<int> rbt;

    std::cout << "Inserting elements: 10, 20, 30, 15, 25, 5, 35" << std::endl;
    rbt.insert(10);
    rbt.insert(20);
    rbt.insert(30);
    rbt.insert(15);
    rbt.insert(25);
    rbt.insert(5);
    rbt.insert(35);

    std::cout << "Inorder Traversal: ";
    rbt.inorderTraversal();

    std::cout << "\nTree structure (Level-order traversal with colors):" << std::endl;
    rbt.printTree();

    std::cout << "\nSearching for 20: " << (rbt.search(20) ? "Found" : "Not Found") << std::endl;
    std::cout << "Searching for 100: " << (rbt.search(100) ? "Found" : "Not Found") << std::endl;

    std::cout << "\nDeleting 20..." << std::endl;
    rbt.remove(20);
    std::cout << "Inorder Traversal after deleting 20: ";
    rbt.inorderTraversal();
    std::cout << "Tree structure after deleting 20:" << std::endl;
    rbt.printTree();

    std::cout << "\nDeleting 10..." << std::endl;
    rbt.remove(10);
    std::cout << "Inorder Traversal after deleting 10: ";
    rbt.inorderTraversal();
    std::cout << "Tree structure after deleting 10:" << std::endl;
    rbt.printTree();

    std::cout << "\nDeleting 5..." << std::endl;
    rbt.remove(5);
    std::cout << "Inorder Traversal after deleting 5: ";
    rbt.inorderTraversal();
    std::cout << "Tree structure after deleting 5:" << std::endl;
    rbt.printTree();

    std::cout << "\nInserting 40, 45" << std::endl;
    rbt.insert(40);
    rbt.insert(45);
    std::cout << "Inorder Traversal: ";
    rbt.inorderTraversal();
    std::cout << "Tree structure:" << std::endl;
    rbt.printTree();

    std::cout << "\nDeleting 25..." << std::endl;
    rbt.remove(25);
    std::cout << "Inorder Traversal after deleting 25: ";
    rbt.inorderTraversal();
    std::cout << "Tree structure after deleting 25:" << std::endl;
    rbt.printTree();

    std::cout << "\nDeleting 35..." << std::endl;
    rbt.remove(35);
    std::cout << "Inorder Traversal after deleting 35: ";
    rbt.inorderTraversal();
    std::cout << "Tree structure after deleting 35:" << std::endl;
    rbt.printTree();

    return 0;
}
