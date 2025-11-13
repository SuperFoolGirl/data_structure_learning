/*
编写一个哈夫曼编码译码程序。针对一段文本，根据文本中字符出现频率构造哈夫曼树，给出每个字符的哈夫曼编码，并进行译码，计算编码前后文本大小。
为确保构建的哈夫曼树唯一，本题做如下限定：

1. 选择根结点权值最小的两棵二叉树时，选取权值较小者作为左子树。
2. 若多棵二叉树根结点权值相等，则先生成的作为左子树，后生成的作为右子树，具体来说：
   i) 对于单结点二叉树，优先选择根结点对应字母在文本中最先出现者，如文本为cba，三个字母均出现1次，但c在文本中最先出现，b第二出现，故则选择c作为左子树，b作为右子树。
   ii) 对于非单结点二叉树，先生成的二叉树作为左子树，后生成的二叉树作为右子树。
   iii) 若单结点和非单结点二叉树根结点权值相等，优先选择单结点二叉树。 
3. 生成哈夫曼编码时，哈夫曼树左分支标记为0，右分支标记为1。

输入格式:
输入为3行。第1行为一个字符串，包含不超过5000个字符，至少包含两个不同的字符，每个字符为a-z的小写字母。第2、3行为两个由0、1组成的字符串，表示待译码的哈夫曼编码。

输出格式:
输出第一行为用空格间隔的2个整数，分别为压缩前后文本大小，以字节为单位，一个字符占1字节，8个二进制位占1字节，若压缩后文本不足8位，则按1字节算。输出从第二行开始，每行为1个字符的哈夫曼编码，按各字符在文本中出现次数递增顺序输出，若多个字符出现次数相同，则按其在文本出现先后排列。每行格式为“字母:编码”。最后两行为两行字符串，表示译码结果，若译码失败，则输出INVALID。
*/

#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class HuffmanTree {
private:
    // 哈夫曼树节点
    struct HuffmanNode {
        char data;             // 字符数据（只在叶子节点有意义）
        int freq;              // 频率（权重）
        HuffmanNode* left;     // 左子节点
        HuffmanNode* right;    // 右子节点
        int id;                // 插入顺序ID，用于稳定排序
        static int node_id;    // 用于记录字符插入顺序的静态变量，使得堆排序能够实现稳定排序

        // 叶子节点构造函数
        HuffmanNode(char data, int freq)
            : data(data)
            , freq(freq)
            , left(nullptr)
            , right(nullptr)
            , id(++node_id) {}

        // 非叶子节点构造函数
        HuffmanNode(int freq, HuffmanNode* l, HuffmanNode* r)
            : data('\0')
            , freq(freq)
            , left(l)
            , right(r)
            , id(++node_id) {}
    };

    // 当priority_queue需要比较时，会创建一个CompareNodes临时对象，然后隐式调用仿函数
    struct CompareNodes {
        bool operator()(HuffmanNode* a, HuffmanNode* b) {
            if (a->freq != b->freq) {
                return a->freq > b->freq;    // 最小堆用大于号，大于号指向的是低优先级，会排在后面
            } else {
                return a->id > b->id;        // 频率相同则按插入顺序比较，保证稳定排序
            }
        }
    };

private:
    HuffmanNode* root;                            // 哈夫曼树的根节点
    unordered_map<char, string> huffman_codes;    // 字符到编码的映射，记录每个字符的哈夫曼编码
    string text;                                  // 原始文本
    string order;                                 // 根据题目要求，对text进行加工、排序

    // 构建哈夫曼树
    void buildTree(const unordered_map<char, int>& frequencies) {
        priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes> min_heap;

        // 题目要求，对于堆，排序第一关键字：频率，第二关键字：字符在文本中首次出现的顺序
        // 这恰好是order字符串的顺序
        for (char c : order) {
            // 都是叶子节点
            HuffmanNode* new_node = new HuffmanNode(c, frequencies.at(c));
            min_heap.push(new_node);
        }

        // 没有字符
        if (min_heap.empty()) {
            return;
        }
        // 只有一个字符
        if (min_heap.size() == 1) {
            HuffmanNode* single_node = min_heap.top();
            min_heap.pop();
            // 单字符节点设为root
            root = new HuffmanNode(single_node->freq, single_node, nullptr);
            return;
        }

        // 自底向上构建哈夫曼树
        while (min_heap.size() > 1) {
            // 每次取出两个频率最小的节点，创建一个新的父节点
            // 左子树频率小于右子树
            HuffmanNode* left = min_heap.top();
            min_heap.pop();
            HuffmanNode* right = min_heap.top();
            min_heap.pop();

            // 都是非叶子节点
            HuffmanNode* parent_node = new HuffmanNode(left->freq + right->freq, left, right);
            min_heap.push(parent_node);
        }
        // 最后堆中剩下的节点就是哈夫曼树的根节点
        root = min_heap.top();
    }

    // 生成哈夫曼编码
    void generateCodes(HuffmanNode* node, string code) {
        if (node == nullptr) {
            return;
        }

        // 递归中如果遇到了叶子节点，就将当前编码存储下来
        if (isLeaf(node)) {
            huffman_codes[node->data] = code;
        }

        generateCodes(node->left, code + "0");
        generateCodes(node->right, code + "1");
    }

    // 必须先释放左右儿子结点，才能释放父节点
    void freeTree(HuffmanNode* node) {
        if (node == nullptr) {
            return;
        }
        freeTree(node->left);
        freeTree(node->right);
        delete node;
    }

    void init() {
        // 计算字符频率
        unordered_map<char, int> frequencies;    // 这个字典用来记录各个字符的频率
        for (char c : text) {
            frequencies[c]++;
        }

        // 求order字符串
        // 要求：组成元素为text中出现的字符（去重），排序第一关键字：频率，第二关键字：字符在文本中首次出现的顺序
        // 1. 去重
        unordered_map<char, bool> seen;
        for (char c : text) {
            if (!seen[c]) {
                seen[c] = true;
                order += c;
            }
        }
        // 2. 排序
        // lambda表达式，需要捕获frequencies和text，用[&]按引用捕获表达式作用域中的所有外部变量
        sort(order.begin(), order.end(), [&](char a, char b) {
            if (frequencies[a] != frequencies[b]) {
                return frequencies[a] < frequencies[b];
            } else {
                // string的find函数返回字符第一次出现的位置索引
                return text.find(a) < text.find(b);
            }
        });

        // 构建哈夫曼树
        buildTree(frequencies);

        // 生成哈夫曼编码
        if (root) {    // 只有树构建成功才生成编码
            generateCodes(root, "");
        }
    }

    bool isLeaf(HuffmanNode* node) const {
        return node && node->left == nullptr && node->right == nullptr;
    }

public:
    // 根据文本构建哈夫曼树
    HuffmanTree(const string& text)
        : root(nullptr)
        , text(text)
        , order("") {
        init();
    }

    ~HuffmanTree() {
        freeTree(root);
        root = nullptr;
    }

    // 根据哈夫曼树中的编码表，来解码文本
    string decode(const string& encoded_text) const {
        if (!root) {    // 如果树为空，无法解码
            return "";
        }

        string decoded_text = "";
        HuffmanNode* curr = root;

        // 如果树只有一个节点，即构建树的字符串中的字符全部相同
        if (isLeaf(root)) {
            // append方法，添加多个相同字符
            decoded_text.append(text.size(), root->data);
            return decoded_text;
        }

        for (char bit : encoded_text) {
            if (bit == '0') {
                curr = curr->left;
            } else if (bit == '1') {
                curr = curr->right;
            } else {
                // 非法字符
                return "";
            }

            if (isLeaf(curr)) {
                decoded_text += curr->data;
                curr = root;    // 回到根节点，准备解码下一个字符
            }
        }

        // 如果解码结束后没有回到根节点，说明最后一个字符的编码不完整
        if (curr != root) {
            return "INVALID";
        }

        return decoded_text;
    }

    // 打印哈夫曼编码表
    void printCodes() const {
        // 按照order的顺序输出
        for (char c : order) {
            cout << c << ":" << huffman_codes.at(c) << endl;
        }
    }

    void solve() const {
        // 输出压缩前大小
        cout << text.size() << " ";

        // 输出压缩后大小
        int res = 0;
        for (char c : text) {
            res += huffman_codes.at(c).size();
        }
        res = (res + 7) / 8;    // 向上取整，转换为字节数
        cout << res << endl;

        // 打印哈夫曼编码表
        printCodes();

        // 解码
        for (int i = 0; i < 2; ++i) {
            string encoded_text;
            cin >> encoded_text;
            cout << decode(encoded_text) << endl;
        }
    }
};

// 初始化静态变量
int HuffmanTree::HuffmanNode::node_id = 0;

int main() {
    string text;
    cin >> text;
    HuffmanTree huffman_tree(text);

    huffman_tree.solve();

    return 0;
}
