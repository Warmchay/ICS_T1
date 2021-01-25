//
// Created by zoriswang on 2021/1/25.
//
#include "iostream"
#include "cstdio"
using namespace std;

float sum_elements(float a[], unsigned len);
int main(){
    float a[] = {0.0, 1.0, 2.0, 3.0};
    cout << sum_elements(a, 0);

    return 0;
}

float sum_elements(float a[], unsigned len){
    int i;
    float result = 0;
    for(int i=0; i<=len-1; ++i){
        result+=a[i];
    }
    return result;
}
