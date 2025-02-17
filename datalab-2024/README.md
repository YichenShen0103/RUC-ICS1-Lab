[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/HwtZfpI1)
# DataLab

```
 ________  ________  _________  ________  ___       ________  ________     
|\   ___ \|\   __  \|\___   ___\\   __  \|\  \     |\   __  \|\   __  \    
\ \  \_|\ \ \  \|\  \|___ \  \_\ \  \|\  \ \  \    \ \  \|\  \ \  \|\ /_   
 \ \  \ \\ \ \   __  \   \ \  \ \ \   __  \ \  \    \ \   __  \ \   __  \  
  \ \  \_\\ \ \  \ \  \   \ \  \ \ \  \ \  \ \  \____\ \  \ \  \ \  \|\  \ 
   \ \_______\ \__\ \__\   \ \__\ \ \__\ \__\ \_______\ \__\ \__\ \_______\
    \|_______|\|__|\|__|    \|__|  \|__|\|__|\|_______|\|__|\|__|\|_______|
```

模板仓库：[RUC DataLab](https://github.com/RUCICS/datalab)

## 项目编译与环境指南

C 语言编译环境

```bash
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install gcc-multilib
```

如果有 C 语言环境问题，你可以选择删除 `gcc` 和 `g++` 然后重新执行上述指令。

```bash
sudo apt-get remove gcc g++
```

如果仍有问题，可以访问 https://ics.ruc.panjd.net 获取服务器登陆信息，在服务器上完成本实验。

--------

Python 编译环境已弃用，现在你不需要执行任何指令来配置，如果你第一次读到这个文档，请忽略这一部分。对于服务器用户，你不需要执行任何配置。

> Python 编译环境，我们演示 Virtualenv 下的配置方法，如果你使用 Conda 可以自行配置类似的环境出来
> 
> ```bash
> sudo apt-get update
> sudo apt-get install python3 python3-pip python3-venv
> python3 -m venv venv # 如果你在我们提供的服务器上进行本实验，这一步请从这里开始
> . venv/bin/activate
> pip install pycparser -i https://mirrors.aliyun.com/pypi/simple/
> ```

--------

如果你想获得 VSCode 的最佳代码提示体验，请指定 `cStandard` 为 `gnu99`:

```json
{
    "C_Cpp.default.cStandard": "gnu99"
}
```

> 面向高级用户，我们还提供了 devcontainer 的支持。有需要的同学可以自行使用。

### 如何编译

```bash
make
```

> 如果你不懂什么是 `make`，请询问 AI 生成工具 “什么是 make，我应该在什么时候执行 make，make 的原理是什么”

### Quick Start

```bash
# 在我们的服务器上，不需要执行这一行
sudo apt-get update && sudo apt-get install build-essential gcc-multilib

# 其中 panjd123 改成你的用户名
git clone git@github.com:RUCICS/datalab-panjd123.git

cd datalab-panjd123

make
python3 test.py
# 然后你应该看到最后一行输出 Total Points:0 的字样
```

## 实验简述

本实验考察对数的二进制表示和位运算的认识。具体来说，本实验会给出多个函数要求实现，比如求两个数的异或，但你只能用限制范围内的位运算操作符（有时候允许更多）。

我们的题目和你们要完成的函数都在 [`bits.c`](./bits.c) 文件中。

### 一个例子

```c
/* 
 * bitAnd - x & y using only ~ and |
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ |
 *   Max ops: 7
 *   Difficulty: 1
 */
int bitAnd(int x, int y) {
    return ~(~x | ~y);
}
```

如在 `bitAnd` 题目中，你需要修改代码，使得：

1. 保证函数的返回值符合要求
2. 保证只使用了 `Legal ops` 中给出的符号
3. 符号数的总数不能超过 `Max ops`

### 具体要求

你被允许：

- 对于整型题目，即 `float_i2f` 之前的题目，你被允许定义 `int` 变量，即不能定义 `unsigned`，除非题目允许
- 对于浮点数题目，即 `float_i2f` 及之后的题目，你被允许定义 `int` 和 `unsigned` 的变量

你被禁止：

- 禁止使用控制流语句，比如 `if`, `else`, `for`, `while`，除非题目允许
- 禁止使用非位运算符号，比如 `&&`, `||`, `-`, `?:`,`!`, `>`, `==` 除非题目允许
- 禁止使用其他不在 `Legal ops` 中的操作符
- 特别说明，例如 `<<=`, `++` 这类 `x = x ? 1` 的 `?=` 符号，我们视作 `?` 符号，换句话说，只要 `?` 在 `Legal ops` 里，则 `?=` 也在。
- 禁止超过 `Max ops` 个符号数
- 禁止使用函数，或调用任何函数
- 禁止使用宏
- 禁止定义其他类型的变量，比如 `float`，`char`，结构体，数组，联合体等
- 禁止使用类型转换：
    - 我们绝对禁止显式类型转换，如 `(float)x` 和 `(int)(1.0f)`
    - 对于隐式类型转换，如果你感觉你的解法利用了隐式类型转换，那你应该尽量避免；如果你的解法不依赖于该隐式类型转换，则无所谓。如有符号数和无符号数计算时，有符号数会被隐式类型转换为无符号数
- 禁止使用任何浮点常量，比如 `1.0f`，当然你可以使用它的二进制形式代表的整数，注意不要违反上一条规则
- 禁止使用 Undefined Behavior，比如对一个 32 位数逻辑右移超过 31 位，你应该假设我们的机器会在这时候返回不确定的结果，不得利用 Undefined Behavior，以免我们跨平台编译的时候结果不一致
- 我们要求你的编译不能有任何警告，在迫不得已的情况下，请尽量用一些代码内编译器指令来消除警告，比如 `__attribute__((unused))`
- 禁止修改除了 `bits.c` 以外的文件
- 禁止给 `bits.c` 文件添加任何头文件，这会导致测试错误。这不影响你使用 `printf` 函数，
但使用 `printf` 函数会与 `test.py` 脚本冲突，你可以使用下文的 `btest` 脚本来调试正确性，
正确性通过后再删除 `printf` 使用 `test.py` 脚本来测试合规性

其他事项：

- 符号数计数不包括赋值等号 `=` 和控制流语句如 `if`, `for` 这种关键字，但例如条件表达式内的符号还是会被计算

### 如何评测

```bash
python3 test.py
python3 test.py -V # 显示更多信息
```

我们提供了一个 `test.py` 脚本，它完成以下判断：

- 尝试编译你的代码
- 测试正确性，即是否能返回函数正确的结果
- 测试合规性，即是否满足题目对符号数等的要求

该脚本要求你的代码不能包含输出到 stdout 的调试输出，如果你需要调试，请用下文的 `btest` 脚本。
通常情况下，你可以只用 `btest` 测试，直到所有函数都能返回正确结果，再回到 `test.py` 测试。

输出示例：

```bash
bitXor          1/1:     PASS    detail: Pass using 7 operations including {'~', '&'} and {'int'}.
samesign        2/2:     PASS    detail: Pass using 8 operations including {'&&', '>>', '^', '!'} and {'if', 'int'}.
logtwo          0/4:     FAIL    error1: Test logtwo(1[0x1]) failed. Gives 2[0x2]. Should be 0[0x0]
                                 error2: Using illegal operations: {'+', '<', 'for'}.
                                 error3: Using excessive operations, you use 14 > max ops 12.
                                 error4: Using type conversion.
...
Total points: 3
```

> 该输出展示了一份正确通过了前两个函数，但是 logtwo 函数实现中有诸多错误的作业。

> `-V` 参数会显示通过的函数所用操作符数量，但可能导致你的报错不明显，默认我们只显示 FAIL 的信息。

### 如何使用我们提供的工具

#### btest

`btest` 是正确性检验工具，是 `test.py` 脚本的一部分，你可以根据需要单独使用它以方便调试。

执行正确性测试

```bash
make
./btest
```

以指定参数调用某个函数测试：

```bash
make
./btest -1 1 -2 2 -f bitXor # bitXor(1, 2)
./btest -h # 查看 btest 的其他功能
```

#### ishow

```bash
./ishow 0x27
Hex = 0x00000027,	Signed = 39,	Unsigned = 39
```

#### fshow

```bash
./fshow 0x15213243
Floating point value 3.255334057e-26
Bit Representation 0x15213243, sign = 0, exponent = 0x2a, fraction = 0x213243
Normalized.  +1.2593463659 X 2^(-85)
```

### 最终得分

DataLab 本次设置为通过性实验，即测试点通过则为满分。

### 实验报告

作为本学期的第一个实验，我们在此说明我们每个实验的报告提交要求。

首先，所有实验都遵循相同的通用要求，具体见 [`report/README.md`](report/README.md)。

每个实验又会提供专用的实验模板，其中包含了具体助教想要你们填写的内容，具体见 [`report/DataLab.md`](report/report.md)。

### 提交

将仓库 push 到上游 Github 仓库里即可完成提交。

## 注意事项

1. 不允许抄袭，助教会将每个人代码同网上代码和其他同学代码进行查重
2. 报告需要写清楚做题思路，否则会酌情扣分
3. 慎用代码格式化工具，如果一定要使用，请用 [`.clang-format`](./.clang-format) 文件，如果因格式化工具导致 `bits.c` 外的文件被修改，可能会被视为作弊
