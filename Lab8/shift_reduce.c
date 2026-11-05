/* Name: Aryan */
/* Roll No: 2401CS48 */

#include <stdio.h>
#include <string.h>

#define max 100

char stack[max];
int top = -1;
char input[max];
int pos = 0;

int prec(char c) {
    if (c == '*')
        return 2;
    return 1;
}

int is_op(char c) {
    return c == '+' || c == '-' || c == '*';
}

void push(char c) {
    top++;
    stack[top] = c;
}

void shift() {
    char c = input[pos];
    pos++;
    push(c);
    if (c == 'i')
        printf("SHIFT: id\n");
    else
        printf("SHIFT: %c\n", c);
}

int main() {
    char line[max];
    int n = 0;
    int i = 0;
    int bad = 0;
    char la, op;

    printf("Enter expression: ");
    fgets(line, max, stdin);

    /* tokenizer: id becomes 'i', other tokens stay as they are */
    while (line[i] != '\0' && line[i] != '\n') {
        if (line[i] == ' ' || line[i] == '\t') {
            i++;
        }
        else if (line[i] == 'i' && line[i + 1] == 'd') {
            input[n] = 'i';
            n++;
            i = i + 2;
        }
        else if (is_op(line[i]) || line[i] == '(' || line[i] == ')') {
            input[n] = line[i];
            n++;
            i++;
        }
        else {
            printf("LEXICAL ERROR: unexpected character '%c'\n", line[i]);
            bad = 1;
            break;
        }
    }
    input[n] = '$';
    input[n + 1] = '\0';

    if (bad) {
        printf("REJECT\n");
        return 0;
    }

    push('$');

    while (1) {
        la = input[pos];

        /* reduce: id -> E */
        if (stack[top] == 'i') {
            top--;
            push('E');
            printf("REDUCE: id -> E\n");
            continue;
        }

        /* reduce: ( E ) -> E */
        if (top >= 2 && stack[top] == ')' && stack[top - 1] == 'E' && stack[top - 2] == '(') {
            top = top - 3;
            push('E');
            printf("REDUCE: ( E ) -> E\n");
            continue;
        }

        /* handle E op E */
        if (top >= 2 && stack[top] == 'E' && is_op(stack[top - 1]) && stack[top - 2] == 'E') {
            op = stack[top - 1];

            if (is_op(la)) {
                printf("CONFLICT: stack has E %c E, input is '%c'\n", op, la);
                if (prec(op) > prec(la)) {
                    printf("  stack operator has higher precedence -> REDUCE\n");
                }
                else if (prec(op) < prec(la)) {
                    printf("  incoming operator has higher precedence -> SHIFT\n");
                    shift();
                    continue;
                }
                else {
                    printf("  equal precedence, left associative -> REDUCE\n");
                }
            }

            top = top - 3;
            push('E');
            printf("REDUCE: E %c E -> E\n", op);
            continue;
        }

        /* nothing to reduce: end of input or shift */
        if (la == '$') {
            if (top == 1 && stack[1] == 'E')
                printf("ACCEPT\n");
            else
                printf("REJECT\n");
            break;
        }

        shift();
    }

    return 0;
}