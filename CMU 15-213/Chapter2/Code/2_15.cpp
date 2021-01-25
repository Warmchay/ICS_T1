//
// Created by zoriswang on 2021/1/25.
//
#include "iostream"
#include "cstdio"

using namespace std;
int judgeEqual(int x, int y);
int main(){
    int x, y;
    cin >> x >> y;
    cout << ((judgeEqual(x, y)==0) ? 1:0) << endl;
    cout << !(x^y);
    return 0;
}

int judgeEqual(int x, int y){
    return x&(~y);
}