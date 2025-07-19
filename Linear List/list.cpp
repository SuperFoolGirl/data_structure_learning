#include <iostream>
using namespace std;

template<typename Object>
class List {
private:
    // 封装了指针节点作为成员变量
    struct Node {
        Object data;
        Node *prev;
        Node *next;

        Node(const Object &d = Object(), Node *p = nullptr, Node *n = nullptr) : data(d), prev(p), next(n) {}
    };

public:
    // 相比vector list类的迭代器是通过嵌套类实现的，不再是简单用指针
    // 根据面向对象的思想，我们所需要的东西，都可以把它封装成一个类

    // 嵌套类作用：
    // 1. 控制外部访问权限
    // 2. 内部类可访问外部类的私有成员（如果两个类分开写的话，那不能直接访问私有成员，还需要友元声明）（反过来不行）
    // 3. 命名空间管理
    class const_iterator {
    public:
        const_iterator() : current(nullptr) {}

        const Object *operator*() const {
            return retrieve();
        }

        const_iterator &operator++() {
            current = current->next;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator old = *this;                     // this指向的是const_iterator这个类 两边类型是相同的
            ++(*this);                                      // 调用前置++
            return old;                                     // 返回常量引用
        }

        bool operator==(const const_iterator &rhs) const    // 比较地址
        {
            return current == rhs.current;
        }

        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);    // 直接调用上面的==运算符，注意传参传的是迭代器对象
        }

    protected:
        Node *current;

        Object &retrieve() const {
            return current->data;
        }

        // 外界不允许调用有参构造函数
        const_iterator(Node *p) : current(p) {}

        friend class List<Object>;    // 友元声明可以写在类的任何地方。朋友函数可以访问protected和private成员
        // 但友元声明不具备继承性，派生类还需要单独进行友元声明
    };

    // 迭代器类继承自const_iterator类，增加了对数据的修改功能。
    // 因为是增加了功能，所以它是作为派生类的。
    // 粗略来说，基类是派生类的子集。
    class iterator : public const_iterator {
    public:
        iterator() {}

        Object &operator*() {
            return const_iterator::retrieve();
        }

        const Object &operator*() const {
            return const_iterator::operator*();
        }

        iterator &operator++() {
            const_iterator::current = const_iterator::current->next;
            return *this;
        }

        iterator operator++(int) {
            iterator old = *this;
            ++(*this);
            return old;
        }

    protected:
        iterator(Node *p) : const_iterator(p) {}

        // 注意，外部类不可访问内部类的私有成员，所以需要友元声明
        friend class List<Object>;    // List可访问iterator的私有成员
    };

public:
    List() {
        init();    // 这个函数外界不允许访问，因此放到private
    }

    ~List() {
        clear();
        delete head;
        delete tail;
    }

    List(const List &rhs) {
        // 拷贝构造是从无到有的过程。此时，对于调用拷贝构造的这个对象来说，它没有任何成员变量，因此也没有堆区数据
        // 因此需要先调用init函数初始化头尾哨兵
        init();
        *this = rhs;    // 注意 这里是重载赋值运算符，是深拷贝
    }

    const List &operator=(const List &rhs) {
        if (this == rhs) {
            return this;
        }
        clear();
        for (const_iterator itr = rhs.begin(); itr != rhs.end(); itr++) {    // 调用itr构造函数 current就有了
            push_back(*itr);                                                 // 这里才是重点 const_iterator类虽然是独立的 但*调用重载函数 与current建立了连接
        }
        return *this;
    }

    iterator begin() {
        return iterator(head->next);    // head是头哨兵 这里调用了it的构造函数 使得current指向头节点
        // 构造函数的隐式返回值是构造出来的对象，符合返回类型。至于it这个类的本质 vector里解释过了 就是Object*
        // 只是多加了一些功能函数而已
    }

    const_iterator begin() const {
        return const_iterator(head->next);
    }

    iterator end() {
        // end方法就应该这样返回，返回最后一个数据结点的下一个结点
        return iterator(tail);    // 最后一个节点的下一个 尾哨兵
    }

    const_iterator end() const {
        return const_iterator(tail);
    }

    int size() const {
        return theSize;
    }

    bool empty() const {
        return size();
    }

    void clear() {
        while (!empty()) {
            pop_front();
        }
    }

    Object &front() {
        return *begin();
    }

    const Object &front() const {
        return *begin();
    }

    Object &back() {
        return *--end();    // --没重载 按理说用不了吧 返回tail的上一个 就是尾结点
    }

    const Object &back() const {
        return *--end();
    }

    void push_front(const Object &x) {
        insert(begin(), x);
    }

    void push_back(const Object &x) {
        insert(end(), x);
    }

    void pop_front() {
        erase(begin());
    }

    void pop_back() {
        erase(--end());
    }

    // push函数没有返回值 这里insert函数的返回值返回了一个迭代器 源码好像是这样写的 要模拟实现嘛
    // 很多库函数都有类似的返回值 不过大多数时候用不到，只需要完成insert的功能即可 例如memcpy
    iterator insert(iterator itr, const Object &x) {
        Node *p = itr.current;
        theSize++;
        // 连续赋值，从右向左执行
        // 对于插入的值，首先为其创建结点，构造Node对象
        // 观察构造函数的传参情况，可以发现，insert方法是要把x插在itr之前
        // 这样可以很完美地处理尾插的情况，配合哨兵，免去特判。但说实话，使用起来的逻辑不太顺畅

        // 这里可以参考pop_front的实现，p是一个已有的节点，它的前驱指针和后继指针都已经指向了正确的位置
        // 然后将新结点的前驱指针指向p的前驱，将新结点的后继指针指向p。
        // 最后将p的前驱的后继指针指向新结点，将p的前驱指针更新为新结点
        // 这样就实现了在迭代器位置插入一个新结点
        return iterator(p->prev = p->prev->next = new Node(x, p->prev, p));
    }

    iterator erase(iterator itr) {
        Node *p = itr.current;
        iterator retVal(p->next);
        p->prev->next = p->next;
        p->next->prev = p->prev;
        delete p;
        theSize--;
        return retVal;    // 同insert的返回值 erase的返回值其实也不常用 返回被删节点的下一项
    }

    iterator erase(iterator start, iterator end) {
        for (iterator itr = start; itr != end;) {
            itr = erase(itr);    // 用到返回值了
        }
        return end;
    }

private:
    int theSize;
    // 哨兵节点
    // 头哨兵和尾哨兵
    Node *head;
    Node *tail;

    void init() {
        theSize = 0;
        head = new Node;
        tail = new Node;
        head->next = tail;
        tail->prev = head;
    }
};

int main() {
    List<int> list;

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);

    for (List<int>::iterator itr = list.begin(); itr != list.end(); itr++) {
        cout << *itr << " ";
    }

    return 0;
}
