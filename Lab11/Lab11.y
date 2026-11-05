%{
/* Name: Aryan */
/* Roll No: 2401CS48 */

#include <stdio.h>
#include <ctype.h>

int yylex(void);
void yyerror(const char *s);

int line_n = 1;
%}

%token id num

%left '+' '-'
%left '*' '/'

%%

prog
    : stmts
    ;

stmts
    : stmt
    | stmts stmt
    ;

stmt
    : id '=' expr ';'
    | expr ';'
    ;

expr
    : expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | '(' expr ')'
    | id
    | num
    ;

%%

int yylex(void) {
    int c;

    while ((c = getchar()) != EOF) {
        if (c == '\n')
            line_n++;
        else if (c != ' ' && c != '\t' && c != '\r')
            break;
    }
    if (c == EOF)
        return 0;

    if (isalpha(c) || c == '_') {
        while (isalnum(c) || c == '_')
            c = getchar();
        ungetc(c, stdin);
        return id;
    }

    if (isdigit(c)) {
        while (isdigit(c))
            c = getchar();
        ungetc(c, stdin);
        return num;
    }

    return c;
}

void yyerror(const char *s) {
    printf("Syntax error at line %d\n", line_n);
}

int main() {
    if (yyparse() == 0)
        printf("Input is syntactically valid.\n");
    return 0;
}