#include <iostream>

using namespace std;

template <typename T>
class Stack {
private:
    T *data;
    int the_size;
    int the_capa;
    static constexpr int SPARE_CAPACITY = 10;    // 常量

public:
    Stack(int size = 0) : the_size(0), the_capa(size + SPARE_CAPACITY) {
        data = new T[the_capa];
    }

    ~Stack() {
        delete[] data;
    }

    Stack(const Stack &rhs) {
        *this = rhs;
    }

    Stack &operator=(const Stack &rhs) {
        if (this != &rhs) {
            delete[] data;
            the_size = rhs.the_size;
            the_capa = rhs.the_capa;

            data = new T[the_capa];
            for (int i = 0; i < the_size; i++) {
                data[i] = rhs.data[i];
            }
        }
        return *this;
    }

    T &top() {
        if (empty()) {
            throw out_of_range("栈空，无法访问栈顶元素");
        }
        return data[the_size - 1];
    }

    const T &top() const {
        if (empty()) {
            throw out_of_range("栈空，无法访问栈顶元素");
        }
        return data[the_size - 1];
    }

    bool empty() const {
        return the_size == 0;
    }

    int size() const {
        return the_size;
    }

    void push(const T &x) {
        // 提供动态数组扩容私有方法
        if (the_size == the_capa) {
            resize(2 * the_capa);
        }
        data[the_size++] = x;
    }

    int pop() {
        if (empty()) {
            // throw out_of_range("栈空，无法出栈");
            return -1;
        }
        the_size--;
        return data[the_size];
    }

private:
    void resize(int new_capa) {
        if (new_capa < the_size) {
            return;
        }
        T *old_data = data;
        data = new T[new_capa];
        for (int i = 0; i < the_size; i++) {
            data[i] = old_data[i];
        }
        delete[] old_data;
        the_capa = new_capa;
    }
};

int main() {
    Stack<int> s;
    int n, op1, op2;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &op1);
        if (op1 == 1) {
            scanf("%d", &op2);
            s.push(op2);
        } else if (op1 == 0) {
            int ret = s.pop();
            if (ret == -1) {
                printf("invalid\n");
            } else {
                printf("%d\n", ret);
            }
        }
    }
    return 0;
}
