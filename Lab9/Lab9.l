%{
/* Name: Aryan */
/* Roll No: 2401CS48 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int line = 1;
int open = 0;
int start_line = 0;

int dec_n = 0;
int oct_n = 0;
int hex_n = 0;
int bin_n = 0;
int flt_n = 0;
int errors = 0;
long long sum = 0;
long long largest = 0;

int int_lit(int skip, int base, char *cat) {
    long long v;
    errno = 0;
    v = strtoll(yytext + skip, NULL, base);
    if (errno == ERANGE) {
        printf("Lexical Error at line %d: integer literal '%s' out of range\n", line, yytext);
        errors++;
        return 0;
    }
    printf("%d: %s -> %s (decimal value %lld)\n", line, yytext, cat, v);
    sum = sum + v;
    if (v > largest)
        largest = v;
    return 1;
}

void flt_lit() {
    double v;
    errno = 0;
    v = strtod(yytext, NULL);
    if (errno == ERANGE) {
        printf("Lexical Error at line %d: float literal '%s' out of range\n", line, yytext);
        errors++;
        return;
    }
    printf("%d: %s -> FLOAT (value %g)\n", line, yytext, v);
    flt_n++;
}
%}

DIGIT   [0-9]
OCT     [0-7]
HEX     [0-9a-fA-F]
EXP     [eE][+-]?{DIGIT}+

%x COMMENT

%%

"//".*          { }

"/*"            { BEGIN COMMENT; open = 1; start_line = line; }
<COMMENT>"*/"   { BEGIN 0; open = 0; }
<COMMENT>\n     { line++; }
<COMMENT>.      { }

\"([^"\\\n]|\\.)*\"  { }

\"([^"\\\n]|\\.)*    {
    printf("Lexical Error at line %d: unterminated string literal\n", line);
    errors++;
}

'([^'\\\n]|\\.)*'    { }

[A-Za-z_][A-Za-z0-9_]*   { }

0[xX]{HEX}+     { if (int_lit(2, 16, "HEX_INT")) hex_n++; }
0[bB][01]+      { if (int_lit(2, 2, "BINARY_INT")) bin_n++; }
0{OCT}+         { if (int_lit(0, 8, "OCTAL_INT")) oct_n++; }
0|[1-9]{DIGIT}* { if (int_lit(0, 10, "DECIMAL_INT")) dec_n++; }

{DIGIT}+"."{DIGIT}+({EXP})?   { flt_lit(); }
{DIGIT}+{EXP}                 { flt_lit(); }

{DIGIT}({DIGIT}|[A-Za-z_.])*  {
    printf("Lexical Error at line %d: malformed numeric literal '%s'\n", line, yytext);
    errors++;
}

\n              { line++; }

.               { }

%%

int yywrap() {
    if (open) {
        printf("Lexical Error at line %d: unterminated comment\n", start_line);
        errors++;
    }
    return 1;
}

int main() {
    yylex();

    printf("------------------------------\n");
    printf("Numeric Literal Statistics\n");
    printf("------------------------------\n");
    printf("Decimal integers : %d\n", dec_n);
    printf("Octal integers : %d\n", oct_n);
    printf("Hexadecimal integers : %d\n", hex_n);
    printf("Binary integers : %d\n", bin_n);
    printf("Floating-point : %d\n", flt_n);
    printf("Lexical errors : %d\n", errors);
    printf("Sum of integers : %lld\n", sum);
    printf("Largest integer : %lld\n", largest);
    printf("------------------------------\n");

    if (errors == 0)
        printf("ACCEPT\n");
    else
        printf("REJECT\n");

    return 0;
}