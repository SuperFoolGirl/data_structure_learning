#include <iostream>
#include <vector>
using namespace std;

class DisjSets
{
public:
    explicit DisjSets(int numElements);
    int find(int x) const;
    int find(int x);
    void unionSets(int root1, int root2);

private:
    vector<int> s;
};

DisjSets::DisjSets(int numElements) : s(numElements)
{
    for (int i = 0; i < s.size(); i++)
        s[i] = -1;
}

void DisjSets::unionSets(int root1, int root2)
{
    // 按高度求并
    // 数组元素值的相反数近似表示集合的高度(初始为-1，添加元素后-1，因此越小的父亲挂的儿子越多)。这里比较 s[root2] 和 s[root1]，如果 s[root2] 小于 s[root1]，意味着以 root2 为根的集合更高。此时，直接将 root1 所在集合挂载到 root2 所在集合下，也就是执行 s[root1] = root2;。这样能保证合并后的新集合高度不变，依然是较高集合的高度，有助于后续查找操作的效率提升，因为更浅的树结构查找根节点更快。
    if (s[root2] < s[root1])
        s[root1] = root2;
    else
    {
        // 当两个集合的高度相同时，无论把哪个集合挂载到另一个集合下，新集合的高度都会增加。此时先对 s[root1] 减 1 ，这是因为合并后新集合高度增加了，这里通过将根节点对应的值减 1 来更新高度信息。随后执行 s[root2] = root1;，把 root2 所在集合挂载到 root1 所在集合下，完成合并。
        if (s[root1] == s[root2])
            s[root1]--;
        s[root2] = root1;
    }
}

int DisjSets::find(int x) const
{
    if (s[x] < 0)
        return x;
    else
        return find(s[x]);
}

int main()
{
    return 0;
}