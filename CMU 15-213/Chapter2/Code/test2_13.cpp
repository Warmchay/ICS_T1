//
// Created by zoriswang on 2021/1/24.
//
#include <iostream>
#include "cstdio"
using namespace std;
int bis(int x, int m);
int bic(int x, int m);
int bool_or(int x, int y);
int bool_xor(int x, int y);

int main()
{
    int x, y;
    cin >> x >> y;
    cout << bool_or(x, y) << endl;
    cout << bool_xor(x, y) << endl;
    return 0;
}

int bis(int x, int m){
    return x|m;
}

int bic(int x, int m){
    return x&(~m);
}
/*calculation of '|'*/
int bool_or(int x, int y){
    return bis(x, y);
}

/*calculation of '^'*/
int bool_xor(int x, int y){
    return bis(bic(x,y),bic(y,x));
}