// 哈夫曼树基本概念
// 1. 路径：从根结点到结点的路径，直接数边就可以了
// 2. 结点的权：结点的权值通常是指该结点所代表的字符在原始数据中出现的频率
// 3. 结点的带权路径长度：从该结点到根之间路径长度与该结点的权值的乘积
// 4. 树的带权路径长度（WPL）：所有叶子结点的带权路径长度之和
// 5. 哈夫曼树：WPL最小的二叉树

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

#include <algorithm>    // for std::sort (if needed)
#include <iostream>
#include <map>
#include <queue>
#include <stdexcept>    // for std::runtime_error
#include <string>
#include <vector>

// 1. 定义哈夫曼树节点 (私有辅助结构)
struct HuffmanNode {
    char data;             // 字符数据（只在叶子节点有意义）
    int frequency;         // 频率（权重）
    HuffmanNode *left;     // 左子节点
    HuffmanNode *right;    // 右子节点

    // 构造函数 for leaf nodes
    HuffmanNode(char data_char, int freq) : data(data_char), frequency(freq), left(nullptr), right(nullptr) {}

    // 构造函数 for internal nodes
    HuffmanNode(int freq, HuffmanNode *l, HuffmanNode *r) : data('\0'), frequency(freq), left(l), right(r) {}

    // 判断是否是叶子节点
    bool isLeaf() const {
        return left == nullptr && right == nullptr;
    }
};

// 2. 比较器：用于 std::priority_queue (私有辅助结构)
// 我们需要一个最小堆，所以频率小的优先级高
// 而 std::priority_queue 默认是最大堆，因此我们需要自定义比较器
// 当priority_queue需要比较时，会创建一个CompareNodes临时对象，然后隐式调用仿函数
struct CompareNodes {
    bool operator()(HuffmanNode *a, HuffmanNode *b) {
        return a->frequency > b->frequency;    // 注意：'>' 表示最小堆
    }
};

// 3. HuffmanTree 类定义
class HuffmanTree {
private:
    HuffmanNode *root;                           // 哈夫曼树的根节点
    std::map<char, std::string> huffmanCodes;    // 字符到编码的映射，记录每个字符的哈夫曼编码

    // 私有辅助方法：构建哈夫曼树
    void buildTree(const std::map<char, int> &frequencies) {
        std::priority_queue<HuffmanNode *, std::vector<HuffmanNode *>, CompareNodes> minHeap;

        for (auto const &pair : frequencies) {
            // 有字符的都是叶子结点，这里调用构造函数也是叶子的重载版本
            minHeap.push(new HuffmanNode(pair.first, pair.second));
        }

        // 处理特殊情况：没有字符或只有一个字符
        if (minHeap.empty()) {
            root = nullptr;
            return;
        }
        if (minHeap.size() == 1) {
            HuffmanNode *singleNode = minHeap.top();
            minHeap.pop();
            // 为单字符创建虚拟父节点，确保能生成编码（例如 '0'）
            root = new HuffmanNode(singleNode->frequency, singleNode, nullptr);
            return;
        }

        while (minHeap.size() > 1) {
            // 每次取出两个频率最小的节点，创建一个新的父节点
            HuffmanNode *left = minHeap.top();
            minHeap.pop();
            HuffmanNode *right = minHeap.top();
            minHeap.pop();

            HuffmanNode *parentNode = new HuffmanNode(left->frequency + right->frequency, left, right);
            minHeap.push(parentNode);
        }
        // 这个就不是虚拟头结点了，而是根节点
        root = minHeap.top();
    }

    // 私有辅助方法：递归生成哈夫曼编码
    void generateCodes(HuffmanNode *node, std::string code) {
        if (node == nullptr) {
            return;
        }

        // 只有叶子结点才有字符数据，进而进行哈夫曼编码
        if (node->isLeaf()) {
            huffmanCodes[node->data] = code;
        }

        generateCodes(node->left, code + "0");
        generateCodes(node->right, code + "1");
    }

    // 私有辅助方法：递归释放树内存
    // 注意，释放内存，能且仅能在后序遍历中进行
    // 必须先释放左右儿子结点，才能释放父节点
    void freeTree(HuffmanNode *node) {
        if (node == nullptr) {
            return;
        }
        freeTree(node->left);
        freeTree(node->right);
        delete node;
    }

public:
    // 构造函数：根据文本构建哈夫曼树
    explicit HuffmanTree(const std::string &text) : root(nullptr) {
        if (text.empty()) {
            // cerr 这是一个警告，不是程序的正常输出
            // 它会立即显示在控制台，即使用户将程序的正常输出重定向到文件。
            // 即使程序崩溃，警告信息也会被显示。
            std::cerr << "Warning: Input text is empty. HuffmanTree will be empty." << std::endl;
            return;
        }

        // 1. 计算字符频率
        std::map<char, int> frequencies;    // 这个字典用来记录各个字符的频率
        for (char c : text) {
            frequencies[c]++;
        }

        // 2. 构建哈夫曼树
        buildTree(frequencies);

        // 3. 生成哈夫曼编码
        if (root) {    // 只有树构建成功才生成编码
            generateCodes(root, "");
        }
    }

    // 析构函数：释放树节点内存
    ~HuffmanTree() {
        freeTree(root);
        root = nullptr;    // 防止 dangling pointer
    }

    // 公共方法：编码文本
    std::string encode(const std::string &text) const {
        if (!root) {                   // 如果树为空，无法编码
            throw std::runtime_error("HuffmanTree is empty, cannot encode.");
        }
        if (huffmanCodes.empty()) {    // 编码表为空，通常意味着树只有一个根节点，但没有生成编码
            throw std::runtime_error("Huffman codes not generated, cannot encode.");
        }

        std::string encodedText = "";
        for (char c : text) {
            if (huffmanCodes.count(c)) {
                encodedText += huffmanCodes.at(c);
            } else {
                // 如果文本中有未出现在编码表中的字符
                throw std::runtime_error(std::string("Error: Character '") + c + "' not found in Huffman codes.");
            }
        }
        return encodedText;
    }

    // 公共方法：解码文本
    std::string decode(const std::string &encodedText) const {
        if (!root) {    // 如果树为空，无法解码
            throw std::runtime_error("HuffmanTree is empty, cannot decode.");
        }

        std::string decodedText = "";
        HuffmanNode *current = root;

        // 特殊处理：如果树只有一个节点（例如输入文本全是 'a'）
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
                throw std::runtime_error(std::string("Error: Invalid bit '") + bit + "' in encoded text.");
            }

            if (current && current->isLeaf()) {
                decodedText += current->data;
                current = root;    // 回到根节点，准备解码下一个字符
            } else if (current == nullptr) {
                // 路径中断，编码流有问题
                throw std::runtime_error("Error: Invalid Huffman code encountered during decoding (path ended prematurely).");
            }
        }

        // 确保解码在叶子节点结束，否则编码可能不完整
        if (current != root) {    // 如果解码结束后没有回到根节点，说明最后一个字符的编码不完整
            throw std::runtime_error("Error: Encoded text is incomplete or contains invalid trailing bits.");
        }

        return decodedText;
    }

    // 公共方法：打印哈夫曼编码表
    void printCodes() const {
        if (huffmanCodes.empty()) {
            std::cout << "No Huffman codes generated (tree might be empty)." << std::endl;
            return;
        }
        std::cout << "Huffman Codes:" << std::endl;
        for (auto const &pair : huffmanCodes) {
            std::cout << "'" << pair.first << "': " << pair.second << std::endl;
        }
    }

    // 公共方法：获取编码表
    const std::map<char, std::string> &getCodes() const {
        return huffmanCodes;
    }
};

// --- main 函数用于测试 ---
int main() {
    std::string text = "this is a huffman tree example";

    std::cout << "Original Text: \"" << text << "\"" << std::endl;

    try {
        // 创建 HuffmanTree 对象，自动构建树和编码
        HuffmanTree huffman_compressor(text);

        huffman_compressor.printCodes();
        std::cout << std::endl;

        // 编码文本
        std::string encodedText = huffman_compressor.encode(text);
        std::cout << "Encoded Text: " << encodedText << std::endl;
        std::cout << "Original size (bits): " << text.length() * 8 << std::endl;    // 假设ASCII，每字符8比特
        std::cout << "Encoded size (bits):  " << encodedText.length() << std::endl;
        std::cout << std::endl;

        // 解码文本
        std::string decodedText = huffman_compressor.decode(encodedText);
        std::cout << "Decoded Text: \"" << decodedText << "\"" << std::endl;

        // 验证解码结果
        if (text == decodedText) {
            std::cout << "Decoding successful: Original and decoded texts match." << std::endl;
        } else {
            std::cout << "Decoding failed: Original and decoded texts do NOT match." << std::endl;
        }

        // ----------------------------------------------------
        std::cout << "\n--- Testing with empty/single character text ---" << std::endl;
        std::string empty_text = "";
        std::string single_char_text = "aaaaa";

        // HuffmanTree empty_tree(empty_text); // 构造函数会打印警告，但不会抛异常

        HuffmanTree single_char_tree(single_char_text);
        single_char_tree.printCodes();    // 'a': 0 (或 "" 取决于构建细节)
        std::string encoded_single = single_char_tree.encode(single_char_text);
        std::cout << "Encoded single char text: " << encoded_single << std::endl;
        std::string decoded_single = single_char_tree.decode(encoded_single);
        std::cout << "Decoded single char text: " << decoded_single << std::endl;
        if (single_char_text == decoded_single) {
            std::cout << "Decoding successful for single char text." << std::endl;
        }

        // ----------------------------------------------------
        std::cout << "\n--- Testing error handling ---" << std::endl;
        HuffmanTree error_test_tree("abc");
        std::string valid_encoded = error_test_tree.encode("a");
        std::cout << "Valid encoded: " << valid_encoded << std::endl;

        // 尝试编码包含未知字符的文本
        try {
            error_test_tree.encode("abx");    // 'x' 未在原始文本中
        } catch (const std::runtime_error &e) {
            std::cerr << "Caught expected error during encoding: " << e.what() << std::endl;
        }

        // 尝试解码无效的比特流
        try {
            error_test_tree.decode("010111000");    // 可能会导致路径中断
        } catch (const std::runtime_error &e) {
            std::cerr << "Caught expected error during decoding: " << e.what() << std::endl;
        }

        // 尝试解码不完整的比特流
        try {
            error_test_tree.decode("01");    // 假设 '01' 不构成完整字符编码
        } catch (const std::runtime_error &e) {
            std::cerr << "Caught expected error during decoding (incomplete): " << e.what() << std::endl;
        }

    } catch (const std::runtime_error &e) {
        std::cerr << "Unhandled runtime error: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Unhandled standard exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unhandled unknown exception!" << std::endl;
    }

    return 0;
}
