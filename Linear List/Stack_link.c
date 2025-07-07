// 实际就是链表

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef int ElementType;

typedef struct Stack {
    ElementType data;
    struct Stack *next;
} Stack;

Stack *initStack() {
    Stack *s = (Stack *)malloc(sizeof(Stack));
    s->data = 0; // 哨兵节点
    s->next = NULL; // 初始化为空
    return s;
}

bool isEmpty(Stack *s) {
    return s->next == NULL; // 如果next为NULL，则栈为空
}

// 根据栈逻辑，只能支持头插法。注意这里并不是把哨兵当作栈底，而是把哨兵当作栈顶的顶
void push(Stack *s, ElementType value) {
    Stack *new_node = (Stack *)malloc(sizeof(Stack));
    new_node->data = value;
    new_node->next = s->next; // 新节点的next指向当前栈顶
    s->next = new_node; // 更新栈顶为新节点
}

void pop(Stack *s) {
    if (isEmpty(s)) {
        printf("Stack underflow\n");
        return; // 栈空，无法出栈
    }
    Stack *to_delete = s->next; // 要删除的节点是当前栈顶
    s->next = to_delete->next; // 更新栈顶为下一个节点
    free(to_delete); // 释放被删除的节点
}
