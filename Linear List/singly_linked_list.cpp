// 单链表封装成类
// 没有使用迭代器
// 头节点为哨兵节点，tail指向最后一个节点
// tail指针使得尾插和访问尾结点操作效率为O(1)

#include <iostream>

using namespace std;

template<typename T>
class List {
private:
    // 嵌套结构体，外部无法访问
    struct Node {
        T data;
        Node *next;

        Node(T val) : data(val), next(nullptr) {}
    };

    Node *head;    // 哨兵
    Node *tail;    // 非哨兵，指向最后一个节点
    int the_size;

public:
    List() : the_size(0) {
        init();
    }

    ~List() {
        clear();
        delete head;
    }

    List(const List &rhs) {
        init();
        *this = rhs;
    }

    List &operator=(const List &rhs) {
        if (this != &rhs) {
            this->clear();
            this->the_size = rhs.the_size;

            Node *r_cur = rhs.head->next;
            Node *cur = this->head;
            while (r_cur != nullptr) {
                cur->next = new Node(r_cur->data);
                cur = cur->next;
                tail = cur;
                r_cur = r_cur->next;
            }
        }
        return *this;
    }

    void clear() {
        Node *cur = head->next;
        while (cur != nullptr) {
            Node *del = cur;
            cur = cur->next;
            delete del;
        }
        head->next = nullptr;
        tail = nullptr;
        the_size = 0;
    }

    // 节点从1开始索引，插到第idx个节点之后
    void insert(int idx, const T &value) {
        if (idx < 0 || idx > the_size) {
            // throw runtime_error("插入位置不合法");
            return;
        }
        Node *new_node = new Node(value);
        Node *prev = head;
        for (int i = 0; i < idx; ++i) {
            prev = prev->next;
        }
        new_node->next = prev->next;
        prev->next = new_node;

        if (idx == the_size) {    // 更新tail
            tail = new_node;
        }
        the_size++;
    }

    void push_front(const T &value) {
        insert(0, value);
    }

    void push_back(const T &value) {
        Node *new_node = new Node(value);

        // 特判一下，如果链表为空，直接插入到头节点后面
        if (empty()) {
            head->next = new_node;
            tail = new_node;
        } else {
            // 其他情况下，调用insert效率太低，因此不用
            tail->next = new_node;
            tail = new_node;
        }
        the_size++;
    }

    // 节点从1开始索引，删除第idx个节点
    void remove(int idx) {
        if (idx < 1 || idx > the_size) {
            // throw runtime_error("删除位置不合法");
            return;
        }

        Node *prev = head;
        for (int i = 1; i < idx; ++i) {
            prev = prev->next;
        }

        Node *del = prev->next;

        // 检查是否是删除最后一个节点
        if (del == tail) {
            tail = prev;
        }

        prev->next = del->next;
        delete del;
        the_size--;

        // 如果删除后表空则更新tail，避免指向head
        if (the_size == 0) {
            tail = nullptr;
        }
    }

    bool empty() const {
        return the_size == 0;
    }

    int size() const {
        return the_size;
    }

    T &front() {
        if (empty()) {
            throw runtime_error("链表为空");
        }
        return head->next->data;
    }

    const T &front() const {
        if (empty()) {
            throw runtime_error("链表为空");
        }
        return head->next->data;
    }

    T &back() {
        if (empty()) {
            throw runtime_error("链表为空");
        }
        return tail->data;
    }

    const T &back() const {
        if (empty()) {
            throw runtime_error("链表为空");
        }
        return tail->data;
    }

    void print() const {
        Node *cur = head->next;
        while (cur != nullptr) {
            cout << cur->data << " ";
            cur = cur->next;
        }
    }

private:
    void init() {
        head = new Node(T());
        tail = nullptr;
        head->next = nullptr;
    }
};

int main() {
    List<int> list;
    int n, m, x, op1, op2, op3;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) {
        scanf("%d", &x);
        list.push_back(x);
    }
    scanf("%d", &m);
    for (int i = 0; i < m; i++) {
        scanf("%d", &op1);
        if (op1 == 0) {
            scanf("%d%d", &op2, &op3);
            list.insert(op2, op3);
        } else {
            scanf("%d", &op2);
            list.remove(op2);
        }
    }
    list.print();

    return 0;
}
