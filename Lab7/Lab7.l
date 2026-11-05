%{
/*
* Name : Aryan
* Roll no : 2401CS48
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Symbol {
    char name[100];
    int first_line;
    int count;
};

struct Symbol symbol_table[1000];
int symbol_count = 0;

void add_symbol(const char *name, int line) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            symbol_table[i].count++;
            return;
        }
    }
    strcpy(symbol_table[symbol_count].name, name);
    symbol_table[symbol_count].first_line = line;
    symbol_table[symbol_count].count = 1;
    symbol_count++;
}

void print_symbol_table() {
    printf("\nSymbol Table:\n");
    for (int i = 0; i < symbol_count; i++) {
        printf("%s\tfirst line %d\toccurrences %d\n", 
               symbol_table[i].name, 
               symbol_table[i].first_line, 
               symbol_table[i].count);
    }
}
%}

%option noyywrap
%option yylineno

%%

"int"|"float"|"char"|"double"|"if"|"else"|"while"|"for"|"return"|"void" {
    printf("%d: %s -> KEYWORD\n", yylineno, yytext);
}

[a-zA-Z_][a-zA-Z0-9_]* {
    printf("%d: %s -> IDENTIFIER\n", yylineno, yytext);
    add_symbol(yytext, yylineno);
}

[0-9]+(\.[0-9]+)? {
    printf("%d: %s -> NUMBER\n", yylineno, yytext);
}

[0-9]+(\.[0-9]+)+[a-zA-Z0-9.]*|[0-9]+[a-zA-Z_][a-zA-Z0-9_]* {
    printf("Lexical Error at line %d: malformed number '%s'\n", yylineno, yytext);
}

\"[^\n"]*\" {
    printf("%d: %s -> STRING\n", yylineno, yytext);
}

\"[^\n"]* {
    printf("Lexical Error at line %d: unterminated string '%s'\n", yylineno, yytext);
}

"=="|"<="|">="|"!=" {
    printf("%d: %s -> OPERATOR\n", yylineno, yytext);
}

"+"|"-"|"*"|"/"|"="|"<"|">" {
    printf("%d: %s -> OPERATOR\n", yylineno, yytext);
}

";"|","|"("|")"|"{"|"}" {
    printf("%d: %s -> PUNCTUATION\n", yylineno, yytext);
}

[ \t\r\n]+ ;

. {
    printf("Lexical Error at line %d: unexpected character '%s'\n", yylineno, yytext);
}

<<EOF>> {
    print_symbol_table();
    return 0;
}

%%

int main() {
    yylex();
    return 0;
}