// 提供课上讲解的C语言实现单链表的代码及各种方法示例
// 由于过于基础 这里不展示更多的基础操作
// 后面可以复习一下P4中单链表的各种应用，包括但不限于反转链表、查找链表中间节点等

#include <stdio.h>
#include <stdlib.h>

// typedef在C中起到类似模板的作用，通过修改ElementType的类型，可以实现不同类型的链表。这里暂时把ElementType定义为int
typedef int ElementType;

typedef struct Node {
    ElementType data;
    struct Node *next;
} Node;

// 初始化函数
Node *initList() {
    // 在本文件中，head被认为是哨兵节点
    Node *head = (Node *)malloc(sizeof(Node));
    head->data = 0;
    head->next = NULL;
    return head;
}

// 头插法
void insertHead(Node *head, ElementType value) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->data = value;
    new_node->next = head->next;
    head->next = new_node;
}

// 获取尾结点
Node *getTail(Node *head) {
    // 这里不要直接把拷贝的head拿过来用，新创建一个current变量来缓存head的值，代码可读性更高。反正出函数都会清栈的。
    Node *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    return current;
}

// 尾插法
Node *insertTail(Node *head, ElementType value) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->data = value;
    new_node->next = NULL;
    Node *tail = getTail(head);
    tail->next = new_node;
    return new_node;    // 返回新插入的节点
}

// 遍历链表打印信息
void listNode(Node *head) {
    Node *current = head->next;
    while (current != NULL) {
        printf("%d\n", current->data);
        current = current->next;
    }
    puts("");
}

int main() {
    return 0;
}
