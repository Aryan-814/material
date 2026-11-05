// Aryan
// 2401CS48
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

char* expr;

void skip_whitespace();
void error();
long long evalsum();
long long evalterm();
long long evalfactor();

void error(){
    printf("Invalid arithmetic expression. \n");
    exit(1);
}

void skip_whitespace(){
    while(*expr == ' '){
        expr++;
    }
}

long long evalfactor(){
    skip_whitespace();
    if(*expr == '('){
        expr++;
        long long result = evalsum();
        skip_whitespace();
        if(*expr == ')'){
            expr++;
            return result;
        }else{
            error();
        }
    }else if(*expr >= '0' && *expr <= '9'){
        long long num = 0;
        while(*expr >= '0' && *expr <= '9'){
            num = num*10 + (*expr - '0');
            expr++;
        }
        skip_whitespace();
        return num;
    }else{
        error();
    }
    return 0;
}

long long evalterm(){
    long long result = evalfactor();
    skip_whitespace();
    while(*expr == '*'){
        expr++;
        result *= evalfactor();
        skip_whitespace();  
    }
    return result;
}

long long evalsum(){
    long long result = evalterm();
    skip_whitespace();
    while(*expr == '+'){
        expr++;
        result += evalterm();
        skip_whitespace();
    }
    return result;
}

int main(){
    char str[10000];
    if(fgets(str, sizeof(str), stdin) == NULL){
        return 0;
    }
    expr = str;
    long long final_val = evalsum();
    skip_whitespace();

    if(*expr != '\0' && *expr != '\n'){
        error();
    }

    printf("%lld \n", final_val);
    return 0;
}
