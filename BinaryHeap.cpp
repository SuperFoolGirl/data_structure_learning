#include <iostream>
#include <vector>
using namespace std;

// 每一个节点下面所有的儿子都要大于本节点的值 根节点是最小的
template<class Comparable>
class BinaryHeap
{
public:
    explicit BinaryHeap(int capacity = 100) {}

    // 注意 这不是拷贝构造函数 形参里没有BH类名 因此我们认为item是无序的 需要使用bulid函数来使其得到BH的顺序
    explicit BinaryHeap(const vector<Comparable> &items) : array(items.size() + 10), currentSize(items.size())
    {
        for (int i = 0; i < items.size(); i++)
            array[i + 1] = items[i];
        buildHeap();
    }

    bool isEmpty() const
    {

    }

    const Comparable &findMin() const
    {

    }

    void insert(const Comparable &x)
    {
        if (currentSize == array.size() - 1)
            array.resize(array.size() * 2);
        int hole = ++currentSize; // 堆是把树放在数组里的 这里暂时把插入元素放到最后一个位置 这个存放顺序决定了树在同一层是从左向右存的
        // 上滤
        // 只要hole大于 1（即新元素不在根节点位置），并且新元素x小于其父节点array[hole/2]，就把父节点下移到当前位置array[hole]
        // 注意赋值的左值右值 想要下移父节点 是把下面的节点赋值为父节点，然后父节点暂时不用动，重复问题最后一次出循环会解决 类比插入排序
        for (; hole > 1 && x < array[hole / 2]; hole /= 2)
            array[hole] = array[hole / 2];
        array[hole] = x; // 出来后执行了hole/=2，最后一次循环执行下移时的父节点了
    }

    void deleteMin()
    {
        // 如果堆为空，尝试删除元素是非法操作，此时抛出一个UnderflowException异常，告知调用者堆里已经没有元素可供删除。
        if (isEmpty())
            throw UnderflowException();
        // 先把最后一个元素 即最下层最靠右边那个抬上来，然后下滤
        array[1] = array[currentSize--]; // arr[0]没用 索引从1开始 为了便于计算，把树存在数组里。size--正好删掉了一个头
        percolateDown(1);
    }

    void deleteMin(Comparable &minItem) // 要对形参动手 不能再const了
    {
        if (isEmpty())
            throw UnderflowException();
        minItem = array[1]; //minItem是引用传参的 调用方可以直接得到改变 不需要返回值
        array[1] = array[currentSize--];
        percolateDown(1); 
    }

    void makeEmpty()
    {

    }

    void decreaseKey(int pos, int delta)
    {

    }

    void increaseKey(int pos, int key)
    {

    }

    void remove(int pos)
    {
        decreaseKey(pos, array[pos]);
        deleteMin();
    }

private:
    int currentSize;
    vector<Comparable> array;

    void buildHeap()
    {
        // 循环从i = currentSize / 2开始。对于一个完全二叉树（用数组表示的二叉堆通常是完全二叉树），它的非叶子节点编号范围是从1到currentSize / 2。叶子节点不需要进行下滤操作，因为它们没有子节点，天然满足堆的性质。从currentSize / 2 开始，倒序遍历到1，可以覆盖所有有子节点的节点。
        // 加入一共有4层 即15个节点 前三层15/2 = 7 = pow(2,3)-1，最后一层全是叶子节点 且占了另外一半，因为pow(2,3)*2=pow(2,4)（加一减一就不细究了）
        for (int i = currentSize / 2; i > 0; i--)
            percolateDown(i); // 下滤充分利用了完全二叉树底层节点多的特性。在完全二叉树里，越靠近底层，节点数量越多 ，下滤能快速把底层不符合堆性质的大元素 “归位”，使得堆快速成型。相比上滤，性能更好
    }

    void percolateDown(int hole)
    {
        int child;
        Comparable tmp = array[hole];
        for (; hole * 2 <= currentSize; hole = child) // 这个判断条件是合理的。如果一个节点有右儿子，根据最小二叉堆的性质 它一定有左儿子，反之则不成立；更新条件，把hole更新为上一个循环里上移的子节点
        {
            // 1. 找到当前节点的左子节点
            child = hole * 2;
            // 2. 如果右子节点存在且比左子节点小，选择右子节点。如果右边小，我们却让左边上来，那就破坏了最小二叉堆
            // hole从根节点开始，可能不断左偏右偏，直到适合他的位置。
            if (child != currentSize && array[child + 1] < array[child])
                child++;

            // 3. 如果子节点比待调整的元素小，将子节点上移
            if (array[child] < tmp)
                array[hole] = array[child]; // hole这个时候还是根，child是左右中较小的那个儿子
            // 4. 如果子节点不小于待调整的元素，说明找到了合适位置，退出循环
            else
                break;
        }
        array[hole] = tmp; // 如果是break出来的 没有走更新条件；如果是因为不满足for循环判断条件出来的，那正好处理重复问题
    }
};

int main()
{
    return 0;
}