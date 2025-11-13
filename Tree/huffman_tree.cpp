// 哈夫曼树基本概念
// 1. 路径：从根结点到结点的路径，直接数边就可以了
// 2. 结点的权：结点的权值通常是指该结点所代表的字符在原始数据中出现的频率
// 3. 结点的带权路径长度：从该结点到根之间路径长度与该结点的权值的乘积
// 4. 树的带权路径长度（WPL）：所有叶子结点的带权路径长度之和
// 5. 哈夫曼树：WPL最小的二叉树

// 在哈夫曼树中，所有被编码的字符都存储在叶子结点上，因此保证了每个被编码的哈夫曼编码都不是另一个编码的前缀
// 在不出现歧义的情况下，哈夫曼编码长度最短

// 构造哈夫曼树
// 1. 把有权值的叶子结点按从小到大排序
// 2.
// 取两个权值最小的结点，构造一个新的父结点N1，其权值为两个子结点的权值之和。对于这两个子结点，小的那个作为N1的左子结点，大的作为N1的右子结点
// 3. 把N1插入到原来的序列中，删除两个子结点
// 4. 重复步骤2和3，直到只剩下一个结点为止，这个结点就是哈夫曼树的根结点

// 哈夫曼编码
// 1. 从根结点到每个叶子结点的路径上，左分支记为0，右分支记为1
// 2. 每个叶子结点的编码就是从根结点到该叶子结点路径上所有分支的编码
// 3. 哈夫曼编码的特点是：编码长度与字符出现的频率成反比，频率越高，编码越短
// 4. 哈夫曼编码是前缀编码，即没有任何一个编码是另一个编码的前缀

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

        // 叶子节点构造函数
        HuffmanNode(char data, int freq)
            : data(data)
            , freq(freq)
            , left(nullptr)
            , right(nullptr) {}

        // 非叶子节点构造函数
        HuffmanNode(int freq, HuffmanNode* l, HuffmanNode* r)
            : data('\0')
            , freq(freq)
            , left(l)
            , right(r) {}

        // 判断是否是叶子节点
        bool isLeaf() const {
            return left == nullptr && right == nullptr;
        }
    };

    // 当priority_queue需要比较时，会创建一个CompareNodes临时对象，然后隐式调用仿函数
    struct CompareNodes {
        bool operator()(HuffmanNode* a, HuffmanNode* b) {
            return a->freq > b->freq;    // 最小堆用大于号
        }
    };

    HuffmanNode* root;                            // 哈夫曼树的根节点
    unordered_map<char, string> huffman_codes;    // 字符到编码的映射，记录每个字符的哈夫曼编码

    // 构建哈夫曼树
    void buildTree(const unordered_map<char, int>& frequencies) {
        priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes> min_heap;

        for (const auto& pair : frequencies) {
            // 有字符的都是叶子结点，这里调用构造函数也是叶子的重载版本
            min_heap.push(new HuffmanNode(pair.first, pair.second));
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
        if (node->isLeaf()) {
            huffman_codes[node->data] = code;
        }

        generateCodes(node->left, code + "0");
        generateCodes(node->right, code + "1");
    }

    // 注意，释放内存，能且仅能在后序遍历中进行
    // 必须先释放左右儿子结点，才能释放父节点
    void freeTree(HuffmanNode* node) {
        if (node == nullptr) {
            return;
        }
        freeTree(node->left);
        freeTree(node->right);
        delete node;
    }

public:
    // 根据文本构建哈夫曼树
    HuffmanTree(const string& text)
        : root(nullptr) {
        // 计算字符频率
        unordered_map<char, int> frequencies;    // 这个字典用来记录各个字符的频率
        for (char c : text) {
            frequencies[c]++;
        }

        // 构建哈夫曼树
        buildTree(frequencies);

        // 生成哈夫曼编码
        if (root) {    // 只有树构建成功才生成编码
            generateCodes(root, "");
        }
    }

    ~HuffmanTree() {
        freeTree(root);
        root = nullptr;
    }

    // 根据哈夫曼树中的编码表，来编码文本
    string encode(const string& text) {
        if (!root || huffman_codes.empty()) {    // 如果树为空，无法编码
            return "";
        }

        string encoded_text = "";
        for (char c : text) {
            if (huffman_codes.count(c)) {
                encoded_text += huffman_codes[c];
            } else {
                // 如果文本中有未出现在编码表中的字符
                return "";
            }
        }
        return encoded_text;
    }

    // 根据哈夫曼树中的编码表，来解码文本
    string decode(const string& encodedText) const {
        if (!root) {    // 如果树为空，无法解码
            return "";
        }

        string decodedText = "";
        HuffmanNode* current = root;

        // 特殊处理：如果树只有一个节点，即构建树的字符串中的字符全部相同
        if (root->isLeaf()) {
            for (char bit : encodedText) {    // 假设编码是重复的 '0' 或其他
                decodedText += root->data;
            }
            return decodedText;
        }

        for (char bit : encodedText) {
            if (bit == '0') {
                current = current->left;
            } else if (bit == '1') {
                current = current->right;
            } else {
                // 非法字符
                return "";
            }

            if (current && current->isLeaf()) {
                decodedText += current->data;
                current = root;    // 回到根节点，准备解码下一个字符
            } else if (current == nullptr) {
                // 路径中断，编码流有问题
                return "";
            }
        }

        // 确保解码在叶子节点结束，否则编码可能不完整
        if (current != root) {    // 如果解码结束后没有回到根节点，说明最后一个字符的编码不完整
            return "";
        }

        return decodedText;
    }

    // 打印哈夫曼编码表
    void printCodes() const {
        if (huffman_codes.empty()) {
            cout << "" << endl;
            return;
        }
        cout << "Huffman Codes:" << endl;
        for (auto const& pair : huffman_codes) {
            cout << "'" << pair.first << "': " << pair.second << endl;
        }
    }
};

int main() {
    string text;
    cin >> text;
    HuffmanTree huffmanTree(text);

    return 0;
}
