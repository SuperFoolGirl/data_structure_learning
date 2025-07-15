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
// 2. 插入结点的叔叔结点（父亲的兄弟结点）是红色，叔父爷变色，然后以爷爷为“插入节点”，进行新的判断，直到满足红黑树性质
//    （实际上，此时父亲和叔叔结点一定是红色，插入结点的祖父结点一定是黑色）
// 3. 插入结点的叔叔结点是黑色（可能是NIL结点），需要判断四种类型，法则与AVL树相同：
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
        // 新插入的结点默认为红色
        Node(T val) : data(val), color(RED), parent(nullptr), left(nullptr), right(nullptr) {}
    };

    Node *root;
    // 如果一个节点的左子节点或右子节点不存在，那么相应的指针会指向NIL节点。
    // 本来应该指向nullptr，但为了方便处理，我们使用一个特殊的NIL节点来代替空指针。
    Node *NIL;

    // 辅助函数：获取结点颜色
    Color getColor(Node *node) const {
        if (node == nullptr || node == NIL) {
            return BLACK;    // 空结点或NIL结点视为黑色
        }
        return node->color;
    }

    // 辅助函数：设置结点颜色
    void setColor(Node *node, Color color) {
        // 叶子结点不允许设置颜色
        if (node != nullptr && node != NIL) {
            node->color = color;
        }
    }

    // 辅助函数：获取父结点
    Node *parent(Node *node) const {
        if (node == nullptr || node == root) {
            return nullptr;
        }
        // 如果 node 是 NIL，它自己的 parent 成员应该指向其逻辑父节点
        // 参考一下transplant方法对 NIL 的处理
        return node->parent;
    }

    // 辅助函数：获取祖父结点
    Node *grandparent(Node *node) const {
        return parent(parent(node));
    }

    // 辅助函数：获取兄弟结点
    Node *sibling(Node *node) const {
        Node *p = parent(node);
        // 根节点没有父节点（或其父节点为 nullptr），因此没有兄弟
        if (p == nullptr) {
            return nullptr;
        }

        // 判断 node 是父节点的左孩子还是右孩子，然后返回另一个孩子
        // 注意：p->left 或 p->right 可能是 NIL
        if (node == p->left) {
            return p->right;
        } else {    // node == p->right
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
    //         curr                      right_son
    //        /    \                    /       \
    //      L     right_son          curr     RR_son
    //           /     \            /   \
    //         RL_son   RR_son     L   RL_son
    // 由于加了parent指针，所有的修改都是双向的
    // a要成为b的子树，那就必然要对应地更改a的parent指针
    void leftRotate(Node *curr) {
        Node *right_son = curr->right;    // right_son 将成为新的子树根

        // 这一对就是双向修改：将right_son的left改为curr的right，那就要对应修改right_son的left的parent指针
        curr->right = right_son->left;    // 将 right_son 的左子树挂到 curr 的右边
        // AVL树的结点没有父亲指针，所以当时不需要修改下面的parent指针
        if (right_son->left != NIL) {
            right_son->left->parent = curr;    // 更新 right_son 左子树的父指针
        }

        // 必须先修改curr的parent指针，最后才能修改curr本身与right_son的关系
        // 下面这段代码，在AVL的实现中，是通过指针引用，将right_son赋值给curr来实现的
        // 即让right_son取代curr在树结构中的位置
        // 但这里是通过修改curr的parent指针的left/right指针，来修改树结构的相对关系的
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
    //            curr               left_son
    //           /    \             /       \
    //      left_son   R         LL_son     curr
    //     /       \                       /   \
    //    LL_son   LR_son               LR_son  R
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
    // 如果curr的父节点是黑色，则为正常插入，无需修复
    // 当curr的父结点是红色时才需要修复 (“不红红”)
    // 根结点必须是黑色 (“根叶黑”)
    // 简单回顾一下修复插入的三种情况
    // 1. 根设为黑； 2. 叔叔是红色，父亲和叔叔变黑，祖父变红； 3. 叔叔是黑色，进行旋转
    void insertFixup(Node *curr) {
        // 如果父亲是红色的，就要循环持续地修改
        while (curr != root && getColor(parent(curr)) == RED) {
            Node *p = parent(curr);
            Node *gp = grandparent(curr);
            Node *u = uncle(curr);

            // 情况1: 叔叔节点是红色 (父节点和叔叔节点都是红色)
            if (getColor(u) == RED) {
                // 变色操作
                setColor(p, BLACK);    // 父节点变黑
                setColor(u, BLACK);    // 叔叔节点变黑
                setColor(gp, RED);     // 祖父节点变红

                // 将 curr 指向祖父节点，继续向上检查
                // 因为祖父节点变红后，它可能与其父节点（如果存在且是红色）形成新的双红情况
                curr = gp;
            }
            // 情况2: 叔叔节点是黑色 (包括NIL节点，因为NIL总是黑色)
            else {
                // 细分：父节点是祖父的左子还是右子
                if (p == gp->left) {
                    // 情况2.1: 父节点是祖父的左子 (左子树插入)

                    // 情况2.1.1: curr 是父节点的右子 (LR情况)
                    if (curr == p->right) {
                        leftRotate(p);
                        // 对父节点进行左旋（转换为LL情况）
                        // 旋转后，p 指向的还是原来的父节点，而 curr 指向的还是原来的子节点
                        // 因为旋转函数是指针的值传递。内部没有改变指针的指向，只是改变了树结构

                        // 这里回顾一下基础：指针赋值意味着，两个指针指向了同一个内存地址
                        // curr = p; 这行代码让 curr 指针暂时指向了旧的 p 节点。（也就是孙子）
                        // p = parent(curr);（此时 curr 已经指向了旧的 p 节点）这行代码，将p更新为旧p节点的父节点。
                        // 那么旧的 p 节点的父节点是谁呢？正是原来的 curr 节点

                        // 经过这两步，p 指向了原来的 curr 节点（它现在是 gp 的左子），而 curr 指向了原来的 p
                        // 节点（它现在是 p 指向节点的左子）。二者实际上是交换了位置，用来抵消旋转时的交换
                        // 交换的原因是，为了保持下面设置颜色时，旋转中心的一致性
                        curr = p;
                        p = parent(curr);
                        // 到这还没完，下面与情况2.1.2共享对爷爷的右旋
                    }

                    // 情况2.1.2: curr 是父节点的左子 (LL情况 或 LR转换后)
                    rightRotate(gp);    // 对祖父节点进行右旋 (修正 LL 情况)
                    // 从视频来看，LR的情况下应该是孙子作为旋转中心
                    // 但前面已经修复了p和curr的相对位置，因此这里永远可以视p为旋转中心
                    setColor(p, BLACK);    // 父节点变黑 (旋转后的新子树根)。
                    setColor(gp, RED);     // 祖父节点变红 (旋转后的新子树根的右子)
                } else {                   // p == gp->right
                    // 情况2.2: 父节点是祖父的右子 (右子树插入)

                    // 情况2.2.1: curr 是父节点的左子 (RL情况)
                    if (curr == p->left) {
                        rightRotate(p);    // 对父节点进行右旋，转换为RR情况
                        curr = p;
                        p = parent(curr);
                    }

                    // 情况2.2.2: curr 是父节点的右子 (RR情况 或 RL转换后)
                    leftRotate(gp);        // 对祖父节点进行左旋 (修正 RR 情况)
                    setColor(p, BLACK);    // 父节点变黑
                    setColor(gp, RED);     // 祖父节点变红
                }
            }
        }
        // 最后调整“根叶黑”
        setColor(root, BLACK);    // 确保根结点始终是黑色
    }

    // 插入操作 (私有辅助函数)
    void insert(Node *node) {
        // 遍历时，用循环来代替递归
        // 因为整个函数体没法完全托付给递归
        // 比如下面的结点修复，这部分逻辑是希望插入后修复，不参与递归
        // 因此可以看到，递归需要整个函数体的参与，而循环只是局部

        // parent是node的父结点。可以类比为单向链表记录前驱，遍历时二者同步向前
        // 因为遍历到NIL时，就失去父节点了，所以这里才需要记录
        Node *parent = nullptr;
        Node *curr = root;    // curr用于遍历树

        while (curr != NIL) {
            parent = curr;
            if (node->data < curr->data) {
                curr = curr->left;
            } else if (node->data > curr->data) {    // 防止插入重复元素
                curr = curr->right;
            } else {                                 // 元素重复，不插入新结点，直接删除新创建的结点并返回
                delete node;                         // 这个node是外部开辟在堆区的
                return;
            }
        }

        node->parent = parent;       // 设置 node 的父结点
        if (parent == nullptr) {
            root = node;             // 如果树为空，node 成为根结点
        } else if (node->data < parent->data) {
            parent->left = node;     // node 成为 parent 的左子
        } else {                     // node->data > parent->data
            parent->right = node;    // node 成为 parent 的右子
        }

        node->left = NIL;            // 新插入结点的左右子都指向NIL
        node->right = NIL;           // 确保新插入结点是红色，其子结点NIL是黑色 (满足性质4)
        setColor(node, RED);         // 新插入结点默认为红色 (可能违反“不红红”和“根叶黑”)

        insertFixup(node);           // 修复红黑树性质
    }

    // 查找最小结点
    Node *minimum(Node *node) const {
        // “提前一个”的思路在链表中已经见过了，这里要深刻体会
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
    // 又是之前提到的，双向更新的思想
    void transplant(Node *u, Node *v) {
        if (u->parent == nullptr) {
            root = v;                // u 是根结点，v 成为新根
        } else if (u == u->parent->left) {
            u->parent->left = v;     // u 是父结点的左子，v 成为左子
        } else {
            u->parent->right = v;    // u 是父结点的右子，v 成为右子
        }

        // 更新 v 的父指针
        // 即使v是NIL，也要更新它的父指针，因为调用parent等方法时，需要用到其逻辑上的父节点。这里可以看一下parent方法
        // 虽然NIL是共享的，但这里借用一下也无妨。当NIL节点再次被用作其他节点的叶子时，它的parent指针会根据最新的上下文被重新设置
        // 如果不允许NIL更新，部分场景下会报错
        v->parent = u->parent;
    }

    // 删除修复 (Fix-up after deletion)
    void deleteFixup(Node *x) {
        // 循环条件：x 不是根节点 且 x 是黑色 (双黑)

        // 这个循环可以把下面几种特殊情况挡在外面
        // 1. 待删除结点只有一个红色孩子，为黑-红类型，把红传进来直接下去改成黑即可
        // 2. 双黑上移到红色结点，直接下去改成黑
        // 3. 双黑上移到根结点，直接改成黑即可
        // 4. 任务完毕，直接将x指向根结点，下次循环直接跳出
        while (x != root && getColor(x) == BLACK) {
            // NIL也要进入循环，parent方法和sibling方法都能处理NIL
            Node *p = parent(x);
            Node *s = sibling(x);    // 兄弟节点

            // --- 情况2：双黑结点的兄弟是红色 ---
            // (在进入这个分支时，s 必然是存在的)
            if (getColor(s) == RED) {
                setColor(s, BLACK);    // 兄弟变黑
                setColor(p, RED);      // 父节点变红
                if (x == p->left) {    // x 是左孩子，s 是右孩子 (RR型)
                    leftRotate(p);     // 对父节点左旋
                } else {               // x 是右孩子，s 是左孩子 (LL型)
                    rightRotate(p);    // 对父节点右旋
                }
                // 旋转后，新的兄弟是黑色的，进入新一轮判断
                s = sibling(x);    // 更新兄弟节点 (现在是黑色的)
            }

            // --- 情况1：双黑结点的兄弟是黑色 ---
            // (经过上面 if 块转换后，s 现在必然是黑色的)
            // 1.2) 如果兄弟的两个孩子都是黑色 (包括 NIL 孩子)，兄弟变红+双黑上移
            if (getColor(s->left) == BLACK && getColor(s->right) == BLACK) {
                setColor(s, RED);    // 兄弟 s 变红色
                // 双黑转移：父亲 p 变成新的双黑节点
                x = p;
                // 这里循环会继续，直到 x 成为红色或根节点，或者在下次循环中找到红色兄弟或红色侄子
                // 原本的NIL和单黑结点，在把双黑转移上去后，自己会自然恢复到相应的准则，无需代码纠正
            }
            // 1.1) 如果兄弟至少有一个红色孩子
            else {
                // 细分兄弟左右子的情况
                if (s == p->right) {    // x 是左孩子，s 是右孩子
                    // RL型 或 RR型
                    if (getColor(s->left) == RED) {    // 兄弟 s 的左孩子是红色（RL型）
                        setColor(s->left, BLACK);      // s 的左孩子变黑
                        setColor(s, RED);              // s 变红
                        rightRotate(s);                // 对 s 右旋
                        s = p->right;                  // 更新 s 为中间位置
                    }
                    // RR型 (或 LR 转换后)
                    // 这里颜色变换与视频中的算法不同，但最终变换完后是等价的。下面也是同理
                    setColor(s, getColor(p));     // s 继承父节点 p 的颜色
                    setColor(p, BLACK);           // 父节点 p 变黑
                    setColor(s->right, BLACK);    // s 的右孩子变黑
                    leftRotate(p);                // 对父节点 p 左旋
                    x = root;                     // 旋转后平衡已恢复，直接将 x 指向根节点，跳出循环
                } else {                          // x 是右孩子，s 是左孩子
                    // LR型 或 LL型
                    if (getColor(s->right) == RED) {    // 兄弟 s 的右孩子是红色（LR型）
                        setColor(s->right, BLACK);      // s 的右孩子变黑
                        setColor(s, RED);               // s 变红
                        leftRotate(s);                  // 对 s 左旋
                        s = p->left;                    // 更新 s 为中间位置
                    }
                    // LL型 (或 LR 转换后)
                    setColor(s, getColor(p));    // s 继承父节点 p 的颜色
                    setColor(p, BLACK);          // 父节点 p 变黑
                    setColor(s->left, BLACK);    // s 的左孩子变黑
                    rightRotate(p);              // 对父节点 p 右旋
                    x = root;                    // 旋转后平衡已恢复，直接将 x 指向根节点，跳出循环
                }
            }
        }
        // 循环结束后，如果 x 是红色（之前是双黑，现在变成单黑），将其染黑
        setColor(x, BLACK);
    }

    // 删除操作 (私有辅助函数)
    // 删除后，可能会违反“黑路同”

    // 基于BST的删除，只需要考虑两种情况：

    // 1. 有一个孩子，直接用红孩子替换，然后变黑
    // 只有两种情况：一黑一红，红要么在左边，要么在右边。红替代黑后，要变黑
    // 原因：红结点不存在只有一个孩子的情况，因为要满足黑路同，且红结点只能有黑儿子

    // 2. 没有孩子
    //    a. 如果是红色结点，直接删除
    //    b. 如果是黑色结点，删除后会导致所有经过它的路径上的黑色结点数会减少1。故将其变为“双黑结点”，
    //       最终目的：将双黑恢复为单黑，三条路径：1.1), 1.2.3), 1.2.4)
    //       这里提醒一下，对于“双黑结点”，要看其兄弟s。其兄弟s是必然存在的，否则无法满足“黑路同”

    //       1) 如果“双黑结点”的兄弟s是黑色

    //          1.1) 如果黑色兄弟至少有一个红色孩子r，根据psr(parent-sibling-red)，来进行旋转/变色。p是“双黑节点”和s的父亲
    //               完成变色和旋转后，原“双黑节点”设为NIL即可，即变为单黑
    //               1.1.1) LL型：s是p的左孩子，且s的左孩子r是红色（包括s有两个红孩子的情况）
    //                      变色：r变s，s变p，p变黑
    //                      旋转：p右旋
    //               1.1.2) RR型：s是p的右孩子，且s的右孩子r是红色（包括s有两个红孩子的情况）
    //                      变色：r变s，s变p，p变黑
    //                      旋转：p左旋
    //               1.1.3) LR型：s是p的左孩子，s只有右孩子r且为红色
    //                      变色：r变p，p变黑
    //                      旋转：s左旋，p右旋（对p执行左右旋）
    //               1.1.4) RL型：s是p的右孩子，s只有左孩子r且为红色
    //                      变色：r变p，p变黑
    //                      旋转：s右旋，p左旋（对p执行左右旋）

    //          1.2) 如果兄弟的两个孩子都是黑色（注意，这里包括了兄弟没有孩子的情况，此时别忘了两个NIL作为黑儿子）
    //               兄弟s变成红色，然后双黑上移至父亲

    //               双黑上移需要持续执行，直到双黑结点彻底消除；情况参考如下：
    //               # 转移到自己身上后，还需要继续双黑上移
    //               1.2.1) 第一轮中将删除结点变成双黑，双黑转移给父亲后，自己就是NIL了
    //               1.2.2) 如果自己本来是黑色结点，被儿子双黑转移了；双黑转移给自己的父亲后，自己重新回到黑色结点
    //               # 转移到自己身上后，直接消除双黑：
    //               1.2.3) 如果父亲本来是红色结点，被转移为双黑结点，则直接变为黑色结点
    //               1.2.4) 如果父亲本来是根结点，被转移为双黑结点，则直接变为黑色结点

    //       2) 如果“双黑结点”的兄弟是红色（需要转移为1)）
    //          兄弟s和父亲p变色，p沿着双黑结点的方向旋转（双黑结点是左子就p左旋，是右子就p右旋）
    //          完成后，双黑结点有了新的兄弟，进入新一轮判断；直到兄弟s是黑色，进入1)的判断

    void remove(Node *nodeToRemove) {
        // y 是实际从树中移除或移动位置的节点（nodeToRemove 或者其后继）
        Node *y = nodeToRemove;
        // x 是 y 的替代者，将用于 deleteFixup 的参数，它会是可能导致问题的点
        Node *x;    // 把y删去后，用x替代y的位置

        // 记录 y 的原始颜色，用于判断是否需要进行修复
        Color y_original_color = getColor(y);

        // Case 1: nodeToRemove 只有一个左孩子或者没有孩子
        if (nodeToRemove->left == NIL) {
            x = nodeToRemove->right;    // x 可能是红孩子或 NIL
            transplant(nodeToRemove, nodeToRemove->right);
        }
        // Case 2: nodeToRemove 只有一个右孩子
        else if (nodeToRemove->right == NIL) {
            x = nodeToRemove->left;    // x 可能是红孩子或 NIL
            transplant(nodeToRemove, nodeToRemove->left);
        }
        // Case 3: nodeToRemove 有两个孩子
        else {
            // 找到 nodeToRemove 右子树的最小节点 (nodeToRemove 的后继)
            y = minimum(nodeToRemove->right);
            // 更新 y 的原始颜色为后继节点的颜色
            y_original_color = getColor(y);
            // x 是 y 的右孩子 (y 不可能有左孩子，因为是后继)
            x = y->right;

            // y 不是 nodeToRemove 的直接右孩子
            // 这意味着，如果y是的话，那就是一个特例
            if (parent(y) != nodeToRemove) {
                // 用 y 的右孩子替换 y 的位置
                transplant(y, y->right);
                // y 接管 nodeToRemove 的右子树
                y->right = nodeToRemove->right;
                y->right->parent = y;
            }

            // 用 y 替换 nodeToRemove 的位置
            transplant(nodeToRemove, y);
            // y 接管 nodeToRemove 的左子树
            y->left = nodeToRemove->left;
            y->left->parent = y;
            // y 继承 nodeToRemove 的颜色
            setColor(y, getColor(nodeToRemove));
        }

        // 如果实际移除或替换的节点 y 的原始颜色是黑色，才需要调用 deleteFixup
        if (y_original_color == BLACK) {
            // 修正删除后的红黑树性质，x是双黑结点
            deleteFixup(x);
        }

        // 释放被删除节点的内存
        // 在 C++ 中，如果 Node 是动态分配的，这里需要 delete nodeToRemove;
        // 确保内存释放与 Node 的创建方式一致
        delete nodeToRemove;
        nodeToRemove = nullptr;    // 防止悬空指针
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

    // 构造函数：初始化NIL结点和根结点。这两个结点就是树的全部私有成员
    RedBlackTree() : root(nullptr) {
        // 创建NIL结点，需要一个T的默认构造函数
        try {
            NIL = new Node(T());
        } catch (const std::exception &e) {
            // Handle case where T does not have a default constructor
            std::cerr << "Error: T must have a default constructor for NIL node. " << e.what() << std::endl;
            // Optionally, rethrow or use a different initialization strategy
            throw std::runtime_error("T must have a default constructor for NIL node.");
        }

        NIL->color = BLACK;
        // 全都指向自己即可
        NIL->left = NIL;
        NIL->right = NIL;
        NIL->parent = NIL;

        root = NIL;    // Initially, the root points to NIL
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
    // 层序遍历可以直接体现出树的形状
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
    /*
                       (20: Black)
                      /          \
            (10: Black)          (30: Black)
             /       \           /        \
        (5: Red) (15: Red)   (25: Red) (35: Red)
    */

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
