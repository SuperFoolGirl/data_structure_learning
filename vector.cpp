#include <iostream>
using namespace std;

template <typename Object>
class Vector
{
public:
    Vector(int initSize = 0) : theSize(initSize), theCapacity(initSize + SPARE_CAPACITY)
    {
        objects = new Object[theCapacity];
    }

    Vector(const Vector &rhs)
    {
        operator=(rhs);
    }

    ~Vector()
    {
        delete[] objects;
    }

    Vector &operator=(const Vector &rhs) // 重载运算符函数返回类型大多数是引用。没必要常量引用
    {
        if (this != &rhs) // 自赋值判断
        {
            delete[] objects;
            theSize = rhs.theSize;
            theCapacity = rhs.theCapacity;
            objects = new Object[theCapacity];
            for (int i = 0; i < theSize; i++)
                objects[i] = rhs.objects[i];
        }
        return *this;
    }

    void resize(int newSize)
    {
        if (newSize > theCapacity)
            reserve(newSize * 2 + 1);
        theSize = newSize;
    }

    void reserve(int newCapacity)
    {
        if (newCapacity < theSize)
            return;
        Object *oldArray = objects;
        objects = new Object[newCapacity];
        for (int i = 0; i < theSize; i++)
            objects[i] = oldArray[i];
        delete[] oldArray;
    }

    Object &operator[](int index)
    {
        return objects[index];
    }
    const Object &operator[](int index) const
    {
        return objects[index];
    }

    bool empty() const
    {
        return size();
    }

    int size() const
    {
        return theSize;
    }

    int capacity() const
    {
        return theCapacity;
    }

    void push_back(const Object &x)
    {
        if (theSize == theCapacity)
            reserve(2 * theCapacity + 1);
        objects[theSize++] = x;
    }

    void pop_back()
    {
        theSize--;
    }

    const Object& back() const
    {
        return objects[theSize - 1];
    }

    // 迭代器用指针实现
    typedef Object *iterator;
    typedef const Object *const_iterator;

    iterator begin()
    {
        return &objects[0];
    }
    const_iterator begin() const
    {
        return &objects[0];
    }

    iterator end()
    {
        return &objects[theSize];
    }
    const_iterator end() const
    {
        return &objects[theSize];
    }

    enum
    {
        SPARE_CAPACITY = 14 // good
    };

private:
    int theSize;
    int theCapacity;
    Object *objects;
};

int main()
{
    Vector<int> v;
    return 0;
}
