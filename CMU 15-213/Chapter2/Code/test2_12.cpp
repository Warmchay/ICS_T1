//
// Created by zoriswang on 2021/1/24.
//
#include "iostream"
#include "cstdio"
using namespace std;

/*typedef unsigned char* pointer_char;

void show_bytes_1(pointer_char x, int len);
void show_bytes_2(pointer_char x, int len);
void show_bytes_3(pointer_char x, int len);
*/
int main(){
    int num = 0x87654321;
    /*A*/
    printf("%.8x\n", num&(0xFF));
    /*B*/
    printf("%.8x\n", num^(0x11111100));
    /*C*/
    printf("%.8x\n", num|(0x000000FF));
    //printf("%x", num>>1);
    return 0;
}
