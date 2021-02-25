# CACHE LAB 

## 1 prepartion

下载对应的handout后，在docker中打开，`write-up`中说一切操作都需要在linux系统中完成，因此尽量不要用自己的本地系统**「指的Windows」**来解压缩，有可能会造成文件中数据丢失.

> cache lab目的是来制作自己的缓存系统，具体来说为实现一个缓存模拟器，根据给定的trace文件来输出对应的操作，之后利用缓存机制加速矩阵转置的运算。

1. 进入docker容器「假设你已经将原`.tar`文件移动到了docker容器内对应文件夹下」

```bash
linux> tar xvf cachelab-handout.tar
```

2. 对其中的文件进行编译

```bash
linux> make clean
linux> make
```

---

**Appedix：**

- [CMU C语言规范]([https://www.cs.cmu.edu/~213/activities/cbootcamp/cbootcamp_s19.pdf](https://links.jianshu.com/go?to=https%3A%2F%2Fwww.cs.cmu.edu%2F~213%2Factivities%2Fcbootcamp%2Fcbootcamp_s19.pdf))

- 因为trace文件是由linux中的`valgrind`程序产生的，因此我们还需要在系统中安装该配置：

```bash
sudo yum install valgrind
```

后输入：

```bash
valgrind --log-fd=1 --tool=lackey -v --trace-mem=yes ls -l
```

`valgrind`有以下格式：

```bash
I 0400d7d4,8
M 0421c7f0,4
L 04f6b868,8
S 7ff0005c,8
```

表示了`操作-地址-大小`三元组格式

- `I: ` 表示指令读取
- `L:` 表示数据读取
- `S:` 表示数据存储
- `M:` 表示数据修改

---

<center>valgrind简略介绍</center>

`valgrind` 是一个提供程序调试及性能分析的工具集，最常用的工具为`memcheck`，主要用于检查程序heap上的内存使用情况.[官方网址](http://valgrind.org/)

- **Valgrind** 能用来做什么，其主要用于检查程序以下问题：
  1. 使用未初始化的内存**「use fo unintialised mems」**
  2. 使用已经释放的内存**「reading mems after it has been freed」**
  3. 使用超过malloc分配的内存**「reading off the end of of malloc's blocks」**
  4. 对堆栈的非法访问**「reading inappropriate areas on the stack」**
  5. 申请的空间是否有释放**「memory leaks - where pointers to malloc'd blocks are lost forever」**
  6. malloc/free/new/delete申请和释放内存的匹配
  7. src和dest的重叠**「overlapping src and dst pointers in memcpy() and related functions」**
- **Valgrind** 的使用约束 ⚠️
  1. 只能检查heap上的错误，不能检查出static和stack内存的使用，如数组越界等
  2. 不能指出为什么泄漏，也不能指出在哪内存泄漏
  3. 指出的错误**并非100%正确**，在编译的时候以**warning**的心态对待它们

》〉》**TIPS:**

- 在gcc优化源文件的过程中:`gcc -g -o test test.cpp`，需要加入`-g`参数，便于valgrind读入符号表之类的信息来提供更丰富的错误定位信息。<font color=red>**不推荐加入`-O`等优化参数，因为优化后的代码容易让valgrind解释错误**</font>

》〉》**补充**

---

在**LEAK SUMMARY**中

1. **definitely lost：**表明没有任何指针指向该区域，已经造成了内存泄漏
2. **possibly lost：** 存在指针指向内存中的某个位置，valgrind认为你可能是在做一些其他的高级应用「如将指针放在申请的内存块中间」
3. **still reachable：**仍有指针引用该内存块，只是没有释放，可以通过设置`--show-reachable=yes`来报错

---

**Valgrind常用命令参数**

1. **`-tool=<name>[default=memblock]`**

`--tool`参数指明要使用的为`valgrind`的哪一个工具，默认的为`memcheck`，因此大部分情况下我们省略了该参数

2. **`--leak-check=<no|summary|yes|full>[default:summary]>`**

退出时检查是否有泄漏，`summary`告诉我们有多少次泄漏，`yes`或者`full`告诉我们每次泄漏的信息

3. **`--show-reachable=<yes|no>[default:no]`**

通过设定该参数为`yes`，则将显示`still reachable`类型的内存泄漏信息

## 2 Review C

> 在part A中我们需要编写一个c程序来实现缓存模拟器，在检验的过程中对于c语言的规范程度有要求。
>
> 在这里先熟悉一下各部分的关于编写规范的规定

编写c时，要注意编写出的程序的五个必要条件：

- 注意代码风格，**不要糟糕的代码**
- **小心隐式类型转换**
- 小心**未定义**的行为
- 小心**内存泄漏**
- **宏和指针**的部分注意计算可能出现的错误

举一些🌰来说明以上规范：

---

<center><b>栗子们</b></center>

1. 因为`u`为无符号整型，所以在比较的时候`-1`也会按照无符号整型来比较，此时的比较就变成了`u>maxInt`，无论输入什么都会返回0.

```c
int foo(unsigned int u){
  return (u>-1)?1:0;
}
```

2. a[]中的值都没有初始化，所以main函数的行为是未定义的

```c
int main(){
  int *a = malloc(100*sizeof(int));
  for(int i=0; i<100; ++i){
		a[i] = i/a[i];
  }
  free(a);
  return 0;
}
```

3. strlen的计算是不包括`C PROGRAMMING`最后的`\0`的，这里写入的时候会越界

```c
int main(){
  char w[strlen("C programming")];
  strcpy(w,'C programming');
  printf("%s\n",w);
  return 0;
}
```

4. 这里的node定义为一个指针，并不是指向一个结构体

```c
struct ht_node{
  int key;
  int data;
};
typedef struct ht_node* node;
node makeNnode(int k, int e){
  node cuur = malloc(sizeof(node));
  curr->key = k;
  curr->data = e;
}
```

5.`strcdup`函数返回了一个分配在栈中的指针，函数返回之后地址A可能会被抹掉

```c
char *strcdup(int n, char c){
  char dup[n+1];
  int i;
  for(i=0; i<n; ++i){
    dup[i] = c;
  }
  dup[i] = '\0';
  char *A = dup;
  return A;
}
```

6. `IS_GREATER`是一个没有带括号的宏，因此m1的值相当于`1>0+1=0`

```c
#define IS_GREATER(a, b) a>b;
inline int isGreater(int a, int b){
  return a>b ? 1:0;
}
int m1 = IS_GREATER(1,0)+1;
int m2 = isGreater(1,0)+1;
```

7. 在这里，b1指向`0x104`，b2指向`0x208`；**会根据类型的不同**，决定下一个`byte`的起始位置

```c
#define NEXT_BYTE(a)((char *)(a+1));
int a1 = 54;					//&a1 = 0x100
long long a2 = 42;    //&a2 = 0x200
void* b1 = NEXT_BYTE(&a1);
void* b2 = NEXT_BYTE(&a2);
```

> cache lab规定提交代码的列数不能超过80

## 3 Part A：Cache Simulator

> 目标：让./test-csim得满分

**write-up提示**：在A部分中，我们需要在`csim.c`中编写一个缓存模拟器，该模拟器将`valgrind`内存跟踪作为输入，模拟此跟踪上的高速缓存存储器的命中/未命中行为并输出命中/不命中和驱逐的总数目。

在`cachelab`的`handout`中提供了参考缓存模拟器的二进制可执行文件：`csim-ref`，在`valgrind`跟踪文件上模拟具有任意大小和关联性的高速缓存的行为，选择LRU「最近使用」替换策略

模拟器采用以下命令行参数：

```bash
./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>
```

> - `-h:` 可选择用于打印usage info
> - `-v:` 可选择用于显示trace info
> - `-s:` `<s>`组，其中（$S= 2^s$为组数）
> - `-E:` `<E>`指每个组的行数
> - `-b:` `<b>`块字节数，($B=2^b$为块大小(字节))
> - `-t:` `<tracefile>`valgrind操作的文件

