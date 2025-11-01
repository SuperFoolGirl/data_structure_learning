// 计算中缀表达式时，通常会先将其转换为后缀表达式再进行计算。
// 因为后缀表达式计算比较简单
// 而中缀表达式计算需要考虑运算符优先级和括号等问题，比较复杂。

// 规则：
// 1. 遇到操作数，直接输出到后缀表达式
// 2. 遇到左括号，压入栈
// 3. 遇到右括号，弹出栈顶运算符输出到后缀表达式，直到遇到第一个左括号为止，左括号丢弃
// 4. 运算符
//    a. 如果栈为空，或者栈顶是左括号，直接压入栈
//    b. 如果当前运算符的优先级大于栈顶运算符（或优先级相同，但是具有右结合性），直接压入栈
//    c.
//    否则，弹出栈顶运算符到后缀表达式，直到栈为空或者栈顶运算符的优先级小于当前运算符的优先级，然后将当前运算符压入栈
// 5. 扫描完毕后，弹出栈中所有运算符到后缀表达式

#include <iostream>
#include <sstream>
#include <stack>
#include <vector>

using namespace std;

int getPrecedence(const string& op) {
    if (op == "+" || op == "-") {
        return 1;
    } else if (op == "*" || op == "/" || op == "%") {
        return 2;
    } else if (op == "^") {
        return 3;
    }

    return 0;
}

bool isRightAssociative(const string& op) {
    return op == "^";
}

bool isOperator(const string& token) {
    return token == "+" || token == "-" || token == "*" || token == "/" || token == "^" || token == "%";
}

long long fastPow(long long base, long long exp) {
    long long res = 1;

    if (exp < 0) {
        return 0;    // 负指数情况下，返回0表示错误（整数运算中不处理分数）
    }

    while (exp) {
        if (exp & 1) {
            res *= base;
        }
        base *= base;
        exp >>= 1;
    }

    return res;
}

vector<string> tokenizeInfixExpression(const string& expr) {
    // 需要正确处理多位数，以及负号；各元素放入tokens中后返回
    vector<string> tokens;
    // 正确解析多位数字
    string curr_num;
    // 标记负号为一元运算符
    bool negative = false;

    for (size_t i = 0; i < expr.size(); ++i) {
        char ch = expr[i];

        if (isdigit(ch)) {
            // 如果是数字，就继续构建当前数字
            curr_num += ch;
        } else {
            // 非数字，则将当前数字入tokens（如果有的话）
            if (!curr_num.empty()) {
                if (negative) {
                    curr_num = "-" + curr_num;
                }
                tokens.push_back(curr_num);
                curr_num.clear();
                negative = false;
            }

            // 处理负号作为一元还是二元运算符
            // 如果是一元，则满足以下要求：
            // 1. 处于表达式开头
            // 2. 前一个字符是左括号
            // 3. 前一个字符是二元运算符
            // isOperator需要string类型参数，这里需要调用构造函数
            if (ch == '-' && (i == 0 || expr[i - 1] == '(' || isOperator(string(1, expr[i - 1])))) {
                negative = true;
                continue;
            }

            // 处理其他运算符
            if (isOperator(string(1, ch)) || ch == '(' || ch == ')') {
                tokens.push_back(string(1, ch));
            } else {
                // 遇到非法字符，忽略
                continue;
            }
        }
    }
    // 循环结束时，最后一个数字还留在curr_num中
    if (!curr_num.empty()) {
        if (negative) {
            curr_num = "-" + curr_num;
        }
        tokens.push_back(curr_num);
    }

    return tokens;
}

vector<string> infixToPostfix(const vector<string>& infix_tokens, bool& is_valid) {
    vector<string> postfix_tokens;    // 结果列表
    stack<string> op_stack;           // 运算符栈

    for (const string& token : infix_tokens) {
        if (isdigit(token[0]) || (token.size() > 1 && token[0] == '-' && isdigit(token[1]))) {
            // 处理操作数，包括含负号的操作数
            postfix_tokens.push_back(token);
        } else if (token == "(") {
            // 左括号直接入栈
            op_stack.push(token);
        } else if (token == ")") {
            // 右括号，弹出栈顶运算符直到遇到第一个左括号
            while (!op_stack.empty() && op_stack.top() != "(") {
                postfix_tokens.push_back(op_stack.top());
                op_stack.pop();
            }
            // 弹出左括号但不加入结果
            if (!op_stack.empty() && op_stack.top() == "(") {
                op_stack.pop();
            } else {
                // 没有找到匹配的左括号，表达式非法
                is_valid = false;
                return {};
            }
        } else if (isOperator(token)) {
            // 处理运算符
            while (!op_stack.empty() && op_stack.top() != "(") {
                // 当前栈非空，且栈顶不是左括号
                // 比较优先级
                int curr_prec = getPrecedence(token);
                int top_prec = getPrecedence(op_stack.top());

                if (top_prec > curr_prec || (top_prec == curr_prec && !isRightAssociative(token))) {
                    // 当前比较菜，需要让牛的先弹
                    // 对于右结合性的运算符，可等价为高优先级
                    // 如果栈顶为^，且当前也是^，当前的优先级更高
                    postfix_tokens.push_back(op_stack.top());
                    op_stack.pop();
                } else {
                    // 当前运算符优先级更高，停止弹出，该自己入栈了
                    break;
                }
            }
            // 对于三种情况，最后都要将当前运算符入栈
            // 注意，栈为空或栈顶为左括号的情况会直接到达这里
            op_stack.push(token);
        } else {
            // 遇到非法字符，表达式非法
            is_valid = false;
            return {};
        }
    }

    // 扫描完毕，弹出栈中所有运算符
    while (!op_stack.empty()) {
        // 不应当再出现左括号，出现则表达式非法
        if (op_stack.top() == "(") {
            is_valid = false;
            return {};
        }
        postfix_tokens.push_back(op_stack.top());
        op_stack.pop();
    }

    return postfix_tokens;
}

long long calculatePostfix(const vector<string>& postfix_tokens, bool& is_valid) {
    stack<long long> calc_stack;

    for (const string& token : postfix_tokens) {
        if (isOperator(token)) {
            if (calc_stack.size() < 2) {
                is_valid = false;
                return -1;
            }

            long long right = calc_stack.top();
            calc_stack.pop();
            long long left = calc_stack.top();
            calc_stack.pop();

            long long res = 0;
            char op = token[0];
            switch (op) {
            case '+':
                res = left + right;
                break;
            case '-':
                res = left - right;
                break;
            case '*':
                res = left * right;
                break;
            case '/':
                if (right == 0) {
                    is_valid = false;
                    return -1;
                }
                res = left / right;
                break;
            case '%':
                if (right == 0) {
                    is_valid = false;
                    return -1;
                }
                res = left % right;
                break;
            case '^':
                res = fastPow(left, right);
                break;
            default:
                break;
            }
            calc_stack.push(res);
        } else {
            calc_stack.push(stoll(token));
        }
    }

    if (calc_stack.size() != 1) {
        is_valid = false;
        return -1;
    }

    return calc_stack.top();
}

int main() {
    string post_expr;
    getline(cin, post_expr);

    // 将中缀表达式正确解析为按空格分割的token列表
    // 用vector<string>来存储token列表，这样方便后续处理
    // 正常情况下，中缀表达式之间无空格；后缀表达式是需要空格的
    vector<string> infix_tokens = tokenizeInfixExpression(post_expr);

    // 将中缀表达式token列表转为后缀表达式token列表
    // 传入引用参数is_valid以判断表达式是否合法
    bool is_valid = true;
    vector<string> postfix_tokens = infixToPostfix(infix_tokens, is_valid);
    if (!is_valid) {
        cout << "Error: Invalid expression." << endl;
        return 0;
    }

    // 计算后缀表达式的值
    long long res = calculatePostfix(postfix_tokens, is_valid);
    if (is_valid) {
        cout << res << endl;
    } else {
        cout << "Error: Invalid expression." << endl;
    }

    return 0;
}
