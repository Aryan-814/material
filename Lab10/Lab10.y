%{
/* Name: Aryan */
/* Roll No: 2401CS48 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int yylex(void);
void yyerror(const char *s);

int line_n = 1;
int temp_n = 0;
int label_n = 0;
int err = 0;

char code[500][80];
int code_n = 0;

void new_temp(char *s) {
    temp_n++;
    sprintf(s, "t%d", temp_n);
}
%}

%union {
    int n;
    char name[32];
}

%token <name> id num
%token if_t else_t le ge eq ne

%type <name> expr cond rel
%type <n> if_head

%left '+' '-'
%left '*' '/'
%right uminus

%nonassoc lower_than_else
%nonassoc else_t

%%

prog
    : stmts
    ;

stmts
    : stmts stmt
    |
    ;

stmt
    : id '=' expr ';' {
        sprintf(code[code_n++], "%s = %s", $1, $3);
    }
    | '{' stmts '}'
    | if_head stmt %prec lower_than_else {
        sprintf(code[code_n++], "L%d:", $1 + 1);
    }
    | if_head stmt else_t {
        int l3;
        label_n++;
        l3 = label_n;
        sprintf(code[code_n++], "goto L%d", l3);
        sprintf(code[code_n++], "L%d:", $1 + 1);
        $<n>$ = l3;
    } stmt {
        sprintf(code[code_n++], "L%d:", $<n>4);
    }
    ;

if_head
    : if_t '(' cond ')' {
        int l1 = label_n + 1;
        label_n = label_n + 2;
        sprintf(code[code_n++], "if %s goto L%d", $3, l1);
        sprintf(code[code_n++], "goto L%d", l1 + 1);
        sprintf(code[code_n++], "L%d:", l1);
        $$ = l1;
    }
    ;

cond
    : expr rel expr {
        sprintf($$, "%s %s %s", $1, $2, $3);
    }
    ;

rel
    : '<' { strcpy($$, "<"); }
    | '>' { strcpy($$, ">"); }
    | le  { strcpy($$, "<="); }
    | ge  { strcpy($$, ">="); }
    | eq  { strcpy($$, "=="); }
    | ne  { strcpy($$, "!="); }
    ;

expr
    : expr '+' expr {
        new_temp($$);
        sprintf(code[code_n++], "%s = %s + %s", $$, $1, $3);
    }
    | expr '-' expr {
        new_temp($$);
        sprintf(code[code_n++], "%s = %s - %s", $$, $1, $3);
    }
    | expr '*' expr {
        new_temp($$);
        sprintf(code[code_n++], "%s = %s * %s", $$, $1, $3);
    }
    | expr '/' expr {
        new_temp($$);
        sprintf(code[code_n++], "%s = %s / %s", $$, $1, $3);
    }
    | '-' expr %prec uminus {
        new_temp($$);
        sprintf(code[code_n++], "%s = -%s", $$, $2);
    }
    | '(' expr ')' {
        strcpy($$, $2);
    }
    | id {
        strcpy($$, $1);
    }
    | num {
        strcpy($$, $1);
    }
    ;

%%

int yylex(void) {
    int c, i;

    while ((c = getchar()) != EOF) {
        if (c == '\n')
            line_n++;
        else if (c != ' ' && c != '\t' && c != '\r')
            break;
    }
    if (c == EOF)
        return 0;

    if (isalpha(c) || c == '_') {
        i = 0;
        while (isalnum(c) || c == '_') {
            if (i < 30) {
                yylval.name[i] = c;
                i++;
            }
            c = getchar();
        }
        yylval.name[i] = '\0';
        ungetc(c, stdin);
        if (strcmp(yylval.name, "if") == 0)
            return if_t;
        if (strcmp(yylval.name, "else") == 0)
            return else_t;
        return id;
    }

    if (isdigit(c)) {
        i = 0;
        while (isdigit(c)) {
            if (i < 30) {
                yylval.name[i] = c;
                i++;
            }
            c = getchar();
        }
        yylval.name[i] = '\0';
        ungetc(c, stdin);
        return num;
    }

    if (c == '<' || c == '>' || c == '=' || c == '!') {
        int next = getchar();
        if (next == '=') {
            if (c == '<') return le;
            if (c == '>') return ge;
            if (c == '=') return eq;
            return ne;
        }
        ungetc(next, stdin);
    }

    return c;
}

void yyerror(const char *s) {
    printf("Syntax error at line %d\n", line_n);
    err = 1;
}

int main() {
    int i;

    if (yyparse() == 0 && err == 0) {
        for (i = 0; i < code_n; i++)
            printf("%s\n", code[i]);
    }
    return 0;
}