#include <cstdio>
#include <cstdlib>
#include <queue>

typedef char ElemType;

struct TreeNode {
    ElemType data;
    TreeNode *lchild;
    TreeNode *rchild;
};

typedef TreeNode *BiTree;

// 前序遍历
void preOrder(BiTree T) {
    if (T == NULL) {
        return;
    }
    printf("%c ", T->data);    // 访问根节点
    preOrder(T->lchild);       // 递归遍历左子树
    preOrder(T->rchild);       // 递归遍历右子树
}

// 中序遍历
void inOrder(BiTree T) {
    if (T == NULL) {
        return;
    }
    inOrder(T->lchild);        // 递归遍历左子树
    printf("%c ", T->data);    // 访问根节点
    inOrder(T->rchild);        // 递归遍历右子树
}

// 后序遍历
void postOrder(BiTree T) {
    if (T == NULL) {
        return;
    }
    postOrder(T->lchild);      // 递归遍历左子树
    postOrder(T->rchild);      // 递归遍历右子树
    printf("%c ", T->data);    // 访问根节点
}

// 层序遍历，获取深度
// bfs，借助队列
int getDepth(BiTree root) {
    if (root == nullptr) {
        return 0;    // 空树深度为0
    }
    int depth = 0;
    std::queue<BiTree> q;
    q.push(root);

    while (!q.empty()) {
        int size = q.size();
        // 每次处理一层的节点，每层结点个数为size
        // 处理完一层后，深度加1
        while (size > 0) {
            BiTree curr = q.front();
            q.pop();
            if (curr->lchild != nullptr) {
                q.push(curr->lchild);
            }
            if (curr->rchild != nullptr) {
                q.push(curr->rchild);
            }
            size--;
        }
        depth++;
    }
    return depth;
}

// 提供另外一个递归求深度的方法
int getDepthRecursive(BiTree root) {
    if (root == nullptr) {
        return 0;
    }
    int left_depth = getDepthRecursive(root->lchild);
    int right_depth = getDepthRecursive(root->rchild);
    return std::max(left_depth, right_depth) + 1;
}

bool isLeaf(BiTree node) {
    return node != nullptr && node->lchild == nullptr && node->rchild == nullptr;
}

// 层序遍历的应用场景：求WPL
// WPL（加权路径长度）是指所有叶子节点的路径（深度）之和
int getWPL(BiTree root) {
    if (root == nullptr) {
        return 0;
    }
    int wpl = 0;
    int depth = 0;
    std::queue<std::pair<BiTree, int>> q;    // pair<BiTree, int> 用于存储节点和其对应的深度
    q.push(std::make_pair(root, 0));

    while (!q.empty()) {
        auto curr = q.front();
        q.pop();
        BiTree node = curr.first;
        int level = curr.second;

        if (isLeaf(node)) {
            // 叶子节点，累加WPL
            wpl += level;
        }
        if (node->lchild != nullptr) {
            q.push(std::make_pair(node->lchild, level + 1));
        }
        if (node->rchild != nullptr) {
            q.push(std::make_pair(node->rchild, level + 1));
        }
    }
    return wpl;
}
