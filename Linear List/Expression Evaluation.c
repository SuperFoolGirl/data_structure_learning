// 表达式求值

// 后缀表达式
// 后缀表达式需要用栈来实现
// 简单讲一下后缀表达式的求值过程：
// 1. 从左到右扫描后缀表达式
// 2. 如果遇到操作数，则将其压入栈中
// 3. 如果遇到操作符，则从栈中弹出两个操作数，进行相应的运算，然后将结果压入栈中
// 4. 重复步骤2和3，直到表达式扫描完毕

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 100

char expr[] = "82/2+56*-";    // 后缀表达式
char ieexpr[] = "8/2+5*6";    // 中缀表达式

typedef int ElementType;

typedef struct Stack {
    ElementType *data;
    int top;
} Stack;

typedef enum {
    // 扫描字符串时可能会遇到的字符类型
    LEFT_PARE,
    RIGHT_PARE,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    EOS,    // End of String
    NUM     // 操作数
} contentType;

Stack *initStack() {
    Stack *s = (Stack *)malloc(sizeof(Stack));
    s->data = (ElementType *)malloc(MAXSIZE * sizeof(ElementType));
    s->top = -1;
    return s;
}

bool isEmpty(Stack *s) {
    return s->top == -1;
}

bool push(Stack *s, ElementType value) {
    if (s->top >= MAXSIZE - 1) {
        printf("Stack overflow\n");
        return false;    // 栈满，无法入栈
    }
    s->data[++s->top] = value;
    return true;
}

ElementType pop(Stack *s) {
    if (isEmpty(s)) {
        printf("Stack underflow\n");
        return -1;    // 栈空，无法出栈
    }
    return s->data[s->top--];
}

ElementType front(Stack *s) {
    if (isEmpty(s)) {
        printf("Stack is empty\n");
        return -1;    // 栈空，无法获取栈顶元素
    }
    return s->data[s->top];
}

void clearStack(Stack *s) {
    free(s->data);
    free(s);
}

// 识别当前字符的类型
contentType getToken(char *symbol, int *index) {
    *symbol = expr[*index];
    *index += 1;
    switch (*symbol) {
    case '(':
        return LEFT_PARE;
    case ')':
        return RIGHT_PARE;
    case '+':
        return ADD;
    case '-':
        return SUB;
    case '*':
        return MUL;
    case '/':
        return DIV;
    case '%':
        return MOD;
    case '\0':
        return EOS;    // 字符串结束
    default:
        return NUM;    // 其他字符视为数字
    }
}

// 逆波兰表示法，即后缀表达式(Postfix Expression)的求值
int RPN(Stack *s) {
    char symbol;
    int op1, op2;
    ElementType result;
    int index = 0;
    contentType token = getToken(&symbol, &index);

    while (token != EOS) {
        if (token == NUM) {
            push(s, symbol - '0');
        } else {
            if (isEmpty(s)) {
                printf("Error: Stack is empty\n");
                return -1;    // 栈空，无法进行运算
            }
            op2 = pop(s);     // 弹出第二个操作数
            if (isEmpty(s)) {
                printf("Error: Stack is empty\n");
                return -1;    // 栈空，无法进行运算
            }
            op1 = pop(s);     // 弹出第一个操作数

            switch (token) {
            case ADD:
                result = op1 + op2;
                break;
            case SUB:
                result = op1 - op2;
                break;
            case MUL:
                result = op1 * op2;
                break;
            case DIV:
                if (op2 == 0) {
                    printf("Error: Division by zero\n");
                    return -1;    // 除以零错误
                }
                result = op1 / op2;
                break;
            case MOD:
                if (op2 == 0) {
                    printf("Error: Division by zero\n");
                    return -1;    // 除以零错误
                }
                result = op1 % op2;
                break;
            default:
                printf("Error: Unknown operator\n");
                return -1;      // 未知操作符
            }
            push(s, result);    // 将结果压入栈中
        }
        token = getToken(&symbol, &index);    // 获取下一个符号
    }
    // 计算结果
    return pop(s);
}

// 中缀表达式转为后缀表达式
int InfixExpressionToPostfixExpression() {
    // 先把中缀表达式转换为后缀表达式，然后再调用RPN进行求值
    // 转换逻辑：操作数直接输出，操作符入栈
    // 1. 如果优先级高于栈顶操作符，则直接入栈（第一个操作符必然入栈）
    // 2. 如果优先级低于或等于栈顶操作符，则将栈顶出栈并输出，同时将当前操作符入栈
    // 3. 左括号在栈外时，优先级最高；在栈内时，优先级最低
    // 4. 如果遇到右括号，持续出栈并输出，直到栈顶为左括号，同时左括号也要出栈
    // 5. 遇到字符串结尾时，持续出栈并输出，直到栈空
}

int main() {
    Stack *s = initStack();
    // int res = RPN(s);
    // if (res != -1) {
    //     printf("The result of the expression is: %d\n", res);
    // }
    printf("%s", ieexpr);
    

    clearStack(s);
    return 0;
}
