// 线索二叉树

// 目的：想要像遍历链表那样，轻松地线性遍历二叉树，而不是使用复杂的递归。结果上是变成了类双向循环链表的形态
// 解决方法：利用叶节点的空余空间，记录前驱、后继
// 加线索：要确定是前序/中序/后序线索
// 实现：以中序遍历为例，先写出中序遍历的序列，然后根据序列，在叶子节点上，左孩子指向前驱，右孩子指向后继

// 中序遍历线索化：
// 1. 创建一个虚拟头节点，头结点的lchild指向根
// 2. 头结点的rchild指向遍历的最后一个节点（最后一个节点指的是中序遍历的结果）
// 3. 第一个结点（中序遍历的结果，不一定是根）的lchild指向头节点
// 4. 最后一个结点的rchild指向头节点

#include <stdio.h>
#include <stdlib.h>

typedef char ElemType;

typedef struct ThreadedNode {
    ElemType data;
    struct ThreadedNode *lchild;
    struct ThreadedNode *rchild;
    int ltag;    // 0表示左孩子，1表示前驱
    int rtag;    // 0表示右孩子，1表示后继
} ThreadedNode;

// 简单说一下给指针重命名的内在逻辑
// 1. 祖宗之法不可变
// 2.
// 只要形参出现一级指针，就意味着函数内要对指针指向的值进行修改；如果没有一级指针，那就是只是使用值。如此一来，规避了二级以上指针的复杂性
typedef ThreadedNode *ThreadedTree;

// 借助字符串来构建树，#代表空位（前序序列）
char str[] = "ABDH##I##EJ###CF##G##";
int idx = 0;
ThreadedTree prev = NULL;    // 前驱节点

// 按照前序遍历的方式创建二叉树
void createTree(ThreadedTree *T) {    // 形参是二级指针。因为需要对传进来的指针进行修改
    ElemType ch = str[idx++];
    if (ch == '#') {
        *T = NULL;
    } else {
        *T = (ThreadedTree)malloc(sizeof(ThreadedNode));
        (*T)->data = ch;
        createTree(&((*T)->lchild));    // 递归创建左子树
        if ((*T)->lchild != NULL) {
            (*T)->ltag = 0;             // 左孩子存在，不能用来放前驱
        }
        createTree(&((*T)->rchild));    // 递归创建右子树
        if ((*T)->rchild != NULL) {
            (*T)->rtag = 0;             // 右孩子存在，不能用来放后继
        }
    }
}

// 针对每一个结点，设置前驱和后继。函数体内还是中序的形式
void threading(ThreadedTree T) {
    if (T != NULL) {
        threading(T->lchild);

        if (T->lchild == NULL) {
            // 这里第一次进入时实现了 3.第一个结点的lchild指向头节点（别忘了此时prev是头节点）
            T->lchild = prev;    // 如果左孩子为空，则将左孩子指向前驱
            T->ltag = 1;         // 左孩子是前驱
        }
        // 逻辑上想让T->rchild指向后继，但实际上是让前驱的rchild指向当前结点，即前驱的后继是T。因为此时还没访问到T的后继
        if (prev->rchild == NULL) {
            prev->rchild = T;    // 如果前驱的右孩子为空，则将前驱的右孩子指向当前结点
            prev->rtag = 1;      // 前驱是后继
        }
        prev = T;                // 更新前驱为当前结点

        threading(T->rchild);
    }
}

// 中序线索化
// 这个函数主要做四件事，开头的注释中有
void inOrderThreading(ThreadedTree *head, ThreadedTree T) {
    // 创建头节点
    *head = (ThreadedTree)malloc(sizeof(ThreadedTree));
    (*head)->ltag = 0;          // 左孩子指向根
    (*head)->rtag = 1;          // 右孩子指向线索（孩子是构成树的边，线索是树之外的东西）
    (*head)->rchild = *head;    // 初始化右孩子为头节点自身

    // 1. 头节点的lchild指向根
    (*head)->lchild = T;
    prev = *head;    // prev记录上一个访问的结点
    threading(T);

    // 最后一个节点线索化
    prev->rchild = *head;    // 4. 最后一个结点的后继指向头结点
    prev->rtag = 1;          // 最后一个结点的rtag设置

    // 2. 头结点的rchild指向最后一个结点
    (*head)->rchild = prev;
}

// 中序遍历
void inOrder(ThreadedTree head) {
    ThreadedTree curr = head->lchild;    // 从根节点开始遍历
    while (curr != head) {               // 树被线索化成了双向循环链表，最后会回到头节点
        // 找左儿子
        while (curr->ltag == 0) {
            curr = curr->lchild;         // 中序遍历，先找最左端
        }

        printf("%c ", curr->data);

        // 找线索
        while (curr->rtag == 1 && curr->rchild != head) {
            curr = curr->rchild;    // 如果右孩子是后继，则直接访问后继
            printf("%c ", curr->data);
        }
        // 如果右孩子不是后继，则访问右孩子
        curr = curr->rchild;
    }
    puts("");
}

int main() {
    ThreadedTree head;    // 虚拟头节点
    ThreadedTree T;
    
    // 创建二叉树
    createTree(&T);
    // 线索化
    inOrderThreading(&head, T);
    // 遍历线索二叉树
    inOrder(head);

    return 0;
}
