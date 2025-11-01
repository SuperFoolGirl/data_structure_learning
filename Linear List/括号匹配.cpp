// 当有多种括号嵌套时，嵌套的顺序应为{ → [ → ( → <

#include <iostream>
#include <stack>
#include <unordered_map>

using namespace std;

bool check(string& str, stack<char>& s, unordered_map<char, int> brackets) {
    for (auto c : str) {
        // 不是括号就跳过
        if (!brackets.count(c)) {
            continue;
        }
        // 是括号，进行入栈判断
        if (s.empty()) {
            // 在空栈的情况下入栈，如果是右括号，直接错
            if (brackets[c] % 2) {
                return false;
            }
            s.push(c);
        } else {
            // 对于左括号，只要满足层级顺序，就可以直接入
            if (!(brackets[c] % 2)) {
                if (brackets[c] <= brackets[s.top()]) {
                    return false;
                }
                s.push(c);
            } else {
                // 对右括号，必须满足与栈顶左括号匹配，然后出栈左括号。栈中不留右括号
                if (brackets[c] != brackets[s.top()] + 1) {
                    return false;
                }
                s.pop();
            }
        }
    }
    // 最后，如果栈非空，则为错误
    if (!s.empty()) {
        return false;
    }
    return true;
}

int main() {
    unordered_map<char, int> brackets;
    brackets.insert({'{', 0});
    brackets.insert({'}', 1});
    brackets.insert({'[', 2});
    brackets.insert({']', 3});
    brackets.insert({'(', 4});
    brackets.insert({')', 5});
    brackets.insert({'<', 6});
    brackets.insert({'>', 7});

    stack<char> s;
    int n;
    cin >> n;
    string str;
    while (n--) {
        cin >> str;
        // 清栈
        while (!s.empty()) {
            s.pop();
        }

        if (check(str, s, brackets)) {
            cout << "Match" << endl;
        } else {
            cout << "Fail" << endl;
        }
    }

    return 0;
}
