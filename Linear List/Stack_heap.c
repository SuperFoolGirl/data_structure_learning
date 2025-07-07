#include <stdio.h>
#include <stdlib.h>

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



int main() {
    return 0;
}