#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAXSIZE 100
typedef int ElementType;

typedef struct Stack {
    ElementType *data;
    int top;
} Stack;

Stack *initStack() {
    Stack *s = (Stack *)malloc(sizeof(Stack));
    s->data = (ElementType *)malloc(MAXSIZE * sizeof(ElementType));
    s->top = -1;
    return s;
}

bool isEmpty(Stack *s) {
    return s->top == -1;
}

bool push(Stack *s, ElementType value) {
    if (s->top >= MAXSIZE - 1) {
        printf("Stack overflow\n");
        return false; // 栈满，无法入栈
    }
    s->data[++s->top] = value;
    return true;
}

ElementType pop(Stack *s) {
    if (isEmpty(s)) {
        printf("Stack underflow\n");
        return -1; // 栈空，无法出栈
    }
    return s->data[s->top--];
}

ElementType front(Stack *s) {
    if (isEmpty(s)) {
        printf("Stack is empty\n");
        return -1; // 栈空，无法获取栈顶元素
    }
    return s->data[s->top];
}


int main() {
    return 0;
}