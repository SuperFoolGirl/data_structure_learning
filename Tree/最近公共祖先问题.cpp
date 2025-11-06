// 题目背景：找到任意两个结点之间的路径，打印路径长度以及路径本身
// findPath：处理核心逻辑
// findLCA：寻找最近公共祖先节点，模板算法
// getDistance：计算从祖先节点到目标节点的距离，并记录路径。这里对于路径的记录比较巧妙，利用了递归回溯的特性，值得学习

#include <iostream>
#include <vector>

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

    // 获取两个节点之间的路径长度
    // 很像树的直径算法，但不完全是
    // 由树的性质可知，路径唯一且必然经过它们的最近公共祖先节点
    // 因此可以先找到最近公共祖先，再计算从祖先到两个节点的路径长度
    void findPath(int val1, int val2) {
        TreeNode* lca = findLCA(root, val1, val2);
        // 题目保证公共祖先不存在，因此忽略检查

        vector<int> path1;
        vector<int> path2;
        bool found1 = false;
        bool found2 = false;
        int dist1 = getDistance(lca, val1, 0, path1, found1);
        int dist2 = getDistance(lca, val2, 0, path2, found2);

        // 打印路径长度
        cout << dist1 + dist2 << endl;

        // 输出路径结点
        // 1.注意两个数组的最后一个元素是公共祖先，不能重复输出
        // 2.path2需要反向输出
        int size1 = path1.size();
        int size2 = path2.size();
        for (int i = 0; i < size1; ++i) {
            cout << path1[i] << " ";
        }
        for (int i = size2 - 2; i >= 0; --i) {
            cout << path2[i] << " ";
        }
        cout << endl;
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
        if (val == "0") {
            t = nullptr;
            return;
        }
        t = new TreeNode(stoi(val));
        buildTree(t->left);
        buildTree(t->right);
    }

    // 寻找最近公共祖先节点
    // 还有一种方法是转化为数组，然后双指针向上遍历，更加简单，但需要辅助空间
    // 这里麻烦在t同时代表着祖先和目标值。以root为例，描述一下递归过程：
    // 如果root是公共祖先，则left_lca和right_lca分别会在左右子树中找到val1或val2
    // 如果root不是公共祖先，则left_lca和right_lca中只有一个会非空，另一个为nullptr
    // 如果left_lca非空，说明公共祖先在root的左子树中
    // 如果最后返回nullptr，说明没找到公共祖先
    TreeNode* findLCA(TreeNode* t, int val1, int val2) {
        if (t == nullptr) {
            return nullptr;
        }
        if (t->val == val1 || t->val == val2) {
            return t;
        }

        TreeNode* left_lca = findLCA(t->left, val1, val2);
        TreeNode* right_lca = findLCA(t->right, val1, val2);

        if (left_lca && right_lca) {
            return t;    // 当前节点是最近公共祖先
        }

        // 返回非空的那个子树结果；若都为空则返回nullptr
        return (left_lca != nullptr) ? left_lca : right_lca;
    }

    // 计算从节点t到目标值val的距离
    // 目标值必然在左子树或右子树中，不含目标结点子树一定会遍历到空结点
    // depth记录当前结点t到初始结点lca的距离/边数

    // 另外，这个函数要利用递归特性，来记录路径
    // 当走到目标终点，即t->val == val时，递归开始回溯，不会再往前走了
    // 回溯过程一定就是lca到目标结点的路径，不会有别的多余结点
    // 虽然这是树的递归，但是给了返回值，有强制返回的return；不像传统树递归的自然执行
    // 如果left_dist得到了非-1的值，那么会将它返回，不会继续执行下面的getDistance
    // 好好体会这种return对递归的控制，在未来的问题中善于利用
    // 因此，控制使得只有这段回溯的路上，结点进行记录即可
    int getDistance(TreeNode* t, int val, int depth, vector<int>& path, bool& found) {
        if (t == nullptr) {
            return -1;
        }
        if (t->val == val) {
            found = true;
            path.push_back(t->val);
            return depth;
        }

        // 含返回值的左右子树遍历
        // 最终返回给root时，一个为-1，一个为正确距离
        int left_dist = getDistance(t->left, val, depth + 1, path, found);
        if (left_dist != -1) {
            // 当不为-1时，说明found一定为true
            path.push_back(t->val);
            return left_dist;
        }

        // --------------------- 中序模式 ---------------------------------
        // 这段注释仅模拟判断lca，即根结点进行判断。当前是中序模式，现在回到lca了
        // 由于目标结点只能在一边
        // 上面找完左子树了，如果找到了，那么直接返回，不需要考虑右子树会受found影响
        // 如果没找到，那么继续找右子树，found正常发挥作用
        // --------------------------------------------------------------

        int right_dist = getDistance(t->right, val, depth + 1, path, found);
        if (found) {
            path.push_back(t->val);
        }
        return right_dist;
    }

private:
    TreeNode* root;
};

int main() {
    BinaryTree tree;

    int t;
    cin >> t;
    while (t--) {
        int a, b;
        cin >> a >> b;
        tree.findPath(a, b);
    }

    return 0;
}
