#include <stdbool.h>
#include <stdio.h>

#define MAXSIZE 100

typedef int ElementType;

typedef struct {
    ElementType data[MAXSIZE];
    int front;
    int rear;
} Queue;

void initQueue(Queue *q) {
    q->front = 0; // 队头指针
    q->rear = 0;  // 队尾指针
}

bool isEmpty(Queue *q) {
    return q->front == q->rear; // 队头和队尾指针相等表示队列为空
}


// 出队
ElementType dequeue(Queue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return -1; // 队列为空，返回一个错误值
    }
    ElementType value = q->data[q->front++];
    return value;
}

// 调整队列
// 事实上，最好直接借助模运算来构造循环队列。这样效率太低了
bool adjustQueue(Queue *q) {
    if (q->front > 0) {
        int diff = q->front;
        for (int i = q->front; i < q->rear; ++i) {
            q->data[i - diff] = q->data[i];
        }
        q->front = 0;
        q->rear -= diff;
        return true;
    }
    else {
        printf("Queue is full\n");
        return false;
    }
}

// 入队
bool enqueue(Queue *q, ElementType value) {
    if (q->rear >= MAXSIZE) {
        if (!adjustQueue(q)) {
            return false; // 调整队列失败，返回false
        }
    }
    q->data[q->rear++] = value;
    return true; // 入队成功
}

// 循环队列的入队
bool enqueueLoop(Queue *q, ElementType value) {
    if ((q->rear + 1) % MAXSIZE == q->front) {
        printf("Queue is full\n");
        return false; // 队列已满
    }
    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % MAXSIZE; // 循环队列的入队
    return true;
}

// 获取队头元素
ElementType front(Queue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return -1; // 队列为空，返回一个错误值
    }
    return q->data[q->front];
}

int main() {
    return 0;
}
