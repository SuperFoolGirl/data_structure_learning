// 根据后缀表达式的计算来构建中缀表达式即可
// 每个二元运算符都会配上括号，无法构建最简中缀表达式

#include <iostream>
#include <stack>
#include <vector>

using namespace std;

bool isOperator(const string& token) {
    return token == "+" || token == "-" || token == "*" || token == "/" || token == "^" || token == "%";
}

string postfixToInfix(const vector<string>& postfix_tokens, bool& is_valid) {
    stack<string> expr_stack;

    for (const string& token : postfix_tokens) {
        if (isOperator(token)) {
            // 遇到操作符，弹出两个操作数
            if (expr_stack.size() < 2) {
                is_valid = false;
                return "";
            }
            string right = expr_stack.top();
            expr_stack.pop();
            string left = expr_stack.top();
            expr_stack.pop();

            // 构建新的中缀表达式，注意加括号
            string new_expr = "(" + left + token + right + ")";
            expr_stack.push(new_expr);
        } else {
            // 遇到数字，直接入栈
            expr_stack.push(token);
        }
    }

    if (expr_stack.size() != 1) {
        is_valid = false;
        return "";
    }

    return expr_stack.top();
}

int main() {
    // 输入以空格分隔的后缀表达式
    // 这里直接输入到postfix_tokens中，不再使用getline和stringstream
    vector<string> postfix_tokens;
    string token;
    while (cin >> token) {
        postfix_tokens.push_back(token);
    }

    bool is_valid = true;
    string infix_expr = postfixToInfix(postfix_tokens, is_valid);
    if (is_valid) {
        cout << infix_expr << endl;
    } else {
        cout << "Invalid postfix expression." << endl;
    }

    return 0;
}
