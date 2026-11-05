%{
/* Name: Aryan */
/* Roll No: 2401CS48 */

#include <stdio.h>
%}

%option noyywrap

%x comment

digit   [0-9]
letter  [a-zA-Z_]

%%

"//".*              { }

"/*"                { BEGIN(comment); }
<comment>"*/"       { BEGIN(INITIAL); }
<comment>\n         { }
<comment>.          { }
<comment><<EOF>>    { printf("Error: unterminated comment\n"); return 0; }

"int"|"float"|"char"|"double"|"if"|"else"|"while"|"for"|"return"|"void" {
    printf("%s -> KEYWORD\n", yytext);
}

{letter}({letter}|{digit})* {
    printf("%s -> IDENTIFIER\n", yytext);
}

{digit}+(\.{digit}+)? {
    printf("%s -> NUMBER\n", yytext);
}

"=="|"<="|">="|"!=" {
    printf("%s -> OPERATOR\n", yytext);
}

[+\-*/=<>] {
    printf("%s -> OPERATOR\n", yytext);
}

[;,(){}] {
    printf("%s -> PUNCTUATION\n", yytext);
}

[ \t\n]+            { }

.                   { printf("Error: invalid character '%s'\n", yytext); }

%%

int main() {
    yylex();
    return 0;
}