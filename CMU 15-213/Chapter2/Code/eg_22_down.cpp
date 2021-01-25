//
// Created by zoriswang on 2021/1/25.
//
#include "iostream"
#include "cstdio"
using namespace std;
typedef unsigned char* pointer_char;
void show_bytes(pointer_char x, int len);
int main(){
    short sx = -12345;
    /*before defining 'su' firstly to transfer 'sx' into int kind*/
    unsigned su = sx;
    printf("su = %u\n", su);
    show_bytes((pointer_char)&su, sizeof(unsigned));

    return 0;
}

void show_bytes(pointer_char x, int len){
    for(int i=0; i<len; ++i){
        printf("%.2x ", x[i]);
    }
}