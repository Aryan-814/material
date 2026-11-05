/* Name: Aryan */
/* Roll No: 2401CS48 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define max 100

struct token {
    int is_num;
    int num;
    char op;
};

struct token post[max];
int post_n = 0;

int prec(char c) {
    if (c == '*' || c == '/')
        return 2;
    if (c == '+' || c == '-')
        return 1;
    return 0;
}

void add_num(int n) {
    post[post_n].is_num = 1;
    post[post_n].num = n;
    post_n++;
}

void add_op(char c) {
    post[post_n].is_num = 0;
    post[post_n].op = c;
    post_n++;
}

/* Phase 1: infix to postfix (shunting-yard) */
int toPostfix(char *s) {
    char stack[max];
    int top = -1;
    int i = 0;

    while (s[i] != '\0') {
        char c = s[i];

        if (c == ' ') {
            i++;
        }
        else if (isdigit(c)) {
            int n = 0;
            while (isdigit(s[i])) {
                n = n * 10 + (s[i] - '0');
                i++;
            }
            add_num(n);
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/') {
            while (top >= 0 && stack[top] != '(' && prec(stack[top]) >= prec(c)) {
                add_op(stack[top]);
                top--;
            }
            top++;
            stack[top] = c;
            i++;
        }
        else if (c == '(') {
            top++;
            stack[top] = c;
            i++;
        }
        else if (c == ')') {
            while (top >= 0 && stack[top] != '(') {
                add_op(stack[top]);
                top--;
            }
            if (top < 0) {
                printf("Error: mismatched parentheses\n");
                return 0;
            }
            top--;
            i++;
        }
        else {
            printf("Error: invalid character '%c'\n", c);
            return 0;
        }
    }

    while (top >= 0) {
        if (stack[top] == '(') {
            printf("Error: mismatched parentheses\n");
            return 0;
        }
        add_op(stack[top]);
        top--;
    }
    return 1;
}

/* Phase 2: evaluate postfix with a number stack */
int evalPostfix(int *result) {
    int stack[max];
    int top = -1;
    int i;

    for (i = 0; i < post_n; i++) {
        if (post[i].is_num) {
            top++;
            stack[top] = post[i].num;
        }
        else {
            if (top < 1) {
                printf("Error: invalid expression\n");
                return 0;
            }
            int b = stack[top];
            top--;
            int a = stack[top];
            int r = 0;

            if (post[i].op == '+')
                r = a + b;
            else if (post[i].op == '-')
                r = a - b;
            else if (post[i].op == '*')
                r = a * b;
            else {
                if (b == 0) {
                    printf("Error: division by zero\n");
                    return 0;
                }
                r = a / b;
            }
            stack[top] = r;
        }
    }

    if (top != 0) {
        printf("Error: invalid expression\n");
        return 0;
    }
    *result = stack[0];
    return 1;
}

int main() {
    char line[max];
    int result;
    int i;

    printf("Enter expression: ");
    fgets(line, max, stdin);
    line[strcspn(line, "\n")] = '\0';

    if (!toPostfix(line))
        return 1;

    printf("Postfix : ");
    for (i = 0; i < post_n; i++) {
        if (post[i].is_num)
            printf("%d ", post[i].num);
        else
            printf("%c ", post[i].op);
    }
    printf("\n");

    if (!evalPostfix(&result))
        return 1;

    printf("Result : %d\n", result);
    return 0;
}