// 以最小度为定义的B树实现
// 其阶数为2t
// 注意一个性质：对于非叶子结点，如果有n个键，则必然有n+1个子节点

#include <algorithm>    // For std::sort, std::find
#include <iostream>
#include <stdexcept>    // For std::invalid_argument
#include <vector>

// 模板类 BTree
template<typename T>
class BTree {
private:
    // 嵌套类 BTreeNode
    // 这个类表示B树中的一个节点
    class BTreeNode {
    public:
        std::vector<T> keys;                  // 存储当前节点中的键值
        std::vector<BTreeNode *> children;    // 存储子节点的指针
        bool isLeaf;                          // 标记该节点是否是叶子节点
        int t;                                // 最小度数，与BTree的t相同

        // 构造函数
        BTreeNode(int min_degree, bool leaf) : t(min_degree), isLeaf(leaf) {
            // 节点最多可以容纳 2*t - 1 个键
            // 节点最多可以有 2*t 个子节点
            // vector 默认构造函数已经做了初始化，这里不需要 reserve
        }

        // 析构函数，用于递归删除子节点以避免内存泄漏
        ~BTreeNode() {
            // 只有当节点不是叶子节点时，才需要删除其子节点
            // 在执行合并或分裂时，子节点的指针可能会被移动到其他节点
            // 确保只删除那些由当前节点“拥有”的子节点
            if (!isLeaf) {
                for (BTreeNode *child : children) {
                    // 只有当指针有效且没有被转移所有权时才删除
                    // 这需要在设计上更严谨，通常通过智能指针或明确所有权转移来解决
                    // 在此简化处理：假定children中的指针始终归属当前节点
                    delete child;
                }
            }
        }

        // 查找键值在当前节点中的索引
        // 如果找到，返回键值的索引；如果未找到，返回应该插入的位置的索引
        int findKey(const T &k) const {
            int idx = 0;
            // 遍历键值，找到第一个大于或等于 k 的键值
            while (idx < keys.size() && k > keys[idx]) {
                ++idx;
            }
            return idx;
        }

        // 遍历B树（中序遍历）
        void traverse() const {
            int i;
            // 先递归遍历第一个子树
            // keys[i]一定大于children[i]->keys的所有键
            for (i = 0; i < keys.size(); ++i) {
                if (!isLeaf) {
                    children[i]->traverse();
                }
                std::cout << keys[i] << " ";
            }
            // 遍历最后一个子树
            // 键值有n个，孩子有n+1个
            // 且第n+1个孩子的键值一定大于keys[n]
            if (!isLeaf) {
                children[i]->traverse();
            }
        }

        // 插入非满子节点
        void insertNonFull(const T &k) {
            // 初始化索引为当前节点的最大键值索引
            int i = keys.size() - 1;

            // 如果是叶子节点
            if (isLeaf) {
                // 找到插入位置
                // it会接受第一个>= k 的位置
                // lower_bound 返回一个迭代器，指向第一个>= k 的元素
                // upper_bound 返回一个迭代器，指向第一个> k 的元素
                auto it = std::lower_bound(keys.begin(), keys.end(), k);
                // vector的insert方法：k插在it前面
                // 也就是说，让k成为原先it位置的元素，也就是把k插入到it处
                // 这种插入是最符合直觉的
                keys.insert(it, k);    // 插入 k
            } else {                   // 如果不是叶子节点
                // 找到合适的子节点进行递归插入
                while (i >= 0 && keys[i] > k) {
                    --i;
                }
                // i+1 是 k 应该进入的子节点的索引
                // 检查找到的子节点是否已满
                if (children[i + 1]->keys.size() == 2 * t - 1) {
                    // 如果已满，则进行分裂操作
                    splitChild(i + 1, children[i + 1]);
                    // 分裂后，父节点可能会有新的键值，需要判断 k 应该去哪个新的子节点
                    // 原本遍历到了keys[i]< k < keys[i + 1]的位置
                    // 如果children没满，正好插入到children[i + 1]中
                    // 因为keys[i] < children[i + 1]->keys < keys[i + 1]
                    // 但满了之后，children[i + 1]中有结点上升到了keys中，且正好上升到keys[i + 1]的位置
                    // 原先的keys[i + 1]现在变成了keys[i + 2]，依次类推
                    // 因此k需要判断与刚升上来的keys[i + 1]的大小关系
                    if (keys[i + 1] < k) {
                        ++i;    // k 应该进入新分裂出来的右子节点
                    }
                }
                // 如果k大于keys中的所有元素，在while循环处会直接退出。此时i为n-1
                // 下面这条语句访问的是i+1，也就是n，恰好就是最后一个子节点，是符合B树逻辑的
                children[i + 1]->insertNonFull(k);    // 递归插入到子节点
            }
        }

        // 分裂子节点 fullChild (位于 children[i])
        // fullChild 必须是满的 (2*t - 1 个键)
        // 下面用索引来描述各位置中的关键字
        // 原fullchild：0 ~ t-2（共t-1个）
        // 提升：t-1（共1个）
        // newChild：t ~ 2t-2（共t-1个）

        // 如果当前结点不是叶子结点，那么一定会有2t个子结点，因为孩子数量总是比键数量多1
        // 原fullchild和newChild各自继承t个子结点
        // 原fullchild：0 ~ t-1（共t个）
        // newChild：t ~ 2t-1（共t个）
        void splitChild(int i, BTreeNode *fullChild) {
            // 创建一个新的节点，用于存储 fullChild 的后半部分键值和子节点
            BTreeNode *newChild = new BTreeNode(fullChild->t, fullChild->isLeaf);

            // 将 newChild 插入到 children 数组的正确位置
            // 注意下外部调用方法时的this指针，指向的是分裂结点的父节点
            // 把 newChild 插入到 children[i + 1] 的位置，正好在 fullChild 的后面（从children数组来看）
            children.insert(children.begin() + i + 1, newChild);

            // 将 fullChild 的中间键值 (t-1索引) 提升到当前节点
            // 放在 keys[i] 处，在外面也就是keys[i + 1] 处
            // 因为children[t-1](当前提升结点)的键值一定小于keys[i]（外面的keys[i + 1]），但一定大于keys[i - 1]（外面的keys[i]）
            // 因此把children[t-1]的键值提升到keys[i]处是正确的

            // 将键值提升到父节点，然后从原节点中移除
            // 正确做法：先插入父节点，再从子节点移除
            // 根据this指针，这里keys就是父节点自己的关键字。把键值提升到keys中即可
            keys.insert(keys.begin() + i, fullChild->keys[t - 1]);

            // 将 fullChild 的后 t-1 个键值移动到 newChild
            // 从 fullChild 的第 t 个键（索引 t）开始，复制 t-1 个键
            // t ~ 2t-2
            for (int j = 0; j < t - 1; ++j) {
                newChild->keys.push_back(fullChild->keys[j + t]);
            }
            // 从 fullChild 中移除已移动的键值和提升的键值
            fullChild->keys.erase(fullChild->keys.begin() + t - 1, fullChild->keys.end());

            // 如果 fullChild 不是叶子节点，则将其后 t 个子节点移动到 newChild
            if (!fullChild->isLeaf) {
                for (int j = 0; j < t; ++j) {
                    newChild->children.push_back(fullChild->children[j + t]);
                }
                // 从 fullChild 中移除已移动的子节点
                fullChild->children.erase(fullChild->children.begin() + t, fullChild->children.end());
            }
        }

        // 搜索键值 k
        BTreeNode *search(const T &k) {
            int i = 0;
            // 找到第一个大于或等于 k 的键值
            while (i < keys.size() && k > keys[i]) {
                ++i;
            }

            // 如果找到 k
            if (i < keys.size() && keys[i] == k) {
                return this;    // 返回包含 k 的节点
            }

            // 如果是叶子节点，且未找到 k
            if (isLeaf) {
                return nullptr;
            }

            // 递归搜索子节点
            return children[i]->search(k);
        }

        // 获取前驱（仅用于删除操作）
        const T &getPredecessor(int idx) {
            // 从 children[idx] (左子树) 开始，一直向右下遍历到最右边的叶子节点
            BTreeNode *curr = children[idx];
            while (!curr->isLeaf) {
                curr = curr->children[curr->keys.size()];
            }
            return curr->keys[curr->keys.size() - 1];    // 返回最右边的键值
        }

        // 获取后继（仅用于删除操作）
        const T &getSuccessor(int idx) {
            // 从 children[idx+1] (右子树) 开始，一直向左下遍历到最左边的叶子节点
            BTreeNode *curr = children[idx + 1];
            while (!curr->isLeaf) {
                curr = curr->children[0];
            }
            return curr->keys[0];    // 返回最左边的键值
        }

        // 从当前节点删除键值 k
        // 核心思路：保证递归调用前，子节点至少有 t 个键

        // 与插入相反，删除操作容易导致下溢出
        // 上溢出：键数超过最大2t - 1
        // 下溢出：键数少于最小t - 1
        // 上下溢出与子节点数目无关，而是键数。修正键数后，子结点就会自然地恢复B树的性质

        // 强调一下，keys数组类型为T，children数组类型为BTreeNode*
        // 结点的概念是基于children的，与键无关，不要把键keys当作结点
        // 为了方便，可以将keys[i]称为children[i]的父键

        // 删除操作分为两种情况，与二叉搜索树类似：
        // 1. 如果当前节点是叶子节点，直接删除键值
        //    删除后若产生下溢出，需要调整
        //    1.1. 兄弟够借（借了之后不破坏B树性质）：父下来，兄上去
        //      1.1.1. 向左借
        //      children[i]删除其某个keys后下溢出了，先把父节点keys[i-1]降级到children[i]的最前面，再把children[i-1]的最后一个keys提升到父节点keys[i-1]
        //      如果左兄弟不是叶子结点，把它最右边的子树移动到自己最左边的子树位置上
        //      1.1.2. 向右借
        //      children[i]删除其某个keys后下溢出了，先把父节点keys[i]降级到children[i]的最后面，再把children[i+1]的第一个keys提升到父节点keys[i]
        //      如果右兄弟children[i+1]不是叶结点，还需要将最左边第一棵子树，即children[i+1]->children[0]，移动到children[i]的末尾最右边子树上
        //      再强调一遍，任何结点任何时刻都要保证子节点数目n，键数目n-1；同时满足最大最小范围。通过上面的子树调整，才能满足这个结构不变
        //    1.2. 兄弟不够借（直接合并）
        //      合并可能会导致父节点下溢出，又需要循环地进行新一轮调整；如果根结点参与了合并，那么根结点就会被删除，树的高度减1，其子结点成为新的根结点
        //      1.2.1. 向左合并
        //      children[i]下溢出了，先把keys[i-1]降级到children[i-1]的最后面，再把children[i]合并到children[i-1]的最后面
        //      1.2.2. 向右合并
        //      children[i]下溢出了，先把keys[i]降级到children[i]的最后面，再把children[i+1]合并到children[i]的最后面
        // 总结一下，不管是借还是合并，如果是左，那涉及的父键不是自己的，而是左兄弟的父键；如果是右，那涉及的父键就是自己的

        // 2. 如果当前节点是非叶子节点
        //    注意，非叶子结点子树数量的范围必然在最小和最大之间，且数量上一定比键数多1
        //    不再像二叉树那样，区分单子树和双子树。只要有孩子，就用统一的寻找前驱后继的策略
        //    找到前驱或后继来替换当前键值，然后递归删除前驱或后继。一定会落在叶子节点上
        //    * 寻找前驱后继方法：
        //      已知keys[i]一定大于children[i]->keys的所有键
        //      因此对于keys[i]，其前驱是children[i]->keys的最大值，后继是children[i+1]->keys的最小值
        void remove(const T &k) {
            int idx = findKey(k);
            // 这个方法是递归调用的，从根结点出发，递归到待删除结点
            // 下溢出处理的时机，与视频中讲得亦不太一样。。

            // 情况1：键值 k 在当前节点中
            if (idx < keys.size() && keys[idx] == k) {
                if (isLeaf) {                          // 情况1a：当前节点是叶子节点
                    keys.erase(keys.begin() + idx);    // 直接删除键
                } else {                               // 情况1b：当前节点是非叶子节点，需要找前驱/后继
                    // 如果左子树 (children[idx]) 至少有 t 个键
                    // 这里的思路是这样的：如果想要取前驱/后继，必须保证被取的结点，取了之后人家本身不会下溢
                    // 但视频里的思路是不管三七二十一先取到，然后后面再处理叶子结点的下溢
                    // 因此诞生了下面的else分支：合并
                    if (children[idx]->keys.size() >= t) {
                        // 虽然后面要删除前驱，但并不依赖T进行删除，所以可以返回常量引用
                        const T &pred = getPredecessor(idx);    // 获取前驱
                        keys[idx] = pred;                       // 用前驱替换当前键值
                        children[idx]->remove(pred);            // 递归删除前驱
                    }
                    // 否则，如果右子树 (children[idx+1]) 至少有 t 个键
                    else if (children[idx + 1]->keys.size() >= t) {
                        const T &succ = getSuccessor(idx);    // 获取后继
                        keys[idx] = succ;                     // 用后继替换当前键值
                        children[idx + 1]->remove(succ);      // 递归删除后继
                    }
                    // 左右子树都只有 t-1 个键，则合并它们
                    else {
                        // 将 children[idx] 和 children[idx+1] 合并
                        // 当前键值 keys[idx] 也将加入到合并后的节点中
                        merge(idx);
                        // 合并后，键值 k (原 keys[idx]) 现在在合并后的子节点中，递归删除它
                        children[idx]->remove(k);
                    }
                }
            }
            // 情况2：键值 k 不在当前节点中
            else {
                if (isLeaf) {    // 情况2a：当前节点是叶子节点，且未找到 k
                    // k 不存在于树中
                    // std::cout << "The key " << k << " is not present in the tree.\n"; // 调试信息，实际可移除
                    return;
                }

                // flag 表示要进入的子节点是否是当前节点的最后一个子节点，即children[n+1]
                // 前面都不够大，k要向右走
                bool flag = (idx == keys.size());

                // 修复下溢
                // 如果子节点 (children[idx]) 只有 t-1 个键，就要填充它
                // 虽然现在还没进入这个子结点 （马上要进入）
                // 但后续这个结点可能因为删除键而下溢，所以要提前填充
                // B树奇特的代码结构，提前预防机制？
                if (children[idx]->keys.size() == t - 1) {
                    fill(idx);    // 填充该子节点以满足删除条件
                }

                // 递归删除键值
                // 如果 flag 为真且 idx 增加，说明之前的 children[idx] (原最后一个子节点)
                // 被合并了，现在 k 可能在新的最后一个子节点中 (children[idx-1])
                // 否则，k 仍在 children[idx] 中
                if (flag && idx > keys.size()) {
                    children[idx - 1]->remove(k);
                } else {
                    children[idx]->remove(k);    // 正常递归下去寻找
                }
            }
        }

        // 填充 children[idx] 子节点，使其至少有 t 个键
        void fill(int idx) {
            // 情况1：如果左兄弟 (children[idx-1]) 有多余的键，从它那里借一个
            if (idx != 0 && children[idx - 1]->keys.size() >= t) {
                borrowFromPrev(idx);
            }
            // 情况2：如果右兄弟 (children[idx+1]) 有多余的键，从它那里借一个
            else if (idx != keys.size() && children[idx + 1]->keys.size() >= t) {
                borrowFromNext(idx);
            }
            // 情况3：左右兄弟都没有多余的键，则进行合并
            else {
                if (idx != keys.size()) {    // 合并 children[idx] 和 children[idx+1]
                    // 正常向右合并
                    merge(idx);
                } else {    // 合并 children[idx-1] 和 children[idx]
                    // 最右端子结点无法向右合并，于是转化为向左合并
                    merge(idx - 1);
                }
            }
        }

        // 从左兄弟 (children[idx-1]) 借一个键到 children[idx]
        void borrowFromPrev(int idx) {
            BTreeNode *child = children[idx];
            BTreeNode *sibling = children[idx - 1];

            // 1. 将父节点的 keys[idx-1] (分隔键) 降级到 children[idx] 的最前面
            child->keys.insert(child->keys.begin(), keys[idx - 1]);

            // 2. 将 sibling 的最后一个键提升到父节点的 keys[idx-1] 位置
            keys[idx - 1] = sibling->keys.back();
            sibling->keys.pop_back();

            // 3. 如果 child 不是叶子节点，将 sibling 的最后一个子节点移动到 children[idx] 的最前面
            if (!child->isLeaf) {
                child->children.insert(child->children.begin(), sibling->children.back());
                sibling->children.pop_back();
            }
        }

        // 从右兄弟 (children[idx+1]) 借一个键到 children[idx]
        void borrowFromNext(int idx) {
            BTreeNode *child = children[idx];
            BTreeNode *sibling = children[idx + 1];

            // 1. 将父节点的 keys[idx] (分隔键) 降级到 children[idx] 的最后面
            child->keys.push_back(keys[idx]);

            // 2. 将 sibling 的第一个键提升到父节点的 keys[idx] 位置
            keys[idx] = sibling->keys[0];
            sibling->keys.erase(sibling->keys.begin());

            // 3. 如果 child 不是叶子节点，将 sibling 的第一个子节点移动到 children[idx] 的最后面
            if (!child->isLeaf) {
                child->children.push_back(sibling->children[0]);
                sibling->children.erase(sibling->children.begin());
            }
        }

        // 合并 children[idx] 和 children[idx+1]
        // 也就是说，这里只提供向右合并的方法
        // 事实上，如果两边都不能借，那向哪边合并都一样。这里为了统一，只提供了向右合并的接口
        // 可以看看调用处是怎么巧妙处理特例：最右端子结点无法向右合并，于是转化为向左合并的
        // keys[idx] 将作为分隔键也加入到合并后的节点中
        void merge(int idx) {
            BTreeNode *child = children[idx];
            BTreeNode *sibling = children[idx + 1];

            // 1. 将当前节点的 keys[idx] (分隔键) 降级到child的末尾
            child->keys.push_back(keys[idx]);

            // 2. 将 sibling 的所有键移动到 child 的末尾
            for (const T &key : sibling->keys) {
                child->keys.push_back(key);
            }

            // 3. 如果不是叶子节点，将 sibling 的所有子节点移动到 child
            if (!child->isLeaf) {
                for (BTreeNode *s_child : sibling->children) {
                    child->children.push_back(s_child);
                }
            }

            // 4. 从当前节点中移除 keys[idx]
            keys.erase(keys.begin() + idx);
            // 5. 从当前节点中移除 sibling (children[idx+1])
            children.erase(children.begin() + idx + 1);

            // 6. 删除 sibling 节点
            // 注意：sibling 的子节点指针已经转移到 child 中，所以这里直接删除 sibling 节点是安全的
            // 否则会发生双重删除
            sibling->children.clear();    // 清空 sibling 的子节点指针，避免析构时重复删除
            delete sibling;
        }
    };    // BTreeNode 嵌套类结束

    BTreeNode *root;    // B树的根节点
    int t;              // B树的最小度数

public:
    // 构造函数
    BTree(int min_degree) : t(min_degree), root(nullptr) {
        if (t < 2) {    // 最小度数 t 必须大于等于 2
            throw std::invalid_argument("Minimum degree (t) must be at least 2.");
        }
    }

    // 析构函数
    ~BTree() {
        if (root != nullptr) {
            delete root;    // 递归删除所有节点
        }
    }

    // 遍历B树
    void traverse() const {
        if (root != nullptr) {
            root->traverse();
        } else {
            std::cout << "The tree is empty.";
        }
        std::cout << std::endl;
    }

    // 搜索键值
    // 返回包含键值的节点指针，如果未找到则返回 nullptr
    BTreeNode *search(const T &k) {
        return (root == nullptr) ? nullptr : root->search(k);
    }

    // 插入键值
    void insert(const T &k) {
        if (root == nullptr) {
            // 如果树为空，创建一个新的根节点
            root = new BTreeNode(t, true);
            root->keys.push_back(k);
        } else {
            // 如果根节点已满，则进行分裂操作
            if (root->keys.size() == 2 * t - 1) {
                // 创建一个新的根节点，它将成为旧根节点的父节点
                BTreeNode *s = new BTreeNode(t, false);
                s->children.push_back(root);    // 旧根节点成为新根节点的第一个子节点
                s->splitChild(0, root);         // 分裂旧根节点 (root 现在是 s 的子节点)
                int i = 0;
                // 判断 k 应该插入到哪个子节点
                if (s->keys[0] < k) {
                    i++;
                }
                s->children[i]->insertNonFull(k);    // 递归插入到相应的子节点
                root = s;                            // 更新根节点
            } else {
                root->insertNonFull(k);              // 否则，直接插入到非满的根节点
            }
        }
    }

    // 删除键值
    void remove(const T &k) {
        if (!root) {
            std::cout << "The tree is empty. Cannot delete.\n";
            return;
        }

        root->remove(k);    // 从根节点开始删除

        // 删除后处理根结点
        // 如果原根结点合并了，那要立新王
        if (root->keys.empty()) {
            BTreeNode *oldRoot = root;
            if (root->isLeaf) {               // 如果根节点是叶子节点且没有键，表示树为空
                root = nullptr;
            } else {                          // 如果根节点不是叶子节点但没有键，其唯一的子节点将成为新的根节点
                root = oldRoot->children[0];
                oldRoot->children.clear();    // 清空旧根的子节点指针，避免双重释放
            }
            delete oldRoot;                   // 删除旧根节点
        }
    }
};

// --- 测试代码 ---
int main() {
    // 创建一个最小度数为 3 的B树
    // (即每个节点最少 2 个键，最多 5 个键，最少 3 个子节点，最多 6 个子节点)
    BTree<int> t(3);

    std::cout << "--- 插入测试 ---\n";
    std::cout << "插入 10, 20, 5, 6, 12, 30, 7, 17, 8, 3, 2, 4, 15, 25, 16, 22, 24, 23, 21, 26, 27, 28, 29, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40\n";
    t.insert(10);
    t.insert(20);
    t.insert(5);
    t.insert(6);
    t.insert(12);
    t.insert(30);
    t.insert(7);
    t.insert(17);
    t.insert(8);
    t.insert(3);
    t.insert(2);
    t.insert(4);
    t.insert(15);
    t.insert(25);
    t.insert(16);
    t.insert(22);
    t.insert(24);
    t.insert(23);
    t.insert(21);
    t.insert(26);
    t.insert(27);
    t.insert(28);
    t.insert(29);
    t.insert(31);
    t.insert(32);
    t.insert(33);
    t.insert(34);
    t.insert(35);
    t.insert(36);
    t.insert(37);
    t.insert(38);
    t.insert(39);
    t.insert(40);

    std::cout << "B树中序遍历结果：\n";
    t.traverse();    // 预期输出：2 3 4 5 6 7 8 10 12 15 16 17 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40

    std::cout << "\n--- 搜索测试 ---\n";
    int search_key = 6;
    if (t.search(search_key) != nullptr) {
        std::cout << search_key << " 存在于B树中。\n";
    } else {
        std::cout << search_key << " 不存在于B树中。\n";
    }

    search_key = 99;
    if (t.search(search_key) != nullptr) {
        std::cout << search_key << " 存在于B树中。\n";
    } else {
        std::cout << search_key << " 不存在于B树中。\n";
    }

    std::cout << "\n--- 删除测试 ---\n";
    std::cout << "删除 6...\n";
    t.remove(6);
    std::cout << "B树中序遍历结果：\n";
    t.traverse();

    std::cout << "删除 13 (不存在)...\n";
    t.remove(13);
    std::cout << "B树中序遍历结果：\n";
    t.traverse();

    std::cout << "删除 7...\n";
    t.remove(7);
    std::cout << "B树中序遍历结果：\n";
    t.traverse();

    std::cout << "删除 20 (内部节点删除，涉及前驱/后继)...\n";
    t.remove(20);
    std::cout << "B树中序遍历结果：\n";
    t.traverse();

    std::cout << "删除 10 (可能导致根节点下移)...\n";
    t.remove(10);
    std::cout << "B树中序遍历结果：\n";
    t.traverse();

    std::cout << "删除 30...\n";
    t.remove(30);
    std::cout << "B树中序遍历结果：\n";
    t.traverse();

    std::cout << "删除 12...\n";
    t.remove(12);
    std::cout << "B树中序遍历结果：\n";
    t.traverse();

    std::cout << "删除 4...\n";
    t.remove(4);
    std::cout << "B树中序遍历结果：\n";
    t.traverse();

    std::cout << "删除 25...\n";
    t.remove(25);
    std::cout << "B树中序遍历结果：\n";
    t.traverse();

    std::cout << "删除 31...\n";
    t.remove(31);
    std::cout << "B树中序遍历结果：\n";
    t.traverse();

    std::cout << "删除所有剩余元素以清空树...\n";
    // 注意：这里的删除顺序可能会影响中间过程的树结构，
    // 但最终结果是清空树。为了方便测试，将剩余元素从小到大排序删除。
    std::vector<int> remaining_elements = {2, 3, 5, 8, 15, 16, 17, 21, 22, 23, 24, 26, 27, 28, 29, 32, 33, 34, 35, 36, 37, 38, 39, 40};
    std::sort(remaining_elements.begin(), remaining_elements.end());    // 确保从小到大删除

    for (int key : remaining_elements) {
        std::cout << "删除 " << key << "...\n";
        t.remove(key);
        // t.traverse(); // 可以打开此行观察每次删除后的树结构
    }
    std::cout << "所有元素删除完毕。\n";
    std::cout << "B树中序遍历结果：\n";
    t.traverse();    // 此时树应该为空

    return 0;
}
