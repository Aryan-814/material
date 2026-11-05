/* Name: Aryan */
/* Roll No: 2401CS48 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#define n_tok 20
#define max 10000

/* token names and their regular expressions (same order) */
char *names[n_tok] = {
    "IDENTIFIER", "REAL", "INTEGER",
    "PLUS", "MINUS", "MULTIPLY", "DIVIDE",
    "LESS_EQ", "GREATER_EQ", "EQUAL", "NOT_EQUAL", "LESS", "GREATER",
    "ASSIGN",
    "LPAREN", "RPAREN", "LBRACE", "RBRACE", "SEMICOLON", "COMMA"
};

char *pats[n_tok] = {
    "^[a-zA-Z][a-zA-Z0-9_]*",
    "^[0-9]+[.][0-9]+",
    "^[0-9]+",
    "^[+]", "^[-]", "^[*]", "^[/]",
    "^<=", "^>=", "^==", "^!=", "^<", "^>",
    "^=",
    "^[(]", "^[)]", "^[{]", "^[}]", "^[;]", "^[,]"
};

regex_t re[n_tok];

char *words[6] = {"if", "else", "while", "int", "string", "return"};

int is_keyword(char *s, int len) {
    int i;
    for (i = 0; i < 6; i++) {
        if (strlen(words[i]) == len && strncmp(s, words[i], len) == 0)
            return 1;
    }
    return 0;
}

int main() {
    char text[max];
    int len, i, pos = 0;
    int best, best_i;
    int ok = 1;

    len = fread(text, 1, max - 1, stdin);
    text[len] = '\0';

    for (i = 0; i < n_tok; i++)
        regcomp(&re[i], pats[i], REG_EXTENDED);

    while (text[pos] != '\0') {
        if (isspace((unsigned char)text[pos])) {
            pos++;
            continue;
        }

        best = 0;
        best_i = -1;

        /* try every pattern, keep the longest match */
        for (i = 0; i < n_tok; i++) {
            regmatch_t m;
            if (regexec(&re[i], text + pos, 1, &m, 0) == 0) {
                if (m.rm_eo > best) {
                    best = m.rm_eo;
                    best_i = i;
                }
            }
        }

        if (best_i == -1) {
            printf("LEXICAL ERROR: unexpected character '%c'\n", text[pos]);
            ok = 0;
            break;
        }

        if (best_i == 0 && is_keyword(text + pos, best))
            printf("KEYWORD %.*s\n", best, text + pos);
        else
            printf("%s %.*s\n", names[best_i], best, text + pos);

        pos = pos + best;
    }

    if (ok)
        printf("ACCEPT\n");
    else
        printf("REJECT\n");

    return 0;
}