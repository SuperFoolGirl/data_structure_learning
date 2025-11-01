// 双向循环链表
// 可用于处理进阶约瑟夫问题

#include <iostream>

using namespace std;

class DoublyCircularLinkedList {
public:
    struct Node {
        int data;
        Node* prev;
        Node* next;

        Node(int val)
            : data(val)
            , prev(nullptr)
            , next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int size_;

public:
    DoublyCircularLinkedList()
        : size_(0)
        , head(nullptr)
        , tail(nullptr) {}

    ~DoublyCircularLinkedList() {
        clear();
    }

    void pushBack(int val) {
        insert(size_, val);
    }

    void pushFront(int val) {
        insert(0, val);
    }

    void popBack() {
        erase(size_ - 1);
    }

    void popFront() {
        erase(0);
    }

    void clear() {
        if (size_ == 0) {
            return;
        }

        Node* curr = head;
        Node* to_del = nullptr;

        for (int i = 0; i < size_; ++i) {
            to_del = curr;
            curr = curr->next;    // 先移动到下一个节点
            delete to_del;        // 再删除当前节点
        }

        head = nullptr;
        tail = nullptr;
        size_ = 0;
    }

    // 插入到第pos个位置，从0开始计数
    void insert(int pos, int val) {
        if (pos < 0 || pos > size_) {
            return;
        }

        Node* new_node = new Node(val);
        if (size_ == 0) {
            // 空链表，插入第一个节点
            head = new_node;
            tail = new_node;
            new_node->next = new_node;
            new_node->prev = new_node;
        } else {
            // 非空链表
            Node* curr = head;
            if (pos < size_) {
                // 如果不是尾插，遍历找到第pos个节点
                for (int i = 0; i < pos; ++i) {
                    curr = curr->next;
                }
            } else {
                curr = head;    // 插入到末尾时，curr指向head
            }
            // 当前curr指向第pos个节点
            // 插入new_node到curr之前
            new_node->prev = curr->prev;
            new_node->next = curr;
            curr->prev->next = new_node;
            curr->prev = new_node;

            // 如果插入位置是头部或尾部，需要更新head或tail指针
            if (pos == 0) {
                head = new_node;
            } else if (pos == size_) {
                tail = new_node;
            }
        }
        size_++;
    }

    // 删除第pos个位置的节点，从0开始计数
    // 对于双向循环链表，头部和尾部的操作可以统一起来，简化代码
    void erase(int pos) {
        if (pos < 0 || pos >= size_) {
            return;
        }

        Node* curr = head;
        for (int i = 0; i < pos; ++i) {
            curr = curr->next;
        }
        Node* to_del = curr;

        // 当前curr指向第pos个节点
        // 1. 特判：只有一个节点
        if (size_ == 1) {
            delete to_del;
            head = nullptr;
            tail = nullptr;
        } else {
            // 2. 一般情况
            // 如果要删除头尾点，需要更新head或tail指针
            if (pos == 0) {
                head = head->next;
            } else if (pos == size_ - 1) {
                tail = tail->prev;
            }

            to_del->prev->next = to_del->next;
            to_del->next->prev = to_del->prev;

            delete to_del;
        }
        size_--;
    }

    int size() const {
        return size_;
    }
};
