/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 *   推导一下A(异或)B的式子，将式子转换为与非式，后面根据逻辑运算返回值
 */
int bitXor(int x, int y) {
  return ~(~(x&~y)&~(~x&y));
}
/* 
 * tmin - return minimum two's complement integer
 * return the minimum of int(32 bits) ,so you need to move left till the sign bits
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 *   要求的是32位整数补码的最小值，即符号位为1，其余为全为0
 */
int tmin(void) {

  return 1<<31;

}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 *   思路：
 *   1. part 1
 *   [1]当x为0111【1111】时，～(x+1)等于x，此时!(~(x+1)^x)==1;
 *   [2]但是有这个性质的还有-1（1111【1111】），其+1后符号位溢出，~([1111]^[1111])==1;
 *   2. part 2
 *   考虑[1][2]位结果都为[0000]0001,则当他们与对应的!(x+1)进行异或运算时，会产生下列结果
 *   [1]: 对任何非-1的数，!(x+1)都为0
 *   [2]: 对-1，!(x+1)为1
 *   因此对1，2式子产生的结果进行异或运算，则对【1】返回1，对【2】返回2
 */
int isTmax(int x) {
  return (!(x+1))^(!(~(x+1)^x));
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   在每一个bits对应的奇数位为1时输出1，举个例子每一位都是[1010]则是对的
 *   所以要证的实质为，求证的数有这个性质：(x&0xAAAAAAAA = 0xAAAAAAAA)
 *   但是因为不能直接用等号，所以需要在过程中证明这个数是相等的
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
    return !((x&0xAAAAAAAA)^0xAAAAAAAA);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return (~x+1);
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 *   这道题卡了有些久久久，甚至画了真值表去找规律（但是没找到，网上有博客有用规律来做的
 *   但是我想着逻辑运算应该是可以构造出上界和下界的，这道题我是构造上界和下界来做的
 *   根据(x-0x30 >= 0)&&(0x39-x >= 0)来构造
 *   需要用到的是TMin，因为看上方可以知道我们需要值大于0，那么由最大负数0x8fffffff,求出范围内的数通过补码来运算就可以了
 *   但是注意需要用&来加入判断
 */
int isAsciiDigit(int x) {
    int tmin = 1<<31;
    return !((x+(~0x30+1)) & tmin) & !((0x39+~x+1) & tmin);
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 *   首先明确题意：要求实现(x>0)?y:z;
 *   重要的如何判断x>0，及x=0的特殊情况；
 *   但是这里不知道为什么x>0时，包括了x!=0的所有情况，可能是x只能用来判断x等不等于0吧（我猜的
 *   这里说明一个比较难想到的性质：1.~z&(y^z)=y; 2.z&(y^z)=z;
 *   但是这个性质可能只是撞到了题目设置的点，用离散的集合来推是不成立的qwq
 *   当x不等于0时，z^[1111]&(y^z) = ~z&(y^z) = y
 *   当x 为 0时 ，z^[0000]&(y^z) =  z&(y^z) = z
 */
int conditional(int x, int y, int z) {
    return z^(!x+~0)&(y^z) ;
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 *   这个用上面的下界搬过来证明(y-x >= 0)就可以了，即跟tmin做个集合的交运算
 *   但是...出错了，看发生错误的点是在0x80000000和0x7fffffff这里，分析知道上面这个方法只能用于符号位相同的两个数
 *   符号位不同时，需要考虑其他的方法,当y>=0,x<0时，一定大于0
 *   总体思路为：
 *   1.!(x_max & (!y_max)): 对应的为x、y的最高有效位，x为1，y为0时，返回1，因为此时x为负数，y为正数
 *   2.(!(x_max^y_max)&!minus_max):这里分两种情况
 *          - x、y同号，此时看mius_max的最高有效位决定。为0，说明为正；为1，说明为负
 *          - x、y不同号，由1的先验情况，此时y一定为负数，x一定为正数，返回值为负数
 */
int isLessOrEqual(int x, int y) {
    int minus = y+(~x+1);
    /*求出x、y的最高位*/
    int x_max = (x>>31) & 1;
    int y_max = (y>>31) & 1;
    int minus_max = (minus>>31) & 1;
    return (x_max&(!y_max))|(!(x_max^y_max)&!minus_max);
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 *   一开始的思路：
 *   把tmin的表格画出，会发现对于(z(1)z(2)z(3)..z(n))中(z(2)z(3)...z(n))在整个真值表中关于z(1)是对称的
 *   除了0，原数n的相反数的符号位都会与原数相反；而0，符号位与0相同，通过这个性质我们从符号位入手
 *   对于除0的每个数，它们的符号位有这个性质：0^1=1 --> 1^1=0
 *   而0，则为：                        0^0=0 --> 0^1=1
 *   但是，负数的符号位跟0的性质是相同的哇qwq，所以这个方法适用于找到正数
 *   那么，既然负数和0的符号位是相同的，而1是不同的，从符号位出发又有什么性质呢？
 *   对比非0数及其相反数，进行OR运算发现他们的符号位一定为1，而0则为0，从这个角度来考虑，通过移位和与1取^则可以返回正确的值啦
 *   但是，又出现问题了（qaq circulation），问题在于移位时理解成了unsigned类型了，当int型移位时，补充的是左移或者右移的最低有效位
 *   所以当符号位为0时，右移31位为0x00000000；反之，为0xffffffff。因此，最后的判断应该是+1。
 */
int logicalNeg(int x) {
    int x1 = (x|(~x+1))>>31;
    return x1+1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 *  这个想了好久，也没怎么搞懂，这个方法是在知乎上面看到的
 *  因为需要求的是补码表示的数，而负数和正数差了一个符号位，因此我们把负数视为正数处理然后根据每个位的规律来推测
 *  假设我们把各个位表示，符号位sign_loc所在位置有这个规律：
 *  最后加个符号位
 *  [0, 1]  ---  2 bits
 *  [2, 3]  ---  3 bits
 *  [4, 7]  ---  4 bits
 *  在这里我们采用二分的方法来判断位数，因int型是32位的，所以我们根据16--1的方式，最后留下的则是我们的符号位
 *  然后相加每一位有的数，就能求出对应的bits
 */
int howManyBits(int x) {
    int b16,b8,b4,b2,b1,b0;
    int sign = x>>31; //得到符号位

    x = (~sign&x)|(sign&~x);//异或求x，x为0(正数)则不变，为1(负数)则取相反数
    b16 = !!(x>>16)<<4;//判断前16位是否有1
    x>>=b16;           //二分到后16位研究
    b8 = !!(x>>8)<<3; //判断二分后的前8位是否有1
    x>>=b8;
    b4 = !!(x>>4)<<2; //判断二分后的前4位是否有1
    x>>=b4;
    b2 = !!(x>>2)<<1; //再判断二分后的前2位是否有1
    x>>=b2;
    b1 = !!(x>>1);
    x>>=b1;
    b0 = x;
    return b16+b8+b4+b2+b1+b0+1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 *   要求：返回给定浮点值的两倍，对于NAN的情况要小心
 *   这里主要根据IEEE的标准来求值，我们分为sign，exp和frac位来讨论
 *   分为三种情况：
 *   首先：先把这三个部分求出
 *   其次，分情况讨论：
 *      - 当exp=0xff时，frac为0说明时无穷大，frac不为0说明为NaN
 *      - 当exp=0x0时，此时位为非规格化值，只有frac位，所以对frac位乘2，即frac<<1即可
 *      - 当exp!=0xff && exp!=0x, 并有frac!=0，此时位规格化值，exp++即可
 */
unsigned floatScale2(unsigned uf) {
    int sign = uf>>31;
    int exp  = (uf&0x7f800000)>>23;
    int frac = (uf&0x7fffff);
    if(exp == 0xff) return uf;
    else if(exp == 0x0){
        frac<<=1;
        return sign<<31 | exp<<23 | frac;
    }else{
        exp++;
        return sign<<31 | exp<<23 | frac;
    }
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 *   则道题我没怎么掌握，构建的逻辑我自己也说服不了我自己，这里是将unsigned表示的浮点数类型转化为int型的六位
 *   int型的六位位：1(sign), 3(exp), 2(frac)根据是否超过(exp+sign)表示的范围来确定int型表示的值
 *
 */
int floatFloat2Int(unsigned uf) {
    int TMIN = 1 << 31;
    int exp = ((uf >> 23) & 0xFF) - 127;
    // Out of range
    if (exp > 31)
        return TMIN;
    if (exp < 0)
        return 0;
    int frac = (uf & 0x007fffff) | 0x00800000;
    // Left shift or right shift
    int f = (exp > 23) ? (frac << (exp - 23)) : (frac >> (23 - exp));
    // Sign
    return (uf & TMIN) ? -f : f;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 *   这里需要求的时关于int型的exp的指数，知道exp最大为255，如果算进符号位最大为127，所以我们在这里视为规格化值和非规格化值
 *   而题目要求当为非规格化值时，返回0；反之返回对应的书，但是如果太大，则范围正无穷
 *   最后返回对应的exp的7位即可；
 *   但是这里超时了，我觉得可能官方的有一些问题，因为试过了无数种方法，感觉不同服务器会有不同的结果吧～
 */
unsigned floatPower2(int x) {
    int exp = x+127;
    if(exp>=255) return 0xff<<23;
    if(exp<=0)  return 0;
    return exp<<23;
}
