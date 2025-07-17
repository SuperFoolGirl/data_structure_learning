#include <iostream>
#include <vector>
using namespace std;

template<class HashedObj>
class HashTable {
public:
    explicit HashTable(int size = 101) : array(nextPrime(size)) {
        makeEmpty();
    }

    bool contains(const HashedObj &x) const {
        return isActive(findPos(x));
    }

    void makeEmpty() {
        currentSize = 0;
        for (int i = 0; i < array.size(); i++) {
            array[i].info = EMPTY;
        }
    }

    bool insert(const HashedObj &x) {
        int currentPos = findPos(x);
        if (isActive(currentPos)) {
            return false;
        }

        array[currentPos] = HashEntry(x, ACTIVE);
        if (++currentSize > array.size() / 2) {
            rehash();
        }
        return true;
    }

    bool remove(const HashedObj &x) {
        int currentPos = findPos(x);
        if (!isActive(currentPos)) {
            return false;
        }

        array[currentPos].info = EMPTY;
        return true;
    }

    enum EntryType {
        ACTIVE,
        EMPTY,
        DELETED
    };

private:
    struct HashEntry {
        HashedObj element;
        EntryType info;

        HashEntry(const HashedObj &e = HashedObj(), EntryType i = EMPTY) : element(e), info(i) {}
    };

    vector<HashEntry> array;
    int currentSize;

    bool isActive(int currentPos) const {
        return array[currentPos].info == ACTIVE;
    }

    int findPos(const HashedObj &x) const {
        int offset = 1;
        int currentPos = myhash(x);
        while (array[currentPos].info != EMPTY && array[currentPos].element != x)    // 如果这个位置被占了
        {
            // 以初始的currentPos为基准 第n次增offset后，currentPos当前的值-基准值=n*n 模拟一下 就是从基准值处不断以平方单位进行下探
            currentPos += offset;
            offset += 2;
            if (currentPos >= array.size()) {    // 如果超出表大小 就重来
                currentPos -= array.size();
            }
        }
        return currentPos;
    }

    void rehash() {
        vector<HashEntry> oldArray = array;
        array.resize(nextPrime(2 * oldArray.size()));
        for (int i = 0; i < array.size(); i++) {
            array[i].info = EMPTY;
        }

        currentSize = 0;                               // insert函数里会加的，for循环走完加出新的currentSize来
        for (int i = 0; i < oldArray.size(); i++) {    // 遍历vector不需要迭代器 直接当数组用就行 但链表就不行了 需要迭代器
            if (oldArray[i].info == ACTIVE) {
                insert(oldArray[i].element);
            }
        }
    }

    int myhash(const HashedObj &x) const {}
};

int main() {
    return 0;
}
