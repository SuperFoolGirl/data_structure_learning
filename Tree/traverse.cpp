// 前序、后续、层序可以判断根节点
// 中序无法判断根节点
// 但对于中序来说，根节点的左边是左子树，右边是右子树
// 因此，中序+前序/后序可以唯一确定一棵树

// 例子：
// 先序：A B D G E C F H（根左右）
// 中序：D G B E A C H F（左根右）
// 先序的第一个节点一定是根节点，因此DGBE是左子树，CHF是右子树
// 然后递归构造下去：
// 先序中的BDGE左子树对应中序中的DGEB，先序中的CFH右子树对应中序中的HCF
// 递归地按照上面的方法构造下去，B是根节点，DG是左子树，E是右子树...

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

// 类名大驼峰
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;

    TreeNode(int value)
        : val(value)
        , left(nullptr)
        , right(nullptr) {}
};

class Tree {
public:
    // 用于存储中序遍历值及其索引，方便查找
    std::map<int, int> inorder_map_;

    TreeNode* buildTree(const std::vector<int>& preorder,
                        const std::vector<int>& inorder) {
        int vector_size = inorder.size();

        // 预处理：将中序遍历的值和索引存入哈希表
        for (int i = 0; i < vector_size; ++i) {
            this->inorder_map_[inorder[i]] = i;
        }

        // 调用核心递归函数
        return build(preorder, 0, vector_size - 1, 0, vector_size - 1);
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
        // 递归的终止条件：如果子树范围无效
        if (preorder_start > preorder_end || inorder_start > inorder_end) {
            return nullptr;
        }

        // 1. 确定根节点：前序遍历的第一个元素
        int root_value = preorder[preorder_start];
        TreeNode* root = new TreeNode(root_value);

        // 2. 找到根节点在中序遍历中的位置，以划分左右子树
        int root_index_inorder = this->inorder_map_[root_value];

        // 3. 计算左子树的大小
        int left_subtree_size = root_index_inorder - inorder_start;

        // 4. 递归构建左子树
        root->left =
            build(preorder,
                  preorder_start + 1,                    // 前序左子树的起始位置
                  preorder_start + left_subtree_size,    // 前序左子树的结束位置
                  inorder_start,                         // 中序左子树的起始位置
                  root_index_inorder - 1);               // 中序左子树的结束位置

        // 5. 递归构建右子树
        root->right = build(preorder,
                            preorder_start + left_subtree_size +
                                1,                     // 前序右子树的起始位置
                            preorder_end,              // 前序右子树的结束位置
                            root_index_inorder + 1,    // 中序右子树的起始位置
                            inorder_end);              // 中序右子树的结束位置

        return root;
    }
};

// 辅助函数：层序遍历打印树结构，方便验证
void levelOrderTraversal(TreeNode* root) {
    if (!root) {
        std::cout << "Tree is empty." << std::endl;
        return;
    }

    std::cout << "Level Order: ";
    std::queue<TreeNode*> node_queue;
    node_queue.push(root);

    while (!node_queue.empty()) {
        TreeNode* current_node = node_queue.front();
        node_queue.pop();

        if (current_node) {
            std::cout << current_node->val << " ";
            if (current_node->left) {
                node_queue.push(current_node->left);
            }
            if (current_node->right) {
                node_queue.push(current_node->right);
            }
        }
    }
    std::cout << std::endl;
}

// 辅助函数：释放内存
void deleteTree(TreeNode* root) {
    if (root) {
        deleteTree(root->left);
        deleteTree(root->right);
        delete root;
    }
}

int main() {
    // 先序：A B D G E C F H（根左右） -> 1 2 4 7 5 3 6 8
    // 中序：D G B E A C H F（左根右） -> 4 7 2 5 1 3 8 6

    std::vector<int> preorder_sequence = {1, 2, 4, 7, 5, 3, 6, 8};
    std::vector<int> inorder_sequence = {4, 7, 2, 5, 1, 3, 8, 6};

    Tree tree_builder;

    TreeNode* root =
        tree_builder.buildTree(preorder_sequence, inorder_sequence);

    std::cout << "Successfully reconstructed the tree." << std::endl;
    std::cout << "Verification:" << std::endl;

    levelOrderTraversal(root);

    // 清理内存
    deleteTree(root);

    return 0;
}
