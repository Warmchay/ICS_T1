# Chapter 3: Machine Code Q&A

## 3.2 Code demo

### **3.2.1** `-Og` `-O1`有什么区别？

**`-O1:`**  Optimizing compilation takes somewhat more time, and a lot more memory for a large function.

`-Og:` Optimize debugging experience. -Og should be the optimization level of choice for the standard edit-compile-debug cycle, offering a reasonable level of optimization while maintaining fast compilation and a good debugging experience. It is a better choice than -O0 for producing debuggable code because some compiler passes that collect debug information are disabled at -O0.

其他优化命令：[gcc -Ox](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)

总的来说：-Og更常用，它能提供debug时需要的info

## 3.3 lab request---gdb

[gdb 快捷键文档](http://csapp.cs.cmu.edu/2e/docs/gdbnotes-x86-64.pdf)

[gdb 参数使用文档](https://visualgdb.com/gdbreference/commands/x)

## 3.4 访问信息

### 3.4.1 operands 的应用

[oracle 文档提供](https://docs.oracle.com/cd/E19253-01/817-5477/ennby/index.html)

### 3.4.2 操作数的值

`260(%ecx, %edx)`: `%ecx` `%edx`相加后再加偏移量260，但因寄存器地址值为`Hex`，所以需要先将260转为`h`进制才能相加

[stackoverflow relative discussion](https://stackoverflow.com/questions/52559116/assembly-att-format-operand-value)

### 3.4.3 movsbl & movszl

出现在char 转 int型：[relative explaination](https://www.cnblogs.com/johnnyflute/p/3597352.html)

`unsigned char = unsigned int`: 无符号位扩展【零扩展】，前补充0: `movzbl`

`unsigned char = signed int:` 有符号扩展，前补充符号位：`movsbl`

