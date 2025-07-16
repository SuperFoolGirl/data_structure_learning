// B树
// 二叉搜索树、平衡二叉树、红黑树，都是先把数据加载到内存中，在内存中进行处理，数据量不会很大
// B树则是为了处理海量数据而设计的，数据量大到无法全部加载到内存中时，B树可以有效地进行硬盘I/O操作
// 先在硬盘里找到某个结点的位置，再将其信息加载到内存中
// 最下面一层结点成为叶子结点，叶子结点下面的空节点（红黑树中的NIL）称为外部结点
// 三个特性：
// 平衡：所有的叶子结点一定在同一层
// 有序：结点内的多个元素有序；且任一元素的左子树都小于它，右子树都大于它
// 多路：对于m阶B树，某结点最多m个分支，内部最多m-1个元素；
//      最少⌈m/2⌉个分支，内部至少⌈m/2⌉-1个元素（特例：根节点最少2分支1元素）

#include <algorithm>    // For std::upper_bound, std::distance
#include <iostream>
#include <vector>

// 定义B树的最小度数 (T)，即2T阶B树
// 每个非根节点至少有 T-1 个关键字，至多有 2T-1 个关键字。
// 每个非根节点至少有 T 个子节点，至多有 2T 个子节点。
const int T = 3;    // 示例阶数为 2*T = 6，6阶B树

template<typename KeyType>
class BTree {
private:
    // B树节点类 (嵌套类)
    // 再重申一下嵌套类的访问原则
    // 外部类可以访问内部类的私有成员，但内部类不能访问外部类的私有成员。
    class BTreeNode {
        friend class BTree;                   // 将Outer声明为友元类

    private:
        std::vector<KeyType> keys;            // 存储关键字
        std::vector<BTreeNode *> children;    // 存储子节点指针
        bool leaf;                            // 是否为叶子节点
        int n;                                // 当前关键字数量

    public:
        // 构造函数
        BTreeNode(bool isLeaf) : leaf(isLeaf), n(0) {
            keys.reserve(2 * T - 1); // reserve方法：预分配空间
            if (!isLeaf) {
                children.reserve(2 * T);
            }
        }

        // 析构函数
        ~BTreeNode() {
            // 注意：BTree的destroyTree函数会递归地、正确地删除所有节点
            // 这里的析构函数不再负责删除子节点，以避免重复删除和内存管理混乱。
        }

        // 打印节点内容 (辅助函数)
        void traverse() const {
            int i;
            for (i = 0; i < n; i++) {
                if (!leaf) {
                    children[i]->traverse();
                }
                std::cout << " " << keys[i];
            }
            if (!leaf) {
                children[i]->traverse();
            }
        }

        // 搜索关键字 (辅助函数)
        BTreeNode *search(KeyType k) {
            // 找到第一个大于或等于k的关键字的索引
            auto it = std::upper_bound(keys.begin(), keys.begin() + n, k);
            int i = std::distance(keys.begin(), it);

            // 如果找到关键字，直接返回当前节点
            if (i < n && k == keys[i]) {
                return this;
            }

            // 如果是叶子节点但没找到，则不存在
            if (leaf) {
                return nullptr;
            }

            // 否则，到对应的子节点中搜索
            return children[i]->search(k);
        }

        // 插入关键字k到非满节点
        void insertNonFull(KeyType k) {
            int i = n - 1;             // 从右向左查找插入位置

            if (leaf) {                // 如果是叶子节点
                keys.resize(n + 1);    // 扩展关键字空间
                while (i >= 0 && k < keys[i]) {
                    keys[i + 1] = keys[i];
                    i--;
                }
                keys[i + 1] = k;    // 插入k
                n = n + 1;          // 关键字数量加1
            } else {                // 如果不是叶子节点
                // 找到应该插入的子节点
                while (i >= 0 && k < keys[i]) {
                    i--;
                }
                i = i + 1;    // i现在是应该下降到的子节点的索引

                // 如果子节点是满的，需要分裂
                if (children[i]->n == 2 * T - 1) {
                    splitChild(i, children[i]);    // 分裂子节点
                    // 分裂后，当前节点可能会多一个关键字，需要判断k应该进入哪个新的子节点
                    if (k > keys[i]) {
                        i++;
                    }
                }
                children[i]->insertNonFull(k);    // 递归插入到子节点
            }
        }

        // 分裂子节点y (它是当前节点的第i个子节点)
        // y必须是满的 (2T-1个关键字)
        void splitChild(int i, BTreeNode *y) {
            // 创建一个新的节点z，存储y的后T-1个关键字和T个子节点
            BTreeNode *z = new BTreeNode(y->leaf);
            z->n = T - 1;

            // 复制y的后T-1个关键字到z
            for (int j = 0; j < T - 1; j++) {
                z->keys.push_back(y->keys[j + T]);
            }

            // 如果y不是叶子节点，复制y的后T个子节点到z
            if (!y->leaf) {
                for (int j = 0; j < T; j++) {
                    z->children.push_back(y->children[j + T]);
                }
                y->children.resize(T);    // 移除y中已移动的子节点
            }

            // 减小y的关键字数量
            y->n = T - 1;
            y->keys.resize(T - 1);    // 移除y中已移动的关键字

            // 移动当前节点的子节点指针，为新子节点z腾出位置
            children.insert(children.begin() + i + 1, z);

            // 将y的中间关键字 (第T-1个关键字) 提升到当前节点
            keys.insert(keys.begin() + i, y->keys[T - 1]);
            n = n + 1;    // 当前节点的关键字数量加1
        }

        // 删除关键字k的主入口（递归调用）
        void remove(KeyType k) {
            int idx = findKeyIndex(k);

            if (idx < n && keys[idx] == k) {    // 关键字 k 在当前节点中
                if (leaf) {                     // Case 1: 当前节点是叶子节点
                    removeFromLeaf(idx);
                } else {                        // Case 2: 当前节点是内部节点
                    removeFromNonLeaf(idx);
                }
            } else {                            // 关键字 k 不在当前节点中，需要在子树中查找
                if (leaf) {                     // 如果是叶子节点，但没找到关键字，则关键字不存在
                    // std::cout << "Key " << k << " not found in the B-Tree." << std::endl; // 可以根据需要打印
                    return;
                }

                bool lastChild = (idx == n);

                if (children[idx]->n < T) {
                    fill(idx);
                }

                if (lastChild && idx > n) {
                    children[idx - 1]->remove(k);
                } else {
                    children[idx]->remove(k);
                }
            }
        }

    private:
        // 删除操作辅助函数：
        int findKeyIndex(KeyType k) {
            int idx = 0;
            while (idx < n && keys[idx] < k) {
                idx++;
            }
            return idx;
        }

        void removeFromLeaf(int idx) {
            keys.erase(keys.begin() + idx);
            n--;
        }

        void removeFromNonLeaf(int idx) {
            KeyType k_to_remove = keys[idx];

            if (children[idx]->n >= T) {
                KeyType pred = getPredecessor(idx);
                keys[idx] = pred;
                children[idx]->remove(pred);
            } else if (children[idx + 1]->n >= T) {
                KeyType succ = getSuccessor(idx);
                keys[idx] = succ;
                children[idx + 1]->remove(succ);
            } else {
                merge(idx);
                children[idx]->remove(k_to_remove);
            }
        }

        KeyType getPredecessor(int idx) {
            BTreeNode *curr = children[idx];
            while (!curr->leaf) {
                curr = curr->children[curr->n];
            }
            return curr->keys[curr->n - 1];
        }

        KeyType getSuccessor(int idx) {
            BTreeNode *curr = children[idx + 1];
            while (!curr->leaf) {
                curr = curr->children[0];
            }
            return curr->keys[0];
        }

        void fill(int idx) {
            if (idx != 0 && children[idx - 1]->n >= T) {
                borrowFromPrev(idx);
            } else if (idx != n && children[idx + 1]->n >= T) {
                borrowFromNext(idx);
            } else {
                if (idx != n) {
                    merge(idx);
                } else {
                    merge(idx - 1);
                }
            }
        }

        void borrowFromPrev(int idx) {
            BTreeNode *child = children[idx];
            BTreeNode *sibling = children[idx - 1];

            child->keys.insert(child->keys.begin(), keys[idx - 1]);
            keys[idx - 1] = sibling->keys[sibling->n - 1];

            if (!child->leaf) {
                child->children.insert(child->children.begin(), sibling->children[sibling->n]);
                sibling->children.pop_back();
            }

            child->n++;
            sibling->n--;
            sibling->keys.pop_back();
        }

        void borrowFromNext(int idx) {
            BTreeNode *child = children[idx];
            BTreeNode *sibling = children[idx + 1];

            child->keys.push_back(keys[idx]);
            keys[idx] = sibling->keys[0];

            if (!child->leaf) {
                child->children.push_back(sibling->children[0]);
                sibling->children.erase(sibling->children.begin());
            }

            child->n++;
            sibling->n--;
            sibling->keys.erase(sibling->keys.begin());
        }

        void merge(int idx) {
            BTreeNode *child = children[idx];
            BTreeNode *sibling = children[idx + 1];

            child->keys.push_back(keys[idx]);

            for (KeyType k_val : sibling->keys) {
                child->keys.push_back(k_val);
            }

            if (!child->leaf) {
                for (BTreeNode *c_ptr : sibling->children) {
                    child->children.push_back(c_ptr);
                }
            }

            keys.erase(keys.begin() + idx);
            children.erase(children.begin() + idx + 1);
            n--;

            child->n += sibling->n + 1;

            delete sibling;
        }
    };    // BTreeNode class ends here

private:
    BTreeNode *root;    // 树的根节点

public:
    // 构造函数
    BTree() : root(nullptr) {}

    // 析构函数
    ~BTree() {
        destroyTree(root);
    }

    // 插入关键字
    void insert(KeyType k) {
        if (root == nullptr) {
            root = new BTreeNode(true);
            root->keys.push_back(k);
            root->n = 1;
        } else {
            if (root->n == 2 * T - 1) {
                BTreeNode *s = new BTreeNode(false);
                s->children.push_back(root);
                s->splitChild(0, root);
                int i = 0;
                if (s->keys[0] < k) {
                    i++;
                }
                s->children[i]->insertNonFull(k);
                root = s;
            } else {
                root->insertNonFull(k);
            }
        }
    }

    // 搜索关键字
    bool search(KeyType k) const {
        if (root == nullptr) {
            return false;
        }
        return root->search(k) != nullptr;
    }

    // 遍历B树
    void traverse() const {
        if (root != nullptr) {
            std::cout << "BTree Traversal:";
            root->traverse();
            std::cout << std::endl;
        } else {
            std::cout << "B树为空。" << std::endl;
        }
    }

    // 删除关键字
    void remove(KeyType k) {
        if (!root) {
            std::cout << "B树为空，无法删除关键字 " << k << std::endl;
            return;
        }

        root->remove(k);

        // 删除后检查根节点的情况
        if (root->n == 0) {
            BTreeNode *old_root = root;
            if (!root->leaf) {
                root = root->children[0];
            } else {
                root = nullptr;
            }
            delete old_root;
        }
    }

private:
    // 递归释放内存
    void destroyTree(BTreeNode *node) {
        if (node) {
            if (!node->leaf) {
                for (BTreeNode *child : node->children) {
                    destroyTree(child);    // 递归删除子树
                }
            }
            delete node;                   // 删除当前节点
        }
    }
};

// --- 示例用法 ---
int main() {
    // 示例1: 使用整数类型
    std::cout << "--- BTree<int> 示例 ---" << std::endl;
    BTree<int> intTree;
    std::vector<int> int_values = {10, 20, 5, 6, 12, 30, 7, 17, 2, 4, 1, 8, 11, 15, 18, 25, 28, 35, 38, 40};
    for (int val : int_values) {
        std::cout << "插入 " << val << ": ";
        intTree.insert(val);
        intTree.traverse();
    }
    std::cout << std::endl;

    std::cout << "搜索 11: " << (intTree.search(11) ? "存在" : "不存在") << std::endl;
    std::cout << "搜索 99: " << (intTree.search(99) ? "存在" : "不存在") << std::endl;
    std::cout << std::endl;

    std::cout << "删除 6:" << std::endl;
    intTree.remove(6);
    intTree.traverse();
    std::cout << "删除 1:" << std::endl;
    intTree.remove(1);
    intTree.traverse();
    std::cout << "删除 30:" << std::endl;
    intTree.remove(30);
    intTree.traverse();
    std::cout << "删除 12:" << std::endl;
    intTree.remove(12);
    intTree.traverse();
    std::cout << "删除 17:" << std::endl;
    intTree.remove(17);
    intTree.traverse();
    std::cout << "删除 2:" << std::endl;
    intTree.remove(2);
    intTree.traverse();
    std::cout << "删除 25:" << std::endl;
    intTree.remove(25);
    intTree.traverse();
    std::cout << "删除所有剩余元素:" << std::endl;
    std::vector<int> remaining_int_values = {10, 20, 5, 7, 4, 8, 11, 15, 18, 28, 35, 38, 40};
    for (int val : remaining_int_values) {
        intTree.remove(val);
        intTree.traverse();
    }
    std::cout << std::endl;

    // 示例2: 使用字符串类型
    std::cout << "--- BTree<std::string> 示例 ---" << std::endl;
    BTree<std::string> stringTree;
    std::vector<std::string> string_values = {"apple", "banana", "grape", "cherry", "date", "fig", "elderberry"};
    for (const std::string &val : string_values) {
        std::cout << "插入 \"" << val << "\": ";
        stringTree.insert(val);
        stringTree.traverse();
    }
    std::cout << std::endl;

    std::cout << "搜索 \"grape\": " << (stringTree.search("grape") ? "存在" : "不存在") << std::endl;
    std::cout << "搜索 \"kiwi\": " << (stringTree.search("kiwi") ? "存在" : "不存在") << std::endl;
    std::cout << std::endl;

    std::cout << "删除 \"banana\":" << std::endl;
    stringTree.remove("banana");
    stringTree.traverse();
    std::cout << "删除 \"apple\":" << std::endl;
    stringTree.remove("apple");
    stringTree.traverse();
    std::cout << "删除 \"date\":" << std::endl;
    stringTree.remove("date");
    stringTree.traverse();
    std::cout << "删除 \"fig\":" << std::endl;
    stringTree.remove("fig");
    stringTree.traverse();
    std::cout << "删除 \"cherry\":" << std::endl;
    stringTree.remove("cherry");
    stringTree.traverse();
    std::cout << "删除 \"grape\":" << std::endl;
    stringTree.remove("grape");
    stringTree.traverse();
    std::cout << "删除 \"elderberry\":" << std::endl;
    stringTree.remove("elderberry");
    stringTree.traverse();
    std::cout << std::endl;

    return 0;
}
