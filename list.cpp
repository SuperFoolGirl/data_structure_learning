#include <iostream>
using namespace std;

template <typename Object>
class List
{
private:
    struct Node
    {
        Object data;
        Node *prev;
        Node *next;

        Node(const Object &d = Object(), Node *p = nullptr, Node *n = nullptr) : data(d), prev(p), next(n) {}
    };

public:
    class const_iterator
    {
    public:
        const_iterator() : current(nullptr) {}
        const Object *operator*() const
        {
            return retrieve();
        }

        const_iterator &operator++()
        {
            current = current->next;
            return *this;
        }
        const_iterator &operator++(int)
        {
            const_iterator old = *this; // this指向的是const_iterator这个类 两边类型是相同的
            ++(*this);
            return old; // old此时还是传进来的那个对象 不过对象的
        }

        bool operator==(const const_iterator &rhs) const
        {
            return current == rhs.current;
        }
        bool operator!=(const const_iterator &rhs) const
        {
            return !(*this == rhs);
        }

    protected: // 允许继承者访问成员 其他类不允许
        Node *current;
        Object &retrieve() const
        {
            return current->data;
        }
        const_iterator(Node *p) : current(p) {}

        friend class List<Object>;
    };

    class iterator : public const_iterator
    {
    public:
        iterator() {}
        Object &operator*()
        {
            return const_iterator::retrieve();
        }
        const Object &operator*() const
        {
            return const_iterator::operator*();
        }

        iterator &operator++()
        {
            const_iterator::current = const_iterator::current->next;
            return *this;
        }
        iterator &operator++(int)
        {
            iterator old = *this;
            ++(*this);
            return old;
        }

    protected:
        iterator(Node *p) : const_iterator(p) {} // 这里初始化列表还可以写函数

        friend class List<Object>; // List可访问iterator的私有成员
    };

public:
    List()
    {
        init();
    }

    ~List()
    {
        clear();
        delete head;
        delete tail;
    }

    List(const List &rhs)
    {
        init(); // ？
        *this = rhs;
    }

    const List &operator=(const List &rhs)
    {
        if (this == rhs)
            return this;
        clear();
        for (const_iterator itr = rhs.begin(); itr != rhs.end(); itr++) // 调用itr构造函数 current就有了
            push_back(*itr);                                            // 这里才是重点 const_iterator类虽然是独立的 但*调用重载函数 与current建立了连接
        return *this;
    }

    iterator begin()
    {
        return iterator(head->next); // head是头哨兵 这里调用了it的构造函数 使得current指向头节点
        // 构造函数的隐式返回值是构造出来的对象，符合返回类型。至于it这个类的本质 vector里解释过了 就是Object* 只是多加了一些功能函数而已
    }
    const_iterator begin() const
    {
        return const_iterator(head->next);
    }
    iterator end()
    {
        return iterator(tail); // 最后一个节点的下一个 尾哨兵
    }
    const_iterator end() const
    {
        return const_iterator(tail);
    }

    int size() const
    {
        return theSize;
    }

    bool empty() const
    {
        return size();
    }

    void clear()
    {
        while (!empty())
            pop_front();
    }

    Object &front()
    {
        return *begin();
    }
    const Object &front() const
    {
        return *begin();
    }
    Object &back()
    {
        return *--end(); // --没重载 按理说用不了吧 返回tail的上一个 就是尾结点
    }
    const Object &back() const
    {
        return *--end();
    }

    void push_front(const Object &x)
    {
        insert(begin(), x);
    }
    void push_back(const Object &x)
    {
        insert(end(), x);
    }

    void pop_front()
    {
        erase(begin());
    }
    void pop_back()
    {
        erase(--end());
    }

    iterator insert(iterator itr, const Object &x)
    {
        Node *p = itr.current;
        theSize++;
        return iterator(p->prev = p->prev->next = new Node(x, p->prev, p)); // push函数没有返回值 这里insert函数的返回值返回了一个迭代器 源码好像是这样写的 要模拟实现嘛 很多库函数都有类似的返回值 不过大多数时候用不到，只需要完成insert的功能即可 例如memcpy
    }

    iterator erase(iterator itr)
    {
        Node *p = itr.current;
        iterator retVal(p->next);
        p->prev->next = p->next;
        p->next->prev = p->prev;
        delete p;
        theSize--;
        return retVal; // 同insert的返回值 erase的返回值其实也不常用 返回被删节点的下一项
    }
    iterator erase(iterator start, iterator end)
    {
        for (iterator itr = start; itr != end;)
            itr = erase(itr); // 用到返回值了
        return end;
    }

private:
    int theSize;
    Node *head;
    Node *tail;

    void init()
    {
        theSize = 0;
        head = new Node;
        tail = new Node;
        head->next = tail;
        tail->prev = head;
    }
};

int main()
{
    List<int> l;
    // 修改测试
    return 0;
}