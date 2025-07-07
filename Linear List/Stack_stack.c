#include <stdio.h>
#include <stdbool.h>

#define MAXSIZE 100
typedef int ElementType;

typedef struct Stack {
    ElementType data[MAXSIZE]; // 栈内存，声明即分配
    int top;    // 栈顶指针，即栈顶下标值
} Stack;

// 初始化
void initStack(Stack *s) {
    s->top = -1; // 初始化栈顶指针为-1，表示栈为空
}

// 判断是否为空
bool isEmpty(Stack *s) {
    return s->top == -1;
}

// 入栈操作
bool push(Stack *s, ElementType value) {
    if (s->top >= MAXSIZE - 1) {
        printf("Stack overflow\n");
        return false; // 栈满，无法入栈
    }
    s->data[++s->top] = value;
    return true;
}

// 出栈操作
ElementType pop(Stack *s) {
    if (isEmpty(s)) {
        printf("Stack underflow\n");
        return -1; // 栈空，无法出栈
    }
    return s->data[s->top--];
}

int main() {

    return 0;
}
