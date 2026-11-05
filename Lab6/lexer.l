%{
/* Name: Aryan */
/* Roll No: 2401CS48 */

#include <stdio.h>

int lines = 0;
int single_c = 0;
int multi_c = 0;
int str_n = 0;
int chr_n = 0;
int pre_n = 0;
int id_n = 0;
int errors = 0;
int pending = 0;

#define YY_USER_ACTION if (yytext[yyleng - 1] == '\n') pending = 0; else pending = 1;
%}

%option noyywrap

%x comment

letter  [a-zA-Z_]
digit   [0-9]

%%

"//".*      { single_c++; }

"/*"        { BEGIN(comment); }
<comment>"*/"    { BEGIN(INITIAL); multi_c++; }
<comment>\n      { lines++; }
<comment>.       { }
<comment><<EOF>> {
    printf("LEXICAL ERROR: unterminated comment\n");
    errors++;
    return 0;
}

\"([^"\\\n]|\\.)*\" {
    printf("STRING_LITERAL %s\n", yytext);
    str_n++;
}

\"([^"\\\n]|\\.)* {
    printf("LEXICAL ERROR: unterminated string literal %s\n", yytext);
    errors++;
}

'([^'\\\n]|\\.)' {
    printf("CHAR_LITERAL %s\n", yytext);
    chr_n++;
}

'([^'\\\n]|\\.)* {
    printf("LEXICAL ERROR: unterminated character literal %s\n", yytext);
    errors++;
}

^[ \t]*"#".* {
    printf("PREPROCESSOR %s\n", yytext);
    pre_n++;
}

"int"|"float"|"char"|"double"|"if"|"else"|"while"|"for"|"return"|"void" { }

{letter}({letter}|{digit})* {
    printf("IDENTIFIER %s\n", yytext);
    id_n++;
}

{digit}+    { }

\n          { lines++; }

.           { }

%%

int main() {
    yylex();

    if (pending)
        lines++;

    printf("------------------------------\n");
    printf("Source Code Statistics\n");
    printf("------------------------------\n");
    printf("Lines             : %d\n", lines);
    printf("Comments          : %d\n", single_c + multi_c);
    printf("  Single-line     : %d\n", single_c);
    printf("  Multi-line      : %d\n", multi_c);
    printf("String literals   : %d\n", str_n);
    printf("Character literals: %d\n", chr_n);
    printf("Preprocessor      : %d\n", pre_n);
    printf("Identifiers       : %d\n", id_n);
    printf("------------------------------\n");

    if (errors == 0)
        printf("ACCEPT\n");
    else
        printf("REJECT\n");

    return 0;
}