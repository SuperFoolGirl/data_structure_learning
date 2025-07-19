#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef int ElementType;

typedef struct QueueNode {
    ElementType data;
    struct QueueNode *next;
} QueueNode;

// 头指针始终指向哨兵节点，尾指针指向最后一个节点
// 入队用尾插法实现
typedef struct Queue {
    QueueNode *front;    // 队头指针
    QueueNode *rear;     // 队尾指针
} Queue;

// 初始化队列
Queue *initQueue() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
    node->next = NULL;    // 哨兵节点
    node->data = 0;
    q->front = node;
    q->rear = node;
    return q;
}

// 入队
void enqueue(Queue *q, ElementType value) {
    QueueNode *new_node = (QueueNode *)malloc(sizeof(QueueNode));
    new_node->data = value;
    new_node->next = NULL;
    q->rear->next = new_node;
    q->rear = new_node;    // 更新队尾指针
}

// 出队
ElementType dequeue(Queue *q) {
    QueueNode *node = q->front->next;
    ElementType ret = node->data;
    q->front->next = node->next;
    if (q->rear == node) {
        q->rear = q->front;    // 如果出队后队列为空，更新队尾指针
    }
    free(node);
    return ret;
}
