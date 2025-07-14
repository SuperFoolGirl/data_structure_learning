// 二叉搜索树
// 所有的左子树都小于根结点，所有的右子树都大于根结点

// 适用情况：需要频繁插入、删除、查找操作的场景
// 虽然有序数组可以频繁进行二分查找，但是插入删除效率很低
// 二叉搜索树就是在二分查找思想的基础上，结合了链表的灵活性，提供了一种高效的动态数据结构
// 按照中序遍历的方式，可以得到一个有序的元素序列

#include <iostream>

template<typename Comparable>
class BinarySearchTree {
public:
    BinarySearchTree() : root(nullptr) {}

    BinarySearchTree(const BinarySearchTree &rhs) {
        // 委托给赋值函数了（而赋值函数又委托给clone了）
        *this = rhs;
    }

    ~BinarySearchTree() {
        makeEmpty();
    }

    // 明确为什么要使用重载函数来组织这个方法
    // 因为不想让用户传参root
    // 但是提出一个问题：root是私有成员，让findMin直接用不就行了？为什么私有重载findMin还要带参数t？
    // 因为其他方法里可能会用到私有重载findMin，不一定仅起到查找最小值的作用
    // 比如查找某子树的最小树呢？
    // 不得不说，面向对象程序设计的灵活性还是值得体会的
    const Comparable &findMin() const {
        return findMin(root);
    }

    const Comparable &findMax() const {
        return findMax(root);
    }

    bool contains(const Comparable &x) const {
        return contains(x, root);
    }

    bool isEmpty() const {
        // root不是哨兵，它本身就是带数据的根
        return root == nullptr;
    }

    void printTree() const {
        printTree(root);
    }

    void makeEmpty() {
        makeEmpty(root);
    }

    void insert(const Comparable &x) {
        insert(x, root);
    }

    void remove(const Comparable &x) {
        remove(x, root);
    }

    const BinarySearchTree &operator=(const BinarySearchTree &rhs) {
        if (this != &rhs) {
            makeEmpty();
            // 通过单向链表的学习应该能意识到 只要给一个头节点
            // 所有连接关系都是附带着的.但是这只是赋值而已。如果要克隆一个新的数据结构，那被克隆结构new出来的的空间肯定也要拷贝一份
            root = clone(rhs.root);
        }
        return *this;
    }

private:
    struct BinaryNode {
        // 数据结构中，结点中的数据一般都是以值方式存储的
        // 值方式是组合的逻辑，可以自动管理Comparable对象的生命周期
        Comparable element;
        BinaryNode *left;
        BinaryNode *right;

        BinaryNode(const Comparable &theElement, BinaryNode *l, BinaryNode *r)
            : element(theElement), left(l), right(r) {}
    };

    BinaryNode *root;

    // 删除函数能且仅能写成后序遍历的形式
    // 只能先删除左右子树，再删除根结点，否则就会非法访问
    void makeEmpty(BinaryNode *&t) {
        if (t != nullptr) {
            makeEmpty(t->left);
            makeEmpty(t->right);
            delete t;
            t = nullptr;
        }
    }

    // 二叉搜索树的插入逻辑，需要保证中序遍历的结果是升序
    // 讲一下为什么要传指针引用
    // 首先明确下指针引用的作用：指针引用允许我们在函数内部直接修改传入的指针
    // 主要原因在于t会被赋值给new BinaryNode(x, nullptr, nullptr);
    // 如果不使用指针引用，t会是一个副本，修改它不会影响原来的树结构

    // 总结一下：只要是修改指针的值——也就是指针的指向，那就需要指针引用了
    // 但是，别忘了原理：
    // 指针作为参数传递时，对于实参的指针来说，它是“值传递”
    // 虽然形参指针和实参指针指向同一个内存地址，但这两个指针的地址是不同的
    // 如果是通过值传递的指针，对指向的对象进行修改，那是可以的
    // 如果指向的是堆区内存，那么也可以在函数内部释放，这都没问题
    // 唯独如果想要改变指针指向，这一点做不到

    // 这里，再稍微总结下引用的作用：
    // 1. 传参效率高，尤其是大对象处理递归问题
    // 2. 在函数内部修改引用，可以直接修改原对象的值
    // 3. 对于第2点，在C语言的函数中，频繁解引用，在操作上是很麻烦的。因此引用带来了便利
    void insert(const Comparable &x, BinaryNode *&t)    // * &t,指针的引用
    {
        if (t == nullptr) {
            t = new BinaryNode(x, nullptr, nullptr);    // 此处t是指针引用，直接修改原树结构
        } else if (x < t->element) {
            insert(x, t->left);
        } else if (t->element < x) {
            insert(x, t->right);
        } else {
            return;    // 不允许有重复元素
        }
    }

    void remove(const Comparable &x, BinaryNode *&t) const {
        if (t == nullptr) {
            return;
        }

        if (x < t->element) {
            remove(x, t->left);
        } else if (t->element < x) {
            remove(x, t->right);
        } else if (t->left != nullptr && t->right != nullptr) {
            // 已经递归到要删除的结点了 但该结点有两个儿子
            // 需要安置好儿子结点的位置，才能把自己删掉
            // 安置方法：把自己的位置换成右子树的最小值（左子树的最大值也可以，都是合法的）
            // 合法原因是：中序遍历后，左子树max和右子树min分别为待删点的前驱和后继
            // 然后删掉原本的右子树最小值结点
            t->element = findMin(t->right)->element;
            remove(t->element, t->right);
        } else {
            // 最后总能走到这
            // 此时t为要删除的结点，且只有一个儿子或没有儿子
            // 以下逻辑可参考视频：
            // 如果只有一个子树，那就直接让那个子树代替自己，不管左右
            // 如果没有子树，那就直接删除自己
            BinaryNode *oldNode = t;
            // 如果有左子树，就让t指向左子树，否则就让t指向右子树
            t = (t->left != nullptr) ? t->left : t->right;
            delete oldNode;
        }
    }

    // 这个方法用不到指针引用了，递归写法
    BinaryNode *findMin(BinaryNode *t) const {
        if (t == nullptr) {
            return nullptr;
        }
        if (t->left == nullptr) {
            return t;
        }
        return findMin(t->left);
    }

    // 改成循环写法，出现了t的赋值，因此需要指针引用
    BinaryNode *findMax(BinaryNode *&t) const {
        if (t != nullptr) {
            while (t->right != nullptr) {
                t = t->right;
            }
            return t;
        } else {
            return nullptr;
        }
    }

    bool contains(const Comparable &x, BinaryNode *t) const {
        if (t == nullptr) {
            return false;
        } else if (x < t->element) {
            return contains(x, t->left);
        } else if (t->element < x) {
            return contains(x, t->right);
        } else {
            return true;
        }
    }

    void printTree(BinaryNode *t) const {
        // 中序遍历打印二叉搜索树，会正好得到升序排列的元素
        if (t != nullptr) {
            printTree(t->left);
            std::cout << t->element << " ";
            printTree(t->right);
        }
    }

    BinaryNode *clone(BinaryNode *t) const {
        if (t == nullptr) {
            return nullptr;
        }
        return new BinaryNode(t->element, clone(t->left), clone(t->right));
        // 创建树的代码顺序是从上到下，从根到儿子
        // 先克隆左子树，再克隆右子树，最后返回新结点

        // 但具体生成的时候，其实是按后序的顺序生成的
        // 因为必须先有了左右子树，才能生成根结点
    }
};

int main() {
    BinarySearchTree<int> bst;

    // 测试函数
    bst.insert(5);
    bst.insert(3);
    bst.insert(7);
    bst.insert(2);
    bst.insert(4);
    bst.insert(6);
    bst.insert(8);
    std::cout << "In-order traversal: ";
    bst.printTree();    // 应该输出：2 3 4 5 6 7 8

    return 0;
}
