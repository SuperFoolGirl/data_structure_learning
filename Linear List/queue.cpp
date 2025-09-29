#include <iostream>

using namespace std;

template <typename T>
class Queue {
private:
    T *data;
    int the_size;
    int the_capa;
    int head;
    int rear;
    static constexpr int SPARE_CAPACITY = 10;    // 常量

public:
    Queue(int size = 0) : the_size(0), the_capa(size + SPARE_CAPACITY), head(0), rear(-1) {
        data = new T[the_capa];
    }

    ~Queue() {
        delete[] data;
    }

    Queue(const Queue &rhs) {
        *this = rhs;
    }

    Queue &operator=(const Queue &rhs) {
        if (this != &rhs) {
            delete[] data;
            the_size = rhs.the_size;
            the_capa = rhs.the_capa;
            head = 0;
            rear = rhs.the_size - 1;

            data = new T[the_capa];
            for (int i = 0, j = rhs.head; i < the_size; i++, j = (j + 1) % rhs.the_capa) {
                data[i] = rhs.data[j];
            }
        }
        return *this;
    }

    void push(const T &x) {
        if (the_size == the_capa) {
            resize(2 * the_capa);
        }
        rear = (rear + 1) % the_capa;
        data[rear] = x;
        the_size++;
    }

    int pop() {
        if (empty()) {
            // throw out_of_range("队列空，无法删除元素");
            return -1;
        }
        int val = data[head];
        head = (head + 1) % the_capa;
        the_size--;
        return val;
    }

    T &front() {
        if (empty()) {
            throw out_of_range("队列空，无法访问队首元素");
        }
        return data[head];
    }

    const T &front() const {
        if (empty()) {
            throw out_of_range("队列空，无法访问队首元素");
        }
        return data[head];
    }

    T &back() {
        if (empty()) {
            throw out_of_range("队列空，无法访问队尾元素");
        }
        return data[rear];
    }

    const T &back() const {
        if (empty()) {
            throw out_of_range("队列空，无法访问队尾元素");
        }
        return data[rear];
    }

    bool empty() const {
        return the_size == 0;
    }

private:
    void resize(int new_capa) {
        if (new_capa < the_size) {
            return;
        }
        T *old_data = data;
        data = new T[new_capa];
        for (int i = 0, j = head; i < the_size; i++, j = (j + 1) % the_capa) {
            data[i] = old_data[j];
        }
        delete[] old_data;
        head = 0;
        rear = the_size - 1;
        the_capa = new_capa;
    }
};

int main() {
    Queue<int> q;
    int n, op1, op2;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &op1);
        if (op1 == 1) {
            scanf("%d", &op2);
            q.push(op2);
        } else if (op1 == 0) {
            int ret = q.pop();
            if (ret != -1) {
                printf("%d\n", ret);
            } else {
                printf("invalid\n");
            }
        }
    }
    return 0;
}
