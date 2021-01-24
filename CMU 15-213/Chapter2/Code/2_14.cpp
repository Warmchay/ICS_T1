//
// Created by zoriswang on 2021/1/24.
//
#include "iostream"
#include "cstdio"
using namespace std;

int main(){
    int x = 0x66, y = 0x39;
    printf("x&y = %x\n", x&y);
    printf("x|y = %x\n", x|y);
    printf("~x|~y = %x\n", ~x|~y);
    printf("x&!y = %x\n", x&!y);
    printf("x&&y = %x\n", x&&y);
    printf("x||y = %x\n", x||y);
    printf("!x||!y = %x\n", !x||!y);
    printf("x&&~y=%x\n", x&&~y);

    return 0;
}

