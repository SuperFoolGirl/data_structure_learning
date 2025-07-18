// 分离链接法的哈希表实现

#include <algorithm>     // 包含 std::find
#include <functional>    // 包含 std::hash
#include <iostream>
#include <list>
#include <string>
#include <vector>

// --- 全局辅助函数 ---

// 辅助函数：判断一个数是否为素数
// 这个写法效率较高
bool isPrime(int n) {
    if (n <= 1) {
        return false;
    }
    if (n <= 3) {
        return true;
    }
    if (n % 2 == 0 || n % 3 == 0) {
        return false;
    }
    // 由于我们已经排除2和3的倍数，那么所有大于3的质数都可以表示为6k
    for (int i = 5; i * i <= n; i = i + 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

// 辅助函数：找到大于或等于 n 的最小素数
int nextPrime(int n) {
    if (n % 2 == 0) {
        n++;       // 如果是偶数，从下一个奇数开始检查
    }
    while (!isPrime(n)) {
        n += 2;    // 只检查奇数
    }
    return n;
}

// --- 模板类 `HashTable` ---

template<class HashedObj>
class HashTable {
public:
    // 构造函数：初始化哈希表
    // 确保表的大小是素数，通常能提供更好的哈希分布
    explicit HashTable(int initialSize = 101) : currentElementCount(0) {
        // nextPrime(initialSize) 确保 initialSize 即使不是素数也能得到一个合适的素数大小
        theBuckets.resize(nextPrime(initialSize));
    }

    // 检查元素是否存在
    bool contains(const HashedObj &x) const {
        // const list<HashedObj>& targetList = theBuckets[myHash(x)];
        // return std::find(targetList.begin(), targetList.end(), x) != targetList.end();

        // 改进：直接使用链表的迭代器进行查找，避免创建不必要的引用
        // 如果 find 返回 end()，则表示没找到
        return std::find(theBuckets[myHash(x)].begin(), theBuckets[myHash(x)].end(), x) != theBuckets[myHash(x)].end();
    }

    // 清空哈希表
    void makeEmpty() {
        for (auto &bucket : theBuckets) {
            bucket.clear();
        }
        currentElementCount = 0;    // 重置元素计数
    }

    // 插入元素
    bool insert(const HashedObj &x) {
        // 获取对应哈希值的链表引用
        std::list<HashedObj> &targetList = theBuckets[myHash(x)];

        // 检查元素是否已存在，如果存在则不插入
        if (std::find(targetList.begin(), targetList.end(), x) != targetList.end()) {
            return false;    // 元素已存在
        }

        // 将元素添加到链表末尾
        targetList.push_back(x);

        // 增加当前元素数量
        currentElementCount++;

        // 检查负载因子，如果超过阈值，则触发再散列
        // 对于分离链接法，负载因子通常可以设定得比1大，比如1.5或2.0，具体取决于链表操作的性能
        // 这里的阈值设置为当元素数量等于桶的数量时触发，即负载因子约为 1.0
        if (currentElementCount > theBuckets.size()) {
            rehash();
        }

        return true;    // 插入成功
    }

    // 删除元素
    bool remove(const HashedObj &x) {
        std::list<HashedObj> &targetList = theBuckets[myHash(x)];
        // typename关键字 明确告诉编译器 std::list<HashedObj>::iterator 是一个类型名
        typename std::list<HashedObj>::iterator itr = std::find(targetList.begin(), targetList.end(), x);

        // 如果未找到元素，则返回 false
        if (itr == targetList.end()) {
            return false;
        }

        // 从链表中移除元素
        targetList.erase(itr);
        currentElementCount--;    // 减少当前元素数量
        return true;              // 删除成功
    }

    // 获取当前哈希表中元素的数量
    int size() const {
        return currentElementCount;
    }

    // 获取哈希表（桶）的数量
    int bucketCount() const {
        return theBuckets.size();
    }

    // 打印哈希表内容（用于调试）
    void printHashTable() const {
        std::cout << "--- Hash Table Contents (Size: " << currentElementCount << ", Buckets: " << theBuckets.size() << ") ---" << std::endl;
        for (int i = 0; i < theBuckets.size(); ++i) {
            std::cout << "Bucket " << i << ": ";
            if (theBuckets[i].empty()) {
                std::cout << "(empty)";
            } else {
                for (const auto &item : theBuckets[i]) {
                    std::cout << item << " -> ";
                }
                std::cout << "NULL";    // 链表末尾
            }
            std::cout << std::endl;
        }
        std::cout << "---------------------------------------------------" << std::endl;
    }

private:
    std::vector<std::list<HashedObj>> theBuckets;    // 哈希桶，重命名为theBuckets更清晰
    int currentElementCount;                         // 当前元素数量，重命名为currentElementCount更清晰

    // 再散列函数
    // 增加哈希表的大小，并重新分布所有元素
    void rehash() {
        // std::move本身不做任何移动操作。它的主要作用是将一个左值转换成一个右值引用，从而启用移动语义。它只是一个类型转换，一个静态转换
        // 右值引用可以被绑定到将要接受移动内容的对象上
        // 区分下右值引用与常量引用
        // 1. 二者都可以绑定到右值上
        // 2. 右值引用不可绑定到左值上，常量引用可以
        // 3. 右值引用可以修改绑定的对象，常量引用不能修改绑定的对象

        // 通过move作用后，左值变为右值，就可以被右值引用&&绑定了，进而触发移动构造，避免深拷贝
        // 可以简单理解为“剪切”，直接把资源转移到新对象中（这个过程就是移动构造）。反正原来的东西也不用了，准备销毁了
        // 这样可以避免数据拷贝，大大提高效率。

        // 简单描述一下这个函数内，移动构造函数的调用过程：
        // 1. move把theBuckets从左值转换为右值引用
        // 2. 右值引用绑定到对象oldBuckets，触发了移动构造，oldBuckets现在拥有theBuckets的资源，且theBuckets自身变为空
        // 3. 后面扩容后，直到函数结束，oldBuckets临时变量会被销毁，释放原有的资源
        std::vector<std::list<HashedObj>> oldBuckets = std::move(theBuckets);    // 使用 std::move 避免拷贝，提高效率

        // 调整新哈希表的大小为当前大小的两倍的下一个素数
        theBuckets.resize(nextPrime(2 * oldBuckets.size()));

        currentElementCount = 0;    // 重置计数，因为后续 insert 会重新计数

        // 遍历旧的哈希表中的所有元素，并重新插入到新的哈希表中
        for (auto &oldBucket : oldBuckets) {        // 使用范围for循环遍历旧桶
            for (const auto &item : oldBucket) {    // 遍历旧桶中的每个链表元素
                insert(item);                       // 重新插入，这会再次调用 myHash 和 insert 逻辑
            }
        }
        // 注意：rehash 过程中每次 insert 都会增加 currentElementCount，但不会再次触发 rehash
        // 因为 rehash 的目的是一次性完成扩容，而不是在扩容过程中再次扩容
        // 最终的 currentElementCount 应该与原始的元素数量一致
    }

    // 核心哈希映射函数
    // 负责将 HashedObj 映射到 `theBuckets` 数组的有效索引
    int myHash(const HashedObj &x) const {
        // 使用std::hash模板，它为基本类型和 std::string 等提供了默认实现
        // 如果HashedObj是自定义类型，需要为HashedObj特化std::hash，或提供一个友元函数
        // std::hash 的 operator() 返回一个 size_t 类型的值
        // 这里使用花括号是在创建匿名对象，用圆括号也行
        // 不过花括号统一表示“初始化”操作，更符合编程规范。其他初始化操作大多也是用花括号完成的
        size_t hashVal = std::hash<HashedObj>{}(x);    // 使用 std::hash 对象
        // std::hash<HashedObj>{}：创建了一个 std::hash<HashedObj> 类型的匿名临时对象，并调用了其默认构造函数。
        // 紧接着的 (x)：调用了这个临时对象的 operator() 成员函数，并将 x 作为参数传递，从而计算出 x 的哈希值。

        // 对哈希值取模，确保它落在 [0, theBuckets.size() - 1] 范围内
        hashVal %= theBuckets.size();

        return static_cast<int>(hashVal);    // 转换为 int 类型返回
    }
};

// --- 主函数测试 ---

int main() {
    // 1. 测试 int 类型的哈希表
    std::cout << "--- Testing HashTable with int ---" << std::endl;
    HashTable<int> intHashTable(7);    // 初始大小为 7 (一个素数)

    // 插入元素
    intHashTable.insert(10);
    intHashTable.insert(21);
    intHashTable.insert(32);
    intHashTable.insert(13);
    intHashTable.insert(44);
    intHashTable.insert(55);    // 负载因子可能触发 rehash
    intHashTable.insert(6);
    intHashTable.insert(17);
    intHashTable.insert(28);    // 再次触发 rehash

    intHashTable.printHashTable();

    // 检查元素是否存在
    std::cout << "Contains 21: " << (intHashTable.contains(21) ? "Yes" : "No") << std::endl;
    std::cout << "Contains 99: " << (intHashTable.contains(99) ? "Yes" : "No") << std::endl;

    // 删除元素
    std::cout << "Removing 13..." << std::endl;
    intHashTable.remove(13);
    intHashTable.printHashTable();
    std::cout << "Contains 13: " << (intHashTable.contains(13) ? "Yes" : "No") << std::endl;

    std::cout << "Removing 100 (non-existent)..." << std::endl;
    intHashTable.remove(100);
    intHashTable.printHashTable();

    // 2. 测试 string 类型的哈希表
    std::cout << "\n--- Testing HashTable with string ---" << std::endl;
    HashTable<std::string> stringHashTable(5);    // 初始大小为 5 (一个素数)

    stringHashTable.insert("apple");
    stringHashTable.insert("banana");
    stringHashTable.insert("cherry");
    stringHashTable.insert("date");
    stringHashTable.insert("elderberry");    // 可能触发 rehash
    stringHashTable.insert("fig");

    stringHashTable.printHashTable();

    std::cout << "Contains 'banana': " << (stringHashTable.contains("banana") ? "Yes" : "No") << std::endl;
    std::cout << "Contains 'grape': " << (stringHashTable.contains("grape") ? "Yes" : "No") << std::endl;

    std::cout << "Removing 'cherry'..." << std::endl;
    stringHashTable.remove("cherry");
    stringHashTable.printHashTable();
    std::cout << "Contains 'cherry': " << (stringHashTable.contains("cherry") ? "Yes" : "No") << std::endl;

    std::cout << "Make empty..." << std::endl;
    stringHashTable.makeEmpty();
    stringHashTable.printHashTable();

    return 0;
}
