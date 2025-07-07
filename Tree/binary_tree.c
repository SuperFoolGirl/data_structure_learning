#include <stdio.h>
#include <stdlib.h>

typedef char ElemType;

typedef struct TreeNode {
    ElemType data;
    struct TreeNode *lchild;
    struct TreeNode *rchild;
} TreeNode;

typedef TreeNode *BiTree;

// 前序遍历
void preOrder(BiTree T) {
    if (T == NULL) {
        return;
    }
    printf("%c ", T->data); // 访问根节点
    preOrder(T->lchild);    // 递归遍历左子树
    preOrder(T->rchild);    // 递归遍历右子树
}

// 中序遍历
void inOrder(BiTree T) {
    if (T == NULL) {
        return;
    }
    inOrder(T->lchild);     // 递归遍历左子树
    printf("%c ", T->data); // 访问根节点
    inOrder(T->rchild);     // 递归遍历右子树
}

// 后序遍历
void postOrder(BiTree T) {
    if (T == NULL) {
        return;
    }
    postOrder(T->lchild);   // 递归遍历左子树
    postOrder(T->rchild);   // 递归遍历右子树
    printf("%c ", T->data); // 访问根节点
}