// 并查集（不相交集）

#include <iostream>
#include <numeric>    // For iota, though not strictly necessary for this implementation
#include <vector>

class DisjSets {
public:
    // 构造函数：初始化 numElements 个互不相交的集合
    explicit DisjSets(int numElements);

    // 查找操作：查找元素 x 所在的集合的根（不带路径压缩）
    // const 版本，用于 const 对象或 const 成员函数调用
    int find_no_path_compression(int x) const;

    // 查找操作：查找元素 x 所在的集合的根（带路径压缩）
    // 这是更常用的版本，能有效提高后续查找的效率
    int find(int x);

    // 合并操作：将两个根 root1 和 root2 所在的集合合并
    // 采用按秩（高度/大小）合并优化
    void unionSets(int root1, int root2);

    // 获取集合数量（非必要，但有时有用）
    int countSets() const;

private:
    // 存储集合信息的向量。
    // 下面这两条是核心：
    // 如果 s[i] < 0，则 i 是一个集合的根，s[i] 的绝对值表示该集合的高度（或大小）。
    // 如果 s[i] >= 0，则 i 的父节点是 s[i]，顺着父节点s[i]继续找下去，可以找到所属集合的根。
    std::vector<int> parent_or_size;
};

// 构造函数实现
// 初始化每个元素为一个独立的集合，根的值为 -1，表示高度为 0（单个元素集合）
// 或者表示大小为 1（单个元素集合）
DisjSets::DisjSets(int numElements) : parent_or_size(numElements) {
    // 使用 std::iota 初始化也可以，但循环更直观表示每个元素都是根
    for (int i = 0; i < parent_or_size.size(); ++i) {
        parent_or_size[i] = -1;    // 初始时，每个元素都是独立的集合的根，高度为0（用-1表示）
    }
}

// 查找操作（不带路径压缩）
// 递归地查找元素的根。效率较低，不推荐在实际中使用。
int DisjSets::find_no_path_compression(int x) const {
    // 输入校验（可选，但推荐）
    if (x < 0 || x >= parent_or_size.size()) {
        std::cerr << "Error: Element " << x << " is out of bounds." << std::endl;
        return -1;                                             // 或者抛出异常
    }

    if (parent_or_size[x] < 0) {                               // 如果 x 是根（值为负）
        return x;
    } else {                                                   // 如果 x 不是根
        return find_no_path_compression(parent_or_size[x]);    // 递归查找其父节点
    }
}

// 查找操作（带路径压缩）
// 在查找过程中，将路径上的所有节点直接连接到根节点，从而扁平化树结构，提高后续查找效率。
int DisjSets::find(int x) {
    // 输入校验，这里x是对集合数组parent_or_size的索引
    if (x < 0 || x >= parent_or_size.size()) {
        std::cerr << "Error: Element " << x << " is out of bounds." << std::endl;
        return -1;                  // 或者抛出异常
    }

    if (parent_or_size[x] < 0) {    // 如果 x 是根
        return x;
    } else {
        // 递归查找根，并将路径上的所有节点直接连接到根
        // 赋值运算符=的返回值是左值引用（即最常见的引用类型）
        return parent_or_size[x] = find(parent_or_size[x]);
    }
}

// 合并操作：将两个根 root1 和 root2 所在的集合合并
// 采用按秩（高度）合并优化，以保持树的扁平性，防止退化为链表。
// root1 和 root2 必须是集合的根（通过 find 查找得到）
void DisjSets::unionSets(int root1, int root2) {
    // 确保 root1 和 root2 是合法的根索引
    // 实际应用中，通常在调用 unionSets 之前先调用 find 获取根
    if (root1 < 0 || root1 >= parent_or_size.size() || parent_or_size[root1] >= 0 || root2 < 0 || root2 >= parent_or_size.size() ||
        parent_or_size[root2] >= 0) {
        std::cerr << "Error: unionSets expects root nodes. Please call find() first." << std::endl;
        return;
    }

    if (root1 == root2) {    // 已经在同一个集合中，无需合并
        return;
    }

    // 按秩（高度）合并策略：将较矮的树连接到较高的树上
    // parent_or_size[root] 的负值表示高度的近似值（-1表示高度0，-2表示高度1，以此类推）
    if (parent_or_size[root2] < parent_or_size[root1]) {
        // root2 对应的集合更高（或更大）
        parent_or_size[root1] = root2;    // 将root1挂到root2下，高度不用变
    } else if (parent_or_size[root1] < parent_or_size[root2]) {
        // root1 对应的集合更高（或更大）
        parent_or_size[root2] = root1;    // 将root2挂到root1下，高度不用变
    } else {
        // 如果两个集合高度相同
        parent_or_size[root2] = root1;    // 将root2挂到root1下，顺序无所谓
        parent_or_size[root1]--;          // root1 的高度增加1
    }
}

// 获取当前不相交集合的数量
// 遍历 parent_or_size 向量，计算根节点的数量
int DisjSets::countSets() const {
    int count = 0;
    for (int val : parent_or_size) {
        if (val < 0) {    // 如果是根节点
            count++;
        }
    }
    return count;
}

int main() {
    // 创建一个包含 10 个元素的 DisjSets 对象
    DisjSets ds(10);

    std::cout << "初始状态，集合数量: " << ds.countSets() << std::endl;    // 10

    // 进行一些合并操作
    std::cout << "\n执行 unionSets 操作..." << std::endl;
    ds.unionSets(ds.find(0), ds.find(1));                                // 合并 0 和 1
    ds.unionSets(ds.find(2), ds.find(3));                                // 合并 2 和 3
    ds.unionSets(ds.find(0), ds.find(2));                                // 合并 (0,1) 和 (2,3) -> (0,1,2,3)
    ds.unionSets(ds.find(4), ds.find(5));                                // 合并 4 和 5
    ds.unionSets(ds.find(6), ds.find(7));                                // 合并 6 和 7
    ds.unionSets(ds.find(4), ds.find(6));                                // 合并 (4,5) 和 (6,7) -> (4,5,6,7)
    ds.unionSets(ds.find(8), ds.find(9));                                // 合并 8 和 9

    std::cout << "合并后，集合数量: " << ds.countSets() << std::endl;    // 理论上应该是 3 (0,1,2,3), (4,5,6,7), (8,9)

    // 查找元素所在的集合的根
    std::cout << "\n查找操作（带路径压缩）:" << std::endl;
    std::cout << "元素 0 的根: " << ds.find(0) << std::endl;
    std::cout << "元素 1 的根: " << ds.find(1) << std::endl;
    std::cout << "元素 2 的根: " << ds.find(2) << std::endl;
    std::cout << "元素 3 的根: " << ds.find(3) << std::endl;
    std::cout << "元素 4 的根: " << ds.find(4) << std::endl;
    std::cout << "元素 5 的根: " << ds.find(5) << std::endl;

    // 再次合并，看看效果
    std::cout << "\n再次执行 unionSets 操作..." << std::endl;
    ds.unionSets(ds.find(0), ds.find(4));                                    // 合并 (0,1,2,3) 和 (4,5,6,7)
    std::cout << "再次合并后，集合数量: " << ds.countSets() << std::endl;    // 理论上应该是 2 ((0-7), (8,9))

    std::cout << "元素 3 的新根: " << ds.find(3) << std::endl;
    std::cout << "元素 7 的新根: " << ds.find(7) << std::endl;

    // 检查元素是否在同一个集合中
    std::cout << "\n检查元素是否在同一个集合中:" << std::endl;
    if (ds.find(0) == ds.find(7)) {
        std::cout << "元素 0 和 7 在同一个集合中。" << std::endl;
    } else {
        std::cout << "元素 0 和 7 不在同一个集合中。" << std::endl;
    }

    if (ds.find(0) == ds.find(9)) {
        std::cout << "元素 0 和 9 在同一个集合中。" << std::endl;
    } else {
        std::cout << "元素 0 和 9 不在同一个集合中。" << std::endl;
    }

    return 0;
}
