// 通过散列（哈希）函数，使关键字key映射到一个值，这个值就是对应下标。用这个下标来访问value
// 给定一个最简单的例子，刚入门时做oj，开一个较大的数组，直接用下标访问
// 哈希函数为H(key) = key. H(key)为映射的地址，即下标
// 但如果key是字符串，或者其他类型的对象，就需要一个散列函数，将key映射到一个整数，然后用这个整数作为下标来访问value

// 实现上，这里采用了除留余数法
// H(key) = key % p
// p为大于等于表长的最小质数
// 这样可以保证散列函数的值在0到p-1之间，比直接取余发生冲突概率更小，表分布会更均匀

// 散列表空间利用率，用装填因子α表示
// α = 当前元素个数 / 散列表大小（表长）
// 装填因子α越大，冲突的概率就越大

// 冲突会影响散列表的性能，散列表性能由散列函数、装填因子和冲突解决方法决定
// a. 散列函数构造方法：
//    1. 直接定址法  适合关键字连续分布。线性函数无冲突
//    2. 除留余数法  最常用，但有冲突风险
// b. 装填因子α：
//    当装填因子到达某个阈值时，通常是0.5或0.7，就需要进行rehash操作
//    rehash是指扩展散列表的大小，并重新计算所有元素的位置
// c. 冲突解决方法：
//    1. 开放寻址法：找一个新的位置存储冲突的元素
//       a. 线性探测：如果当前位置被占用，就向后查找，直到找到空位。
//           - 如果遇到了一个空位，说明目标元素不在表中。因为如果目标元素在表中，在线性探测下，一定会找到一个空位安置自己
//           - 缺点：容易造成关键字堆集，数据量过大时性能下降严重
//           - 删除时，不能直接删除元素，而是将其标记为已删除。因为如果直接删除，后续的查找可能会错过这个元素；打了标记的位置，仍然可以放新value
//       b. 二次探测：使用二次函数来计算新的位置
//           - 冲突时，按照n^2的方式探测(n取1,-1,4,-4,9,-9,...)
//           - 查找：同样先计算位置，如果被占用，就使用二次函数计算新的位置，直到找到空位或目标元素
//           - 缺点：有的位置可能永远不会被访问到，导致空间浪费；而且二次探测的步长不够灵活，可能会导致聚集现象
//           - 特殊：如果表长为4k+3的质数，二次探测法可以保证每个位置都能被访问到。所以在实现时，通常会选择这样的质数作为表长（这就是数论！！）
//    2. 分离链接法：每个位置存储一个单链表，冲突的元素都放在这个链表中（类似邻接表）
//       - 表长等参数取法还是根据装填因子、质数等来决定
//       - 表中每个元素都放一个头结点
//       - 需要插入时，直接头插即可。由于是单链表，头插效率高
//       - 删除时，直接删除链表中的元素即可

// 衡量散列表效率：Average Search Length (ASL)
// 可以一边插入，一边计算平均查找长度
// ASL分为两种情况：成功查找和不成功查找
// 成功查找ASL：
// ASL(success) = 所有表中数据查找次数之和 / 数据个数
// 不成功查找ASL：
// ASL(failure) = 所有位置查找到失败次数之和 / 表长-1

// 开放寻址法——二次探测法的哈希表实现

#include <algorithm>     // for std::find if needed, but not directly used in open addressing search
#include <functional>    // for std::hash
#include <iostream>
#include <string>
#include <vector>

// --- 全局辅助函数 ---

// 辅助函数：判断一个数是否为素数
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
    for (int i = 5; i * i <= n; i = i + 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

// 辅助函数：找到大于或等于 n 的最小素数
int nextPrime(int n) {
    if (n <= 0) {
        return 3;    // 确保至少从3开始
    }
    if (n % 2 == 0) {
        n++;         // 如果是偶数，从下一个奇数开始检查
    }
    while (!isPrime(n)) {
        n += 2;      // 只检查奇数
    }
    return n;
}

// --- 模板类 `HashTable` ---

template<class HashedObj>
class HashTable {
public:
    // 定义槽位状态枚举
    enum EntryType {
        ACTIVE,    // 包含活跃元素
        EMPTY,     // 空槽位，从未被使用或已清空
        DELETED    // 元素已被逻辑删除
    };

    // 哈希表的槽位结构
    struct HashEntry {
        HashedObj element;
        EntryType info;

        // 构造函数
        HashEntry(const HashedObj &e = HashedObj(), EntryType i = EMPTY) : element(e), info(i) {}
    };

    // 构造函数：初始化哈希表
    // 确保表的大小是素数，这对于二次探测非常重要
    // 确保 initialSize 即使不是素数也能得到一个合适的素数大小
    // 初始时 array 会自动用 HashEntry 的默认构造函数填充 EMPTY 状态
    explicit HashTable(int initialSize = 101) : array(nextPrime(initialSize)) {
        makeEmpty();    // 明确将所有槽位设置为 EMPTY，并重置计数
    }

    // 检查元素是否存在
    bool contains(const HashedObj &x) const {
        return isActive(findPos(x));    // 存在的基础上，必须是活跃状态，才能算存在
    }

    // 清空哈希表
    void makeEmpty() {
        currentActiveSize = 0;    // 重置活跃元素计数
        // 将所有槽位标记为 EMPTY，懒惰删除
        // 懒惰删除这个思想很关键，每个槽位用一个结构体封装也很巧妙
        for (int i = 0; i < array.size(); ++i) {
            array[i].info = EMPTY;
        }
    }

    // 插入元素
    bool insert(const HashedObj &x) {
        int currentPos = findPos(x);    // 查找元素位置或插入位置

        // 如果找到的位置是 ACTIVE，说明元素已存在，无需重复插入
        if (isActive(currentPos)) {
            return false;
        }

        // 找到的位置不是 ACTIVE (可能是 EMPTY 或 DELETED)，可以插入
        array[currentPos] = HashEntry(x, ACTIVE);
        currentActiveSize++;    // 增加活跃元素计数

        // 检查负载因子。对于二次探测，负载因子通常建议不超过 0.5。
        // currentActiveSize 是活跃元素数量
        // array.size() 是哈希表总大小
        if (currentActiveSize > array.size() / 2) {
            rehash();
        }
        return true;
    }

    // 删除元素（逻辑删除）
    bool remove(const HashedObj &x) {
        int currentPos = findPos(x);    // 查找元素位置

        // 如果元素不存在或不在活跃状态，则无法删除
        if (!isActive(currentPos)) {
            return false;
        }

        // 标记为 DELETED
        array[currentPos].info = DELETED;
        currentActiveSize--;    // 减少活跃元素计数（重要修复）
        return true;
    }

    // 获取当前活跃元素的数量
    int size() const {
        return currentActiveSize;
    }

    // 获取哈希表（桶）的总容量
    int capacity() const {
        return array.size();
    }

    // 打印哈希表内容（用于调试）
    void printHashTable() const {
        std::cout << "--- Hash Table Contents (Active: " << currentActiveSize << ", Capacity: " << array.size() << ") ---" << std::endl;
        for (int i = 0; i < array.size(); ++i) {
            std::cout << "Bucket " << i << ": ";
            if (array[i].info == ACTIVE) {
                std::cout << array[i].element << " (ACTIVE)";
            } else if (array[i].info == DELETED) {
                std::cout << " (DELETED)";
            } else {    // EMPTY
                std::cout << " (EMPTY)";
            }
            std::cout << std::endl;
        }
        std::cout << "---------------------------------------------------" << std::endl;
    }

private:
    std::vector<HashEntry> array;    // 存储哈希表槽位的vector
    int currentActiveSize;           // 当前活跃元素的数量

    // 辅助函数：判断给定位置的槽位是否活跃
    bool isActive(int currentPos) const {
        return array[currentPos].info == ACTIVE;
    }

    // 查找元素 x 的位置
    // 如果 x 存在，返回其位置；如果 x 不存在，返回它应该插入的位置
    // 这里实现的是标准平方探测，即1, 4, 9, 16, ... 的方式探测
    // 并没有使用双向平方探测，即 -1, 1, -4, 4, -9, 9, ... 的方式
    int findPos(const HashedObj &x) const {
        int offset = 1;
        int currentPos = myHash(x);    // 初始哈希位置

        // 探测循环：
        // 1. 槽位不是 EMPTY (可能 ACTIVE 或 DELETED)
        // 2. 槽位中的元素不是 x (我们还没有找到 x)
        while (array[currentPos].info != EMPTY && array[currentPos].element != x) {
            currentPos += offset;    // 加上当前偏移量
            offset += 2;             // 增加偏移量，为下次探测做准备 (1, 3, 5, 7...)

            // 处理循环回绕
            if (currentPos >= array.size()) {
                currentPos -= array.size();
            }
        }
        return currentPos;
    }

    // 再散列函数：当哈希表负载过高时进行扩容和重建
    void rehash() {
        std::vector<HashEntry> oldArray = std::move(array);    // 使用移动语义，避免深拷贝

        // 调整新哈希表的大小为当前大小的两倍的下一个素数
        array.resize(nextPrime(2 * oldArray.size()));

        // 遍历新表，将其所有槽位初始化为 EMPTY
        // (resize会默认调用HashEntry的默认构造，所以这里实际是多余的，但写上确保语义明确)
        for (auto &entry : array) {
            entry.info = EMPTY;
        }

        currentActiveSize = 0;    // 重置活跃元素计数，因为后续 insert 会重新计数

        // 遍历旧哈希表中的所有活跃元素，并重新插入到新的哈希表中
        for (const auto &entry : oldArray) {    // 使用范围for循环遍历旧数组
            if (entry.info == ACTIVE) {
                insert(entry.element);          // 重新插入，这会再次调用 myHash 和 insert 逻辑
                                                // 这里的 insert 调用会增加 currentActiveSize，
                                                // 但不会再次触发 rehash，因为是在 rehash 内部。
            }
        }
    }

    // 核心哈希映射函数
    // 负责将 HashedObj 映射到 `array` 数组的有效索引
    int myHash(const HashedObj &x) const {
        // 使用 std::hash 模板，它为基本类型和 std::string 等提供了默认实现
        size_t hashVal = std::hash<HashedObj>{}(x);

        // 对哈希值取模，确保它落在 [0, array.size() - 1] 范围内
        hashVal %= array.size();

        // std::hash 返回的是 size_t (无符号)，所以 hashVal 不会是负数
        return static_cast<int>(hashVal);
    }
};

// --- 主函数测试 ---

int main() {
    // 1. 测试 int 类型的哈希表
    std::cout << "--- Testing HashTable with int (Quadratic Probing) ---" << std::endl;
    HashTable<int> intHashTable(7);    // 初始大小为 7 (一个素数)

    std::cout << "Initial state:" << std::endl;
    intHashTable.printHashTable();

    // 插入元素
    intHashTable.insert(10);    // H(10) % 7 = 3
    intHashTable.insert(21);    // H(21) % 7 = 0
    intHashTable.insert(32);    // H(32) % 7 = 4
    intHashTable.insert(13);    // H(13) % 7 = 6
    intHashTable.insert(44);    // H(44) % 7 = 2

    std::cout << "\nAfter initial insertions:" << std::endl;
    intHashTable.printHashTable();

    // 触发 rehash (负载因子 > 0.5)
    // 假设当前容量 7，当 activeSize > 3 时触发 rehash
    intHashTable.insert(55);          // H(55) % 7 = 6 (冲突), 探测到下一个空位
    std::cout << "\nAfter triggering rehash with 55 (current active size is 6, capacity > 3.5):" << std::endl;
    intHashTable.printHashTable();    // 此时容量可能已变为 17 (nextPrime(2*7=14))

    intHashTable.insert(6);           // 再次插入一些元素
    intHashTable.insert(17);
    intHashTable.insert(28);

    std::cout << "\nAfter more insertions:" << std::endl;
    intHashTable.printHashTable();

    // 检查元素是否存在
    std::cout << "\nContains 21: " << (intHashTable.contains(21) ? "Yes" : "No") << std::endl;    // 应该 Yes
    std::cout << "Contains 99: " << (intHashTable.contains(99) ? "Yes" : "No") << std::endl;      // 应该 No

    // 删除元素
    std::cout << "\nRemoving 13..." << std::endl;
    intHashTable.remove(13);                                                                    // H(13) % 17 = 13 (假设在容量17的表中)
    intHashTable.printHashTable();
    std::cout << "Contains 13: " << (intHashTable.contains(13) ? "Yes" : "No") << std::endl;    // 应该 No

    std::cout << "Removing 100 (non-existent)..." << std::endl;
    intHashTable.remove(100);
    intHashTable.printHashTable();

    // 尝试插入一个被删除的位置的元素，或者新的元素
    std::cout << "\nInserting 33 (might re-use DELETED slot)..." << std::endl;
    intHashTable.insert(33);    // H(33) % 17 = 16
    intHashTable.printHashTable();

    // 清空哈希表
    std::cout << "\nMake empty..." << std::endl;
    intHashTable.makeEmpty();
    intHashTable.printHashTable();
    std::cout << "Size after makeEmpty: " << intHashTable.size() << std::endl;

    // 2. 测试 string 类型的哈希表
    std::cout << "\n--- Testing HashTable with string ---" << std::endl;
    HashTable<std::string> stringHashTable(5);

    stringHashTable.insert("apple");
    stringHashTable.insert("banana");
    stringHashTable.insert("cherry");
    stringHashTable.insert("date");
    stringHashTable.insert("elderberry");
    stringHashTable.insert("fig");

    stringHashTable.printHashTable();

    std::cout << "Contains 'banana': " << (stringHashTable.contains("banana") ? "Yes" : "No") << std::endl;
    std::cout << "Contains 'grape': " << (stringHashTable.contains("grape") ? "Yes" : "No") << std::endl;

    std::cout << "Removing 'cherry'..." << std::endl;
    stringHashTable.remove("cherry");
    stringHashTable.printHashTable();
    std::cout << "Contains 'cherry': " << (stringHashTable.contains("cherry") ? "Yes" : "No") << std::endl;

    return 0;
}
