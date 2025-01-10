#include <iostream>
using namespace std;

template <typename Comparable>
class BinarySearchTree
{
public:
    BinarySearchTree() : root(nullptr) {}

    BinarySearchTree(const BinarySearchTree &rhs)
    {
        *this = rhs;
    }

    ~BinarySearchTree()
    {
        makeEmpty();
    }

    const Comparable &findMin() const
    {
        return findMin(root);
    }

    const Comparable &findMax() const
    {
        return findMax(root);
    }

    bool contains(const Comparable &x) const
    {
        return contains(x, root);
    }
    bool isEmpty() const
    {
        return root == nullptr;
    }

    void printTree() const
    {
        printfTree(root);
    }

    void makeEmpty()
    {
        makeEmpty(root);
    }

    void insert(const Comparable &x)
    {
        insert(x, root);
    }
    void remove(const Comparable &x)
    {
        remove(x, root);
    }

    const BinarySearchTree &operator=(const BinarySearchTree &rhs)
    {
        if (this != &rhs)
        {
            makeEmpty();
            root = clone(rhs.root); // 通过单向链表的学习应该能意识到 只要给一个头节点 所有连接关系都是附带着的.但是这只是赋值而已。如果要克隆一个新的数据结构，那被克隆结构new出来的的空间肯定也要拷贝一份
        }
        return *this;
    }

private:
    struct BinaryNode
    {
        // 父子之间不需要是双向的 好好观察insert函数 每次从root遍历 总能找到要插入的地方
        Comparable &element;
        BinaryNode *left;
        BinaryNode *right;

        BinaryNode(const Comparable &theElement, BinaryNode *l, BinaryNode *r) : element(theElement), left(l), right(r) {}
    };

    BinaryNode *root;

    // 通过public单参数版本调用private双参数版本
    // 首先简单讲一下 指针的引用
    // C语言中接触过这样一个事实：给出变量a，如果想在函数里改变a的值，那么传参要传a的一级指针；如果想改变a的地址，则要传二级指针
    // 在swap函数里 想要改变a b两个指针的指向 只传一级指针是改变不了的
    // 这是因为非引用传参都是创建了一个副本
    // 而引用传参 没有副本 相当于直接对实参进行修改
    // 因此 如果实参是a b的一级指针 形参用一级指针的引用接受 那么我们就可以在函数里对一级指针进行任意全局修改，而不是需要传一个二级指针实参
    // 引用的强大主要就是在于 直接对实参进行操作 而不是借助副本

    // 想一下 如果这个题不传指针的引用，而是传指针会怎样？
    // 先回顾一下c语言的单向链表遍历 当时形参只是用node*接受 遍历函数里是while循环 而不是递归
    // head = head->next，对head进行修改了 如果改成尾递归的话 那这个head就失效了
    /*
    在 private 中 insert 函数的 BinaryNode *&t 参数使用指针引用，原因如下：
    1. 实现原地修改
        递归调用 insert 函数时，如果形参只是普通指针，那么每次函数调用传入的指针只是实参指针的一个副本。这意味着，在递归函数内部对这个指针副本所做的任何赋值操作，都不会影响到上层调用的指针。例如，当发现一个合适位置（t 为 nullptr）需要插入新节点时，若形参是普通指针，t = new BinaryNode(x, nullptr, nullptr); 这一赋值语句仅仅改变了副本指针的指向，而不会真正更新上层调用的指针，导致无法将新节点链接到树中。
        使用指针引用，就相当于直接操作传入的指针本身，没有额外的副本。所以当找到插入位置时，t = new BinaryNode(x, nullptr, nullptr); 这句代码能够真正改变调用处的指针，新节点就被正确地接入到树结构里。
    2. 与递归调用适配
        二叉搜索树的插入操作是递归的，整个递归过程依赖于对当前节点指针的持续更新与追踪。要是形参只是指针，递归回溯时，各层之间无法传递指针变动的信息，因为每次递归调用产生的新指针副本在函数结束时就销毁了，无法影响到外层函数。而指针引用能让递归调用链中的每一层，都精准修改同一个指针，维持树结构的一致性，保障递归逻辑可以按预期工作，将新元素层层传递，插入到正确的位置。
    3. 简化代码逻辑
        如果不用指针引用，要达成同样的修改效果，可能得借助返回值，把新的指针层层返回到调用处，代码会变得更复杂，可读性降低。采用指针引用，代码能够更直观地表达意图，清晰展现树节点指针在递归过程中的更新过程。
    */
    void insert(const Comparable &x, BinaryNode *&t) // * &t,指针的引用
    {
        if (t == nullptr)
            t = new BinaryNode(x, nullptr, nullptr);
        else if (x < t->element)
            insert(x, t->left);
        else if (t->element < x)
            insert(x, t->right);
        else
            return; // 不能重复
    }

    void remove(const Comparable &x, BinaryNode *&t) const
    {
        if (t == nullptr)
            return;

        if (x < t->element)
            remove(x, t->left);
        else if (t->element < x)
            remove(x, t->right);
        else if (t->left != nullptr && t->right != nullptr) // 已经递归到要删除的节点了 但该节点有两个儿子
        {
            t->element = findMin(t->right)->element;
            remove(t->element, t->right);
        }
        else // 最后总能走到这
        // 如果没走过上一个else if 即要删的节点只有一个儿子 那就直接删
        // 如果走过上一个else if 即要删的节点有两个儿子 那么此时这两个节点的element值相等(x的值被改为目标节点后的最小值 故一定会不断往左走) 要删掉的不是目标节点 反而是被目标节点copy了element的那个最小值节点
        {
            BinaryNode *oldNode = t;
            t = (t->left != nullptr) ? t->left : t->right; // t现在必然只有一个儿子 由于t被父节点接着 把t改成自己的儿子，就实现了t的父亲接着t的儿子 然后美美删掉t 结束
            delete oldNode;
        }
    }

    // 本函数没有对t进行改动 但由于写成了递归 因此依然选择使用指针引用
    // 递归函数依赖层层嵌套的调用关系来处理数据结构。对于二叉搜索树找最小节点这种操作，从根节点出发，不断深入左子树，指针引用让每一层递归的返回信息简单且直接 —— 只需把当前指针更新为左子树的最小节点指针。要是没有指针引用，每层递归都得新开炉灶处理返回值，递归代码的连贯性和简洁性就被破坏了。
    // 在递归调用过程中，每一层函数调用共享查找最小节点这个任务。使用指针引用，能让所有层级的函数都操作同一个指针变量。从顶层调用开始，一路深入到最底层找到最小节点，这个过程中，指针引用保证了每一步对指针的修改，都是针对同一个真实的指针，没有产生副本。打个比方，这就像是一群人接力找东西，大家传递的不是东西的副本，而是直接把东西本身（指针）递来递去，最后找到目标位置时，最初拿东西的人手里的东西也同步更新了，不需要额外交接。(虽然还得走递归那一套流程，但明显效率快了很多)
    // 总的来说 这里选择使用指针引用 更多在于隐性地提高效率。但前提是返回类型改为void并写成递归，显然这里写得是有问题的
    // 返回值也是副本。如果返回类型是void的话 就可以避免每次返回副本 从而提高效率。
    // 虽然 findMin 函数没有在递归过程中修改树结构，但使用指针引用可以简化找到最小元素后的返回方式。如果不使用指针引用，每次递归就得返回一个 BinaryNode* 指针，在多层递归嵌套时，外层函数需要不断追踪内层返回的指针，代码会变得繁琐。使用指针引用，直接修改传入的指针，当递归结束时，在void类型下，调用者传入的指针就已经指向最小元素节点，后续获取该节点元素的操作更加直接顺畅。
    BinaryNode *findMin(BinaryNode *&t) const
    {
        if (t == nullptr)
            return nullptr;
        if (t->left == nullptr)
            return t;
        return findMin(t->left);
    }

    BinaryNode *findMax(BinaryNode *&t) const
    {
        if (t != nullptr)
        {
            while (t->right != nullptr)
                t = t->right;
            return t;
        }
        else
            return nullptr;
    }

    // 并不是只要有递归就要用指针引用，返回值副本的开销其实不大，返回BN指针类型主要是麻烦，不容易追踪，毕竟每层都有一个副本指针，想想就恶心。但int这样的基本类型就简单了
    // 提升效率不能脱离函数的实际功能。contains 函数核心在于遍历二叉搜索树，对比节点元素判断目标元素是否存在，没有涉及修改指针来更新树结构或者跨层级共享指针变动信息的需求。要是仅仅为了所谓的 “避免副本” 强加上指针引用，会让代码逻辑背离其清晰的查找功能，得不偿失。
    bool contains(const Comparable &x, BinaryNode *t) const
    {
        if (t == nullptr)
            return false;
        else if (x < t->element)
            return contains(x, t->left);
        else if (t->element < x)
            return contains(x, t->right);
        else
            return true;
    }

    void makeEmpty(BinaryNode *&t)
    {
        // 遍历一下这两句递归 先往左边走 到底了之后归 如果本节点有右儿子，就进入其分支，删掉它后面所有的儿子然后回来，再删掉本节点；如果没有右儿子就直接删掉本节点然后递归回上一级。回到根节点后（栈底函数），删完了左子树，然后栈底函数执行第二句，来删右子树。最后delete掉根节点 结束
        if (t != nullptr)
        {
            makeEmpty(t->left);
            makeEmpty(t->right);
            delete t;
        }
        t = nullptr; // 置为空指针。把t指向的内存delete掉之后 p成为野指针 要置空一下的.对于递归过程中的子节点们，也是一样的，不止root需要走这一句
    }

    void printTree(BinaryNode *t) const
    {
        // 前序 中序 后序
    }

    BinaryNode *clone(BinaryNode *t) const
    {
        if (t == nullptr)
            return nullptr;

        // 树魅力时刻 递归思路参考makeEmpty函数 或者让ai生成一个可视化描述
        return new BinaryNode(t->element, clone(t->left), clone(t->right));
    }
};

int main()
{
    BinarySearchTree<int> bst;
    return 0;
}