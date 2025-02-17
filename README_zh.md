# RUC-ICS1-Lab 中文版介绍


> [!WARNING]
> 本仓库是中国人民大学《计算机系统导论（ICS1）》课程的实验作业存档，**仅供学习参考，请勿直接抄袭代码**。实验提交后会有严格的代码查重，若直接复制代码将被系统判定抄袭并导致作业零分，这一后果可能难以承受。

<p align="center">
    <a href="README.md">
        <img src="https://img.shields.io/badge/Translate%20to-English-8A2BE2" alt="Translate to English">
    </a> 
</p>

<p align="center">
    <img src="https://img.shields.io/github/stars/YichenShen0103/RUC-ICS1-Lab" alt="stars">
    &nbsp
    <img src="https://img.shields.io/github/issues/YichenShen0103/RUC-ICS1-Lab?style=social" alt="issues">
</p>

## 为什么建立这个仓库？

1. 我希望从GitHub账户中删除各个实验的独立仓库。
2. 希望保留实验作业的存档记录。
3. 帮助后续选修该课程的RUC同学，甚至其他高校选修类似课程的同学了解各个实验的基本要求。


## 仓库结构

```
.
├── README.md
├── attacklab-2024     
│   ├── README.md
│   ├── p1               
│   ├── p2              
│   ├── p3               
│   ├── p4               
│   └── reports          
├── bomblab-2024        
│   ├── Bomblab指导_2024ver.pdf
│   ├── README.md
│   └── report           
├── cachelab-2024       
│   ├── Dockerfile
│   ├── Makefile
│   ├── README.md
│   ├── cachelab.h
│   ├── common.h
│   ├── csim-ref         
│   ├── csim-ref2
│   ├── csim-show
│   ├── csim.c           
│   ├── csim.cpp
│   ├── demo.cpp
│   ├── gemm.cpp         
│   ├── gemm.cpp.template
│   ├── gemm.h
│   ├── gemm_baseline.cpp
│   ├── imgs           
│   ├── main.cpp
│   ├── matrix.cpp
│   ├── matrix.h
│   ├── parabuild_example
│   ├── report          
│   ├── show_miss
│   ├── submit_gemm.sh
│   ├── test
│   ├── test_case.cpp
│   ├── test_case.h
│   └── traces           
├── datalab-2024      
│   ├── Dockerfile
│   ├── Makefile
│   ├── README.md
│   ├── bits.c       
│   ├── bits.h
│   ├── btest.c
│   ├── btest.h
│   ├── decl.c
│   ├── fshow.c
│   ├── ishow.c
│   ├── pycparser
│   ├── report         
│   ├── test.py
│   └── tests.c
└── linklab-2024      
    ├── Makefile
    ├── README.md
    ├── grader.py       
    ├── grader_config.toml
    ├── include          
    ├── report           
    ├── requirements.txt
    ├── src             
    └── tests           
```

## 关于课程

《计算机系统导论1》（ICS1）是中国人民大学计算机类专业的本科必修课程。本课程旨在帮助学生建立计算机系统的基本概念，内容包括计算机基本结构、数据表示、汇编语言、存储层次结构和链接过程等。

ICS1课程改编自CMU 15-213课程的前半部分内容，后者是计算机科学领域的经典课程。

## 预备知识推荐

如果您对`git`、`linux`、`ssh`或`shell`等工具不熟悉，推荐学习：
- MIT The Missing Semester of Your CS Education 

## 各实验概要提示

### 实验1：Data Lab

使用有限的操作符实现若干函数。通过本实验，您将加深对位操作和IEEE-754浮点数标准的理解。

**提示：**
1. 仅允许修改`bits.c`文件
2. 仔细阅读每个问题的注释，可能包含特殊限制
3. 初次尝试时不必追求最少操作符的解法，可采用"先实现后优化"策略
4. 仔细阅读教材（特别是IEEE-754标准）和课件

### 实验2：Bomb Lab

给定二进制文件`bomb`，通过逆向工程分析出正确的输入字符串来拆除炸弹。您需要阅读汇编代码并破解密码，从而深入理解程序在机器级的执行过程。

**提示：**
1. 无需修改任何文件
2. 推荐使用`gdb`进行动态调试，结合`objdump`反汇编工具分析
3. 了解程序内存布局和函数调用栈帧结构将大有裨益
4. 建议通过控制流分析将程序分段，逐步理解各模块功能

### 实验3：Attack Lab（2024年作为作业）

本实验实际上是2024年秋季的课程作业。与炸弹实验类似，但角色转换为攻击者。您需要通过缓冲区溢出攻击实现特定目标，从而深入理解程序安全漏洞。

**提示：**
1. 无需修改文件
2. 使用Python脚本生成攻击字符串可能更高效

### 实验4：Cache Lab

A部分：实现LRU缓存模拟器

B部分：在三种场景下优化矩阵乘法，提升缓存命中率

**提示：**
1. A部分仅修改`csim.c`，B部分修改`gemm.c`
2. 实现LRU策略可采用时间戳、循环链表等多种方式
3. 实验说明文件（cachelab-2024/README.md）包含详细优化提示

### 实验5：Link Lab

实现简化版链接器`ld`，将目标文件链接为可执行文件。实验使用自定义的FLE（友好链接可执行）格式。

**提示：**
1. 仅需修改src/student目录下的`ld.c`和`nm.c`
2. 建议结合代码框架和测试用例理解FLE格式
3. 按照README的步骤分阶段实现

## 后续课程推荐

- MIT 6.S081 操作系统工程
- MIT 6.824 分布式系统
- Stanford CS144 计算机网络
- Stanford CS143 编译原理
- CMU 15-445 数据库系统

## 重要声明

1. 本仓库内容在2024年春季学期获得满分（实验+作业+期中100分，期末92分，总评98分）
2. 本仓库不再维护，不接受PR请求。如有发现错误或课程相关问题，欢迎提交issue
3. 本仓库代码可自由参考，无需授权或注明出处（课程学生请勿直接抄袭）
4. 如果觉得本仓库有帮助，欢迎点亮⭐，这是对作者持续创作的最大鼓励
5. Vim是世界上最棒的代码编辑器！