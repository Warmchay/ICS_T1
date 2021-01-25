//
// Created by zoriswang on 2021/1/25.
//
#include "iostream"
#include "cstdio"
using namespace std;

void transfer(int x);
int main(){
    int x;
    while(1){
        cout << "input x: ";
        scanf("%x", &x);
        getchar();
        transfer(x);
    }
    return 0;
}

void transfer(int x){
    cout << "choose move bytes:";
    int mv;
    //getchar();
    cin >> mv;
    switch (mv) {
        case 3:
            printf("Arith. %x << 3 = %x\n", x, x << 3);
            break;
        case 2:
            printf("Arith. %x >> 2 = %x\n", x, x >> 2);
            break;
        default:
            cout << "nothing input~" << endl;
            break;
    }
}