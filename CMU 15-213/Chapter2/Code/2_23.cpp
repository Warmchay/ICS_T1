//
// Created by zoriswang on 2021/1/25.
//
#include "iostream"
#include "cstdio"
using namespace std;

int fun1(unsigned word);
int fun2(unsigned word);
int main(){
    while(1){
        int x;
        scanf("%x", &x);
        printf("fun1: %.8x\n", fun1(x));
        printf("fun2: %.8x\n", fun2(x));
    }

    return 0;
}

int fun1(unsigned word){
    return (int)((word << 24) >> 24);
}

int fun2(unsigned word){
    return ((int)(word << 24) >> 24);
}