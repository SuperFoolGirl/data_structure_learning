// 前序、后续、层序可以判断根节点
// 中序无法判断根节点
// 但对于中序来说，根节点的左边是左子树，右边是右子树
// 因此，中序+前序/后序可以唯一确定一棵树

// 例子：
// 先序：A B D G E C F H（根左右）
// 中序：D G B E A C H F（左根右）
// 先序的第一个节点一定是根节点，因此DGBE是左子树，CHF是右子树
// 然后递归构造下去：
// 先序中的BDGE左子树对应中序中的DGBE，先序中的CFH右子树对应中序中的HCF
// 递归地按照上面的方法构造下去，B是根节点，DG是左子树，E是右子树...

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <vector>

using namespace std;

class Tree {
public:
    struct TreeNode {
        int val;
        TreeNode* left;
        TreeNode* right;

        TreeNode(int value)
            : val(value)
            , left(nullptr)
            , right(nullptr) {}
    };

public:
    Tree(const vector<int>& preorder,
         const vector<int>& inorder)
        : is_valid(true) {
        root = buildTree(preorder, inorder);
        }

    ~Tree() {
        deleteTree(root);
    }

    TreeNode* buildTree(const vector<int>& preorder,
                        const vector<int>& inorder) {
        int vector_size = inorder.size();

        // 预处理：将中序遍历的值和索引存入哈希表
        // 其实就是做了个反函数
        for (int i = 0; i < vector_size; ++i) {
            this->inorder_map_[inorder[i]] = i;
        }

        // 调用核心递归函数
        return build(preorder, 0, vector_size - 1, 0, vector_size - 1);
    }

    void levelOrderTraversal() {
        levelOrderTraversal(root);
    }

private:
    // 核心递归函数
    // preorder_start, preorder_end：当前子树在前序遍历中的范围
    // inorder_start, inorder_end：当前子树在中序遍历中的范围
    TreeNode* build(const std::vector<int>& preorder,
                    int preorder_start,
                    int preorder_end,
                    int inorder_start,
                    int inorder_end) {
        // 如果前序和中序遍历无法唯一确定一棵树，直接退出
        if (!is_valid) {
            return nullptr;
        }

        // 递归的终止条件：如果子树范围无效
        if (preorder_start > preorder_end || inorder_start > inorder_end) {
            return nullptr;
        }

        // 1. 确定根节点：前序遍历的第一个元素
        int root_value = preorder[preorder_start];

        // 非法点1：根结点必须在中序映射中，否则说明不合法
        if (inorder_map_.find(root_value) == inorder_map_.end()) {
            is_valid = false;
            return nullptr;
        }

        // 2. 找到根节点在中序遍历中的位置，以划分左右子树
        int root_index = this->inorder_map_[root_value];

        // 非法点2：根结点如果不在当前子树范围内，说明不合法
        if (root_index < inorder_start || root_index > inorder_end) {
            is_valid = false;
            return nullptr;
        }

        // 3. 创建根节点
        TreeNode* root = new TreeNode(root_value);

        // 4. 计算左子树的大小
        int left_subtree_size = root_index - inorder_start;

        // 5. 递归构建左子树
        root->left =
            build(preorder,
                  preorder_start + 1,                    // 前序左子树的起始位置
                  preorder_start + left_subtree_size,    // 前序左子树的结束位置
                  inorder_start,                         // 中序左子树的起始位置
                  root_index - 1);                       // 中序左子树的结束位置

        // 6. 递归构建右子树
        root->right = build(preorder,
                            preorder_start + left_subtree_size +
                                1,                     // 前序右子树的起始位置
                            preorder_end,              // 前序右子树的结束位置
                            root_index + 1,    // 中序右子树的起始位置
                            inorder_end);              // 中序右子树的结束位置

        return root;
    }

    // 层序遍历打印树结构，方便验证
    void levelOrderTraversal(TreeNode* root) {
        if (!root) {
            cout << "Tree is empty." << endl;
            return;
        }

        cout << "Level Order: ";
        queue<TreeNode*> node_queue;
        node_queue.push(root);

        while (!node_queue.empty()) {
            TreeNode* current_node = node_queue.front();
            node_queue.pop();

            if (current_node) {
                cout << current_node->val << " ";
                if (current_node->left) {
                    node_queue.push(current_node->left);
                }
                if (current_node->right) {
                    node_queue.push(current_node->right);
                }
            }
        }
        cout << endl;
    }

    void deleteTree(TreeNode*& root) {
        if (root) {
            deleteTree(root->left);
            deleteTree(root->right);
            delete root;
            root = nullptr;
        }
    }

public:
    // 用于存储中序遍历值及其索引，方便查找
    unordered_map<int, int> inorder_map_;
    // 判断给定的前序和中序遍历能否唯一确定一棵二叉树
    bool is_valid;

private:
    TreeNode* root;
};

int main() {
    // 先序：A B D G E C F H（根左右） -> 1 2 4 7 5 3 6 8
    // 中序：D G B E A C H F（左根右） -> 4 7 2 5 1 3 8 6

    vector<int> preorder_sequence = {1, 2, 4, 7, 5, 3, 6, 8};
    vector<int> inorder_sequence = {4, 7, 2, 5, 1, 3, 8, 6};

    Tree tree(preorder_sequence, inorder_sequence);

    std::cout << "Successfully reconstructed the tree." << std::endl;
    std::cout << "Verification:" << std::endl;

    tree.levelOrderTraversal();

    return 0;
}
