#include <stdio.h>
#include <stdlib.h>

typedef int ElementType;

typedef struct Node {
    ElementType data;
    struct Node *prev;
    struct Node *next;
} Node;

// 初始化双向链表
Node *initList() {
    Node *head = (Node *)malloc(sizeof(Node));
    head->data = 0;    // 哨兵节点
    head->prev = NULL;
    head->next = NULL;
    return head;
}

// 打印双向链表
void printList(Node *head) {
    Node *current = head->next;    // 跳过哨兵节点
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    puts("");
}

// 销毁双向链表
void freeList(Node *head) {
    Node *current = head;
    while (current != NULL) {
        Node *next = current->next;
        free(current);
        current = next;
    }
}

// 头插法
void insertHead(Node *head, ElementType value) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->data = value;
    new_node->prev = head;
    new_node->next = head->next;

    if (head->next != NULL) {
        head->next->prev = new_node;
    }
    head->next = new_node;
}

// 获取尾结点
Node *getTail(Node *head) {
    Node *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    return current;
}

// 尾插法
void insertTail(Node *head, ElementType value) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->data = value;
    new_node->next = NULL;

    Node *tail = getTail(head);
    tail->next = new_node;
    new_node->prev = tail;
}

// 插入数据（借助前驱）
void insertNode(Node *head, int pos, ElementType value) {
    Node *current = head;
    // 正常跳出循环后，current恰好指向插入位置的前驱
    for (int i = 0; i < pos - 1; ++i) {
        current = current->next;
        if (current == NULL) {
            // 如果进入，此时current为tail->next的空节点
            printf("Position %d is out of bounds.\n", pos);
            return;
        }
    }

    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->data = value;
    new_node->prev = current;
    new_node->next = current->next;
    current->next = new_node;
    if (new_node->next != NULL) {
        new_node->next->prev = new_node;
    }
}

// 删除节点
void deleteNode(Node *head, int pos) {
    Node *current = head;
    // 依然是找前驱节点
    for (int i = 0; i < pos - 1; ++i) {
        current = current->next;
        if (current == NULL) {
            printf("Position %d is out of bounds.\n", pos);
            return;
        }
    }

    Node *to_delete = current->next;
    if (to_delete == NULL) {
        printf("No node to delete at position %d.\n", pos);
        return;
    }
    current->next = to_delete->next;
    if (to_delete->next != NULL) {
        to_delete->next->prev = current;
    }
    free(to_delete);
}

int main() {
    Node *head = initList();
    insertHead(head, 10);
    insertHead(head, 20);
    insertTail(head, 30);
    deleteNode(head, 2);    // 删除位置2的节点
    printList(head);
    freeList(head);
    return 0;
}
