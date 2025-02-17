# RUC-ICS1-Lab

> [!WARNING]
> This is a repository for the lab assignments of the course Introduction to Computer Systems (ICS1) at Renmin University of China, **which is for reference only. Please do not copy the code directly.** There is actually a strict code duplication checking after the submission of each lab assignment. If you copy the code directly, you will be caught by the system and get a zero score for the assignment, which may be hard for you to afford.


## WHY THIS REPO?

1. I want to delete the independent repository for each lab from my github account.
2. I want to keep a record of my lab assignments.
3. To help others RUCers who are taking the same course or even students in other universities who takes similar course just know basically how each lab asks us to do.


## STRUCTURE OF THIS REPO

```
.
├── README.md
├── attacklab-2024
│   ├── README.md
│   ├── p1
│   ├── p2
│   ├── p3
│   ├── p4
│   └── reports
├── bomblab-2024
│   ├── Bomblab指导_2024ver.pdf
│   ├── README.md
│   └── report
├── cachelab-2024
│   ├── Dockerfile
│   ├── Makefile
│   ├── README.md
│   ├── cachelab.h
│   ├── common.h
│   ├── csim-ref
│   ├── csim-ref2
│   ├── csim-show
│   ├── csim.c
│   ├── csim.cpp
│   ├── demo.cpp
│   ├── gemm.cpp
│   ├── gemm.cpp.template
│   ├── gemm.h
│   ├── gemm_baseline.cpp
│   ├── imgs
│   ├── main.cpp
│   ├── matrix.cpp
│   ├── matrix.h
│   ├── parabuild_example
│   ├── report
│   ├── show_miss
│   ├── submit_gemm.sh
│   ├── test
│   ├── test_case.cpp
│   ├── test_case.h
│   └── traces
├── datalab-2024
│   ├── Dockerfile
│   ├── Makefile
│   ├── README.md
│   ├── bits.c
│   ├── bits.h
│   ├── btest.c
│   ├── btest.h
│   ├── decl.c
│   ├── fshow.c
│   ├── ishow.c
│   ├── pycparser
│   ├── report
│   ├── test.py
│   └── tests.c
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

## ABOUT THIS COURSE

Introduction to Computer System 1 (ICS1) is a required course for undergraduate students majoring in computer science at Renmin University of China. This course is designed to help students understand the basic concepts of computer systems, including the basic structure of a computer, the representation of data, the assembly language, the memory hierarchy and the linking process. 

ICS1 is a revision of the first half of CMU 15-213, which is a classic course in computer science.

## ABSTRACTIONS AND HINTS FOR EACH LAB

### Lab 1: Data Lab

Implement several functions with a limited range and number of operators. You are expected to be more familiar with the bitwise operations and the IEEE-754 standard after finishing this lab.

**Hints:**
1. You may only modify the file `bits.c`.
2. Carefully read the comments of each problems, which may contain some special limit.
3. Try not to pursue the solution with least operators in your first attempt. It may be hard to find the optimal solution at the beginning. Baseline and then optimization may be a good strategy.
4. Read the book (especially the IEEE-754 standard) and slides carefully. It may help you a lot.

### Lab 2: Bomb Lab

Given a binary file `bomb`, you are expected to defuse the bomb by figuring out the correct input string. You will need to read the assembly code and find out thems. You are expected to get a better understanding of how the program works in the machine level.

**Hints:**
1. You don't need to modify any files.
2. You may need to use `gdb` to safely execute the bomb and find out what is really going on in the machine level. You may also need to use `objdump` to disassemble the binary file. If it is too hard for you to defuse the bomb, you may try to use `IDA pro` to find out the correct input string. (I highly **don't recommend** this, because it is not the purpose of this lab)
3. Before you start, make sure you know the memory layout of a program and the stack frame layout of a function call. You will find it very useful when you are trying to understand the assembly code.
4. Additionally, my personal suggestion is to split each phrase into several parts by detecting the control flow of the program and figure out what each part does. It may help you to understand the program more easily.

### Lab 3: Attack Lab

This is actually not a formal lab but a homework in Fall 2024.

Similar like the bomb lab, you are expected to read the assembly code and find out the correct input strings. Yet this time, you are a hacker not a defender. You are expected to know more deeply how the buffer overflow attack works and avoid wrting a dangerous program.

**Hints:**
1. You don't need to modify any files.
2. Using a python script to generate the input strings may be a good idea.

### Lab 4: Cache Lab

Part A: Implement a LRU cache simulator.

Part B: Optimize the matrix multiplication function to make it more cache-friendly in three cases.

**Hints:**
1. You may only modify the file `csim.c` in Part A and the file `gemm.c` in Part B.
2. There are several ways to implement the LRU cache simulator like maintaining a time stamp, using a circular linked list or queue.
3. There are sufficient hints for part B in cachelab-2024/README.md. Read it carefully and you will find it helpful.

### Lab 5: Link Lab

Implement a toy linker `ld` to link the object files into an executable file. To simplify the problem, a special `FLE(Friendly Linking Executable)` format is used.

**Hints:**
1. You may only modify the file `ld.c` and `nm.c` in src/student.
2. The README.md of this lab is well organized yet a little bit hard to understand. I will recommend you to carefully read the code framework and test cases to be more familiar with the FLE format and the requirements of this lab.
3. Follow the steps the README.md tells you to do. It may help you to finish the lab more easily.

## THINGS YOU SHOULD KNOW

1. As a special reminder, these stuffs get full credit ( 100 [lab + HW + mid-term] + 92 [final] = 98 ) in 2024.
2. I will not maintain this repository. No PRs will be accepted. While you can still open an issue if you find any mistakes in this repository or if you have some questions about this course.
3. If you want to reference any code in this repository, please feel free to do so. You don't need to ask for permission or even site this repo as reference. Yet, if you are taking this course, please do not copy the code.
4. If you find this repository helpful, please give it a star 🌟. It will be a great encouragement for me to keep writing.
