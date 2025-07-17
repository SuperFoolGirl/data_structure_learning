#include <iostream>
#include <list>
#include <string>
#include <vector>

using namespace std;

int hash(const string &key) {
    int HashVal = 0;
    for (int i = 0; i < key.length(); i++) {
        HashVal = 37 * HashVal + key[i];
    }
    return HashVal;
}

int hash(int key) {}

template<class HashedObj>
class HashTable {
public:
    explicit HashTable(int size = 101) {}    // 使表的大小为质数 会得到更好的分布

    bool contains(const HashedObj &x) const {
        const list<HashedObj> &whichList = theLists[myhash(x)];
        return find(whichList.brgin(), whichList.end(), x) != whichList.end();
    }

    void makeEmpty() {
        for (int i = 0; i < theLists.size(); i++) {
            theLists[i].clear();
        }
    }

    bool insert(const HashedObj &x) {
        // 表的大小是theList.size() 计算得到的hash保证落到这里面来
        list<HashedObj> &whichList = theLists[myhash(x)];
        if (find(whichList.begin(), whichList.end(), x) != whichList.end()) {
            return false;
        }
        whichList.push_back(x);
        // 添加完一个正好加加 然后看是否溢出。在这段代码里，当 currentSize > theLists.size()，也就是负载因子大于 1 时，触发 rehash 函数
        if (++currentSize > theLists.size()) {
            rehash();
        }
        return true;
    }

    bool remove(const HashedObj &x) {
        list<HashedObj> &whichList = theLists[myhash(x)];
        list<HashedObj>::iterator itr = find(whichList.begin(), whichList.end(), x);
        if (itr == whichList.end()) {
            return false;
        }
        whichList.erase(itr);
        --currentSize;
        return true;
    }

private:
    vector<list<HashedObj>> theLists;
    int currentSize;

    // 再散列函数
    // 随着装填因子上升 表的性能在下降 因此 达到某个装填因子后进行再散列
    void rehash() {
        vector<list<HashedObj>> oldLists = theLists;
        theLists.resize(nextPrime(2 * theLists.size()));
        for (int i = 0; i < theLists.size(); i++) {
            theLists[i].clear();
        }

        currentSize = 0;
        for (int i = 0; i < oldLists.size(); i++) {
            list<HashedObj>::iterator itr = oldLists[i].begin();    // 遍历链表需要迭代器
            while (itr != oldLists[i].end()) {
                insert(*itr++);
            }
        }
    }

    int myhash(const HashedObj &x) const {
        int hashVal = hash(x);
        hashVal %= theLists.size();
        if (hashVal < 0) {
            hashVal += theLists.size();
        }
        return hashVal;
    }
};

int main() {
    return 0;
}
