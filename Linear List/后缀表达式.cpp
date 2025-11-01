// 这里给出的后缀表达式，需要是以空格分隔的形式输入，例如：
// 输入: "3 4 + 2 * 7 /"
// 如果数字之间没有空格，会被解析为一个多位数字

#include <iostream>
#include <sstream>
#include <stack>

using namespace std;

bool isOperator(const string& token) {
    return token == "+" || token == "-" || token == "*" || token == "/" || token == "^" || token == "%";
}

int fastPow(int base, int exp) {
    int res = 1;
    while (exp) {
        if (exp & 1) {
            res *= base;
        }
        base *= base;
        exp >>= 1;
    }
    return res;
}

// 后缀表达式可以无脑计算，不含括号、优先级等问题
// 1. 从左到右扫描后缀表达式
// 2. 如果遇到数字，则将其压入栈中
// 3. 如果遇到运算符，则从栈中弹出两个操作数，先弹出的作为右操作数，后弹出的作为左操作数；运算后入栈
// 4. 最终剩下的唯一元素就是计算结果
int calculate(string& expression, bool& is_valid) {
    stack<int> s;
    // 使用stringstream方便按空格分割，正确解析多位数字
    stringstream ss(expression);
    string token;

    while (ss >> token) {
        if (isOperator(token)) {
            if (s.size() < 2) {
                is_valid = false;
                return -1;
            }
            int right = s.top();
            s.pop();
            int left = s.top();
            s.pop();

            int res = 0;
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
                    is_valid = false;    // 除以0错误
                    return -1;
                }
                res = left / right;
                break;
            case '^':
                res = fastPow(left, right);
                break;
            case '%':
                if (right == 0) {
                    is_valid = false;    // 除以0错误
                    return -1;
                }
                res = left % right;
                break;
            default:
                break;
            }
            s.push(res);
        } else {
            // 处理数字，支持多位数
            int num = stoi(token);
            s.push(num);
        }
    }

    if (s.size() != 1) {
        is_valid = false;    // 表达式不合法
        return -1;
    }

    return s.top();
}

int main() {
    string expression;
    getline(cin, expression);    // 使用getline以支持包含空格的输入

    // 该函数支持判断表达式是否合法，传入引用参数is_valid
    bool is_valid = true;
    int res = calculate(expression, is_valid);

    if (is_valid) {
        cout << res << endl;
    } else {
        cout << "Invalid Expression" << endl;
    }

    return 0;
}
