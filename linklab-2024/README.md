[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/Vg2X8l0l)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=17598784&assignment_repo_type=AssignmentRepo)
# LinkLab 2024：构建你自己的链接器

```
 ___       ___  ________   ___  __    ___       ________  ________
|\  \     |\  \|\   ___  \|\  \|\  \ |\  \     |\   __  \|\   __  \
\ \  \    \ \  \ \  \\ \  \ \  \/  /|\ \  \    \ \  \|\  \ \  \|\ /_
 \ \  \    \ \  \ \  \\ \  \ \   ___  \ \  \    \ \   __  \ \   __  \
  \ \  \____\ \  \ \  \\ \  \ \  \\ \  \ \  \____\ \  \ \  \ \  \|\  \
   \ \_______\ \__\ \__\\ \__\ \__\\ \__\ \_______\ \__\ \__\ \_______\
    \|_______|\|__|\|__| \|__|\|__| \|__|\|_______|\|__|\|__|\|_______|
```

> 每个程序员都用过链接器，但很少有人真正理解它。
>
> 在这个实验中，你将亲手实现一个链接器，揭开程序是如何被「拼接」在一起的秘密。我们设计了一个友好的目标文件格式（FLE），让你可以专注于理解链接的核心概念。

> [!WARNING]
> 这是 LinkLab 的第一个版本，可能存在一些问题。如果你：
>
> - 发现了 Bug，请[提交 Issue](https://github.com/RUCICS/LinkLab-2024-Assignment/issues)
> - 有任何 Question，请在[讨论区](https://github.com/RUCICS/LinkLab-2024-Assignment/discussions)提出
>
> 也欢迎你积极参与开源协作，改进框架代码解决 Issue、或是为同学们答疑解惑。这部分的表现可被计为[额外的分数](#评分标准)。

> [!NOTE]
> 本实验预计耗时 7 - 15 个小时，具体情况因个体差异可能有所区别。

[![GitHub Issues](https://img.shields.io/github/issues/RUCICS/LinkLab-2024-Assignment?style=for-the-badge&logo=github)](https://github.com/RUCICS/LinkLab-2024-Assignment/issues)

## 什么是链接？

链接是将多个目标文件组合成一个可执行程序的过程。在现代软件开发中，我们不会把所有代码都写在一个文件里——这样既不利于代码复用，也不便于团队协作。相反，我们会将程序分解成多个源文件，分别编译成目标文件，再通过链接器将它们「拼接」在一起。

链接器的主要工作包括：

1. 符号解析：将代码中的符号（如函数调用、全局变量）与它们的定义对应起来
2. 重定位：调整符号的地址，确保程序在内存中正确运行
3. 布局规划：合理安排各个部分在内存中的位置，并设置适当的访问权限

这个过程看似简单，但实际上涉及许多复杂的细节：如何处理符号冲突？如何修正目标地址？如何保护代码不被篡改？在这个实验中，你将亲手实现这些功能，深入理解现代程序是如何被组装起来的。

## 环境要求

- 操作系统：Linux（推荐 Ubuntu 22.04 或更高版本）
  - Windows 用户可考虑使用 WSL 2
- 编译器：支持 C++17
  - g++-8 及以上，推荐使用最新版本
  - clang 的较新版本
- Python 3.6+
- Make, Git

请使用 Git 管理你的代码，养成经常提交的好习惯。

## 快速开始

```bash
# 克隆仓库（请将 your-assignment-repo-url 替换为你的仓库地址）
git clone your-assignment-repo-url
cd your-assignment-repo-name

# 构建项目
make

# 运行测试（此时应该会失败，这是正常的）
make test_1  # 运行任务一的测试
make test    # 运行所有测试
```

## 项目结构

```
LinkLab-2024/
├── include/                  # 头文件
│   └── fle.hpp               # FLE 格式定义（请仔细阅读）
├── src/
│   ├── base/                 # 基础框架（助教提供）
│   │   ├── cc.cpp            # 编译器前端，生成 FLE 文件
│   │   └── exec.cpp          # 程序加载器，运行生成的程序
│   └── student/              # 你需要完成的代码
│       ├── nm.cpp            # 任务一：符号表查看器
│       └── ld.cpp            # 任务二~六：链接器主程序
└── tests/                    # 测试用例
    └── cases/                # 按任务分类的测试
        ├── 1-nm-test/        # 任务一：符号表显示
        ├── 2-single-file/    # 任务二：基础链接
        └── ...               # 更多测试用例
    └── common/               # 测试用例的公共库
        └── minilibc.c        # 迷你 libc 的实现
```

每个任务都配有完整的测试用例，包括：

- 源代码：用于生成测试输入
- 期望输出：用于验证你的实现
- 配置文件：定义测试参数

你可以：

1. 阅读测试代码了解具体要求
2. 运行测试检查实现是否正确
3. 编写新的测试用例来探索更多可能性

## 任务零：理解目标文件格式

Linux 使用 **ELF 格式**来组织和存储可执行文件、目标文件和共享库，非常强大和灵活。然而，这种格式并不是面向人类可读而设计的，它的表达形式和隐含的大量琐碎细节对于初学者而言有极大的认知成本。为了让你专注于链接的核心概念，我们重新设计了 *FLE (Friendly Linking Executable)* ，提供一种更平坦、更人类友好和更符合行为局部性原则（Locality of Bahaviour Principle）的格式。


以下是一个简单的例子，展示了 C 代码及其编译后的目标文件内容（但是 FLE 格式）：

```c
// test.c
int message[2] = {1, 2};  // 全局数组

static int helper(int x) { // 静态函数
    return x + message[0];
}

int main() {             // 程序入口
    return helper(42);
}
```

通过 `./cc test.c -o test.o`，可生成 `test.fle` 内容如下：

```json5
{
    "type": ".obj", // 这是一个目标文件
    "shdrs": [
        // 各个节的元数据
        {
            "name": ".text", // 节名
            "type": 1, // 节类型
            "flags": 1, // 节权限
            "addr": 0, // 节在内存中的起始地址
            "offset": 0, // 节在文件中的偏移量
            "size": 36 // 节的大小
        },
        {
            "name": ".data",
            "type": 1,
            "flags": 1,
            "addr": 0,
            "offset": 36,
            "size": 8
        },
        {
            "name": ".bss",
            "type": 8,
            "flags": 9,
            "addr": 0,
            "offset": 44,
            "size": 0
        }
    ],
    ".text": [
        "🏷️: helper 20 0", // 局部符号
        "🔢: 55 48 89 e5 89 7d fc 8b 15", // 机器码
        "❓: .rel(message - 4)", // 需要重定位
        "🔢: 8b 45 fc 01 d0 5d c3", // 机器码
        "📤: main 16 20", // 全局符号
        "🔢: 55 48 89 e5 bf 2a 00 00 00 e8 de ff ff ff 5d c3" // 机器码
    ],
    ".data": [
        "📤: message 8 0", // 全局符号
        "🔢: 01 00 00 00 02 00 00 00" // 数据
    ],
    ".bss": []
}
```

FLE 使用表情符号来标记不同类型的信息：

1. 机器码 `字节 字节 字节 ...` 🔢
2. 符号 `符号名 大小 节内偏移量`
  - 文件内部的局部符号 🏷️
  - 可以被其他文件引用的全局符号（强符号） 📤
  - 可以被覆盖的全局弱符号 📎
3. 重定位 `重定位类型(目标符号名 [+/-] 附加值)` ❓

这些信息在内存中用 C++ 结构体 `FLEObject` 表示（定义在 [`include/fle.hpp`](include/fle.hpp) 中）：

```cpp
struct FLEObject {
    std::string name; // Object name
    std::string type; // ".obj" or ".exe"
    std::map<std::string, FLESection> sections; // Section name -> section data
    std::vector<Symbol> symbols; // Global symbol table
    std::vector<ProgramHeader> phdrs; // Program headers (for .exe)
    std::vector<SectionHeader> shdrs; // Section headers
    size_t entry = 0; // Entry point (for .exe)
};

struct FLESection {
    std::vector<uint8_t> data; // Section data (stored as bytes)
    std::vector<Relocation> relocs; // Relocation table for this section
    bool has_symbols; // Whether section contains symbols
};

// …
```

我们的实验框架会自动将磁盘上的 *FLE 格式文件*加载为内存中的*结构体 `FLEObject`*，这个过程也叫**反序列化**。在实现链接器时，你可能更多需要关注 `FLEObject`；FLE 格式文件主要是为了方便查看和调试。

> [!IMPORTANT]
> FLE 格式文件和 `FLEObject` 大部分内容是对应的，但在某些数据的表示上有所不同。具体来说：
>
> - 在 FLE 格式文件中，重定位和符号定义是内联的。符号定义的地方直接用 📤 / 📎 / 🏷️ 标记；而在 `FLEObject` 中，符号表是独立的，包含每个符号及其定义位置的信息，并且每个 `FLESection` 中有独立的重定位表。
> - 在 FLE 格式文件中，需要重定位的地方直接用 ❓ 内联标记，**不存在占位的 0 字节**；而在每个 `FLESection` 的 `data` 字段中，需要重定位的地方会用 0 占位。


在接下来的任务中，你将逐步实现处理这种格式的工具链，从最基本的符号表查看器开始，最终实现一个完整的链接器。

准备好了吗？让我们开始第一个任务！

## 任务一：窥探程序的符号表

你有没有遇到过这样的错误？

```
undefined reference to `printf'
multiple definition of `main'
```

这些都与符号（symbol）有关。符号就像程序中的"名字"，代表了函数、变量等。让我们通过一个例子来理解：

```c
static int counter = 0;       // 静态变量：文件内可见
int shared = 42;              // 全局变量：其他文件可见
extern void print(int x);     // 外部函数：需要其他文件提供

void count() {                // 全局函数
    counter++;                // 访问静态变量
    print(counter);           // 调用外部函数
}
```

这段代码中包含了几种不同的符号：

- `counter`：静态符号，只在文件内可见
- `shared`：全局符号，可以被其他文件引用
- `print`：未定义符号，需要在链接时找到
- `count`：全局函数符号

你的第一个任务是补充 [src/student/nm.cpp](src/student/nm.cpp) 中的 `FLE_nm` 函数，实现一个工具来查看这些符号。

对于上面的代码，它应该输出三行三列信息：

```
0000000000000000 T count    # 全局函数在 .text 节
0000000000000000 b counter  # 静态变量在 .bss 节
0000000000000000 D shared   # 全局变量在 .data 节
```

每一列的具体含义是：

- 地址：符号**在其所在节中**的偏移量
- 类型：符号的类型和位置
  - 大写（T、D、B、R）：全局符号，分别表示在代码段、数据段、BSS 段、只读数据段
  - 小写（t、d、b、r）：局部符号，分别表示在代码段、数据段、BSS 段、只读数据段
  - W、V：弱符号，分别表示在代码段、还是在数据段或 BSS 段
- 名称：符号的名字

要实现这个工具，你需要遍历符号表，确定每个符号的类型，并按格式打印信息。

> [!TIP]
> 格式化输出可以使用:
>  ```cpp
>  std::cout << std::setw(16) << std::setfill('0') << std::hex << addr;  // C++ 风格
>  ```
>  或
>  ```c
>  printf("%016lx", addr);  // C 风格,输出16位的十六进制数,左侧补0
>  ```
>  以及，在 C++ 20 后，`std::format` 和 `std::print` 提供了更简洁的格式化方案（本实验使用 `-std=c++17` 编译选项，不支持此特性。此方案仅供了解）
>  ```cpp
>  std::print("{:016x}", addr);  // Modern C++ 风格
>  ```

> [!TIP]
> 你可以根据符号的 section 字段判断其位置:
>   - ".text" - 代码段
>   - ".data" - 数据段
>   - ".bss" - BSS段
>   - "" (empty) - 未定义符号
>       - 你应该在 `FLE_nm` 的输出中，忽略所有未定义符号

完成后，运行测试来验证：

```bash
make test_1
```

## 任务二：实现基础链接器

### 示例

让我们从一个简单的例子开始理解链接器的工作原理。假设有这样一个程序：

```c
// foo.c
const char str[] = "Hello, World!";  // 一个全局字符串常量

// main.c
extern const char str[];  // 声明：str 在别处定义

void _start()
{
    int ans = 0;
    for (int i = 0; i < 10; i++) {
        ans += str[i];  // 需要找到 str 的实际位置
    }
    // ... 退出系统调用 ...
}
```

编译器会将这些源文件编译成目标文件。

复习下[任务零](#任务零理解目标文件格式)的内容：我们有两种等价方式来表达这个目标文件，一种是磁盘上的 FLE 格式文件，一种是内存中的 `FLEObject` 结构体。

以 `foo.fle` 为例，它在磁盘上的格式文件是：

```json5
{
    "type": ".obj",
    "shdrs": [ /* ... 节头（略） ... */ ],
    ".text": [],
    ".rodata": [
        "📤: str 14 0",    // 定义一个全局符号 str，大小为 14 字节
        "🔢: 48 65 6c 6c 6f 2c 20 57 6f 72 6c 64 21 00"   // "Hello, World!" 的字节表示
    ]
}
```

当这个文件被加载到内存时，它会被解析成一个 `FLEObject` 结构。如上文所述，其中的 `symbols` 成员是一系列 `Symbol` 结构的列表，每个 `Symbol` 结构存储着符号及其定义位置的信息。在上例中，假设我们用 `obj` 来表示这个 `FLEObject`，那么 `obj.symbols[0]` 是：

```cpp
Symbol {
    .type = SymbolType::GLOBAL,  // 对应文件中的 📤
    .section = ".rodata",        // 符号所在的节是只读数据段
    .offset = 0,                 // 在节内的偏移
    .size = 14,                  // 符号大小为 14 字节
    .name = "str"                // 符号名称
}
```

它本质上是在说，符号 `str` 对应那么一个指向 `.rodata+0` 至 `.rodata+14` 的「指针」。

这个 `FLEObject` 中，还有个 `sections` 成员，是一系列 `FLESection` 结构的列表，其中包含了一个对应 `.rodata` 节的 `FLESection` 对象，即 `obj.sections[".rodata"]`。这样，我们就看到了 `Symbol` 对象 `str` 所指向的具体内容，

```cpp
FLESection {
    .data = { 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 
              0x6f, 0x72, 0x6c, 0x64, 0x21, 0x00 }  // "Hello, World!"
}
```

以上是 `foo.fle` 目标文件的两种表示；类似地，对于 `main.fle`，其磁盘上的格式文件为：

```json5
{
    "type": ".obj",
    "shdrs": [
        {
            "name": ".text",
            "type": 1,
            "flags": 1,
            "addr": 0,
            "offset": 0,
            "size": 32
        },
        // 其他节（略）
    ],
    ".text": [
        "📤: _start 32 0",    // 定义一个全局符号 _start
        "🔢: 31 c0 31 d2 0f be 88",  // 机器码
        "❓: .abs32s(str + 0)",  // 需要重定位：填入 str 的地址
        "🔢: 48 ff c0 01 ca 48 83 f8 0a 75 ee 89 d7 b8 3c 00",
        "🔢: 00 00 0f 05 c3"
    ],
    // 其他节（略）
}

```

同样，在 `FLEObject` 中，每个 `FLESection` 对象都进一步有一个 `relocations` 成员，它是一系列 `Relocation` 结构的列表。比如，
`obj.sections[".text"].relocations[0]` 是：

```cpp
Relocation {
    .type = RelocationType::R_X86_64_32S,  // 32位有符号绝对寻址
    .offset = 7,                           // 重定位在节内的位置
    .symbol = "str",                       // 目标符号
    .addend = 0                            // 偏移量
}
```

这本质上是在说，有那么一个指向 `.text+7` 至 `.text+11` 的「指针」，其值需要被修正为符号 `str` 的地址。

### 链接过程

[链接器](#什么是链接)只做三件事：布局规划！符号解析！重定位！

最后还要生成可执行文件。

#### 布局规划

首先，链接器将各个目标文件中的相同节（`.text`、`.rodata` 等）合并，形成一个最终的内存布局。

> [!TIP]
> 在本任务中，你可以先选择简单地将所有目标文件的所有节都按顺序合并到一个 `.load` 节中，而不是生成 `.text`、`.rodata` 等节、并赋予它们不同的权限 —— 节分离是[任务六](#任务六分离代码和数据)的内容。

#### 符号解析

在节合并后，链接器需要找到每个符号的定义，把每个符号关联到合并后的内存布局中。也就是记录下每个符号都去了哪里，这样你在重定位时才能找到它。

> [!TIP]
> 你可以用 `0x400000` 作为程序的基址，意思是，`.load` 段在运行时将被加载到 `0x400000`。
> 
> 这样，如果你已经把 `main.fle` 中大小为 `32` 的 `.text` 节合并进 `.load` 段了，接下来你要合并 `foo.fle` 的 `.data` 节，这个节在运行时的起始地址就应该是 `0x400020`。从而，在这个节中定义的 `offset` 为 `0` 的符号 `str` 的地址为 `0x400020`。

> [!TIP]
> 你可以边合并节，边做符号解析。比如，你可以合并完某个目标文件的某个节后，找到在这个节中定义的符号，把每个符号的 `offset` 字段，设置为它在合并后的新位置，并把每个更新后的 `Symbol` 都加入到全局符号表中。

> [!IMPORTANT]
> 在处理目标文件的符号表时，你会发现单个目标文件的符号表中可能包含未定义符号（即在当前文件中被引用但尚未定义的符号）。这与标准 ELF 格式的行为是一致的。处理这些符号时：
> - 不要立即报告未定义错误
> - 在后续的目标文件中继续查找该符号的定义
> - 只有在处理完所有目标文件后，仍然找不到定义时才报错

#### 重定位

得到全局符号表后，链接器需要遍历所有的重定位项，根据重定位项的类型，计算要填入的值，更新重定位项所指向的地址引用。

> [!TIP]
> 重定位类型决定了如何计算要填入的值。
> 
> 在本任务中，你只需要处理 `R_X86_64_32` 和 `R_X86_64_32S` 类型的重定位 —— 它们都是直接将符号的绝对地址填入重定位位置，即**绝对重定位**。

> [!NOTE]
> `R_X86_64_32` 和 `R_X86_64_32S` 都会将 64 位地址截断为 32 位，区别在于链接器如何验证这个截断是否合法：
> - `R_X86_64_32`：要求截断掉的高 32 位必须为 0，这样通过零扩展可以还原出原始的 64 位值
> - `R_X86_64_32S`：要求高 32 位的值与截断后的符号位（即第 32 位）相同（全 0 或全 1），这样才能通过符号扩展还原出原始的 64 位值
>
> 你可以简单地忽略它们之间的区别。

仍以上述的 `main.fle` 为例，对于重定位项 `❓: .abs32s(str + 0)`，类型为 `R_X86_64_32S`，代表 `text+7` 至 `text+11` 这段空间的值需要被修正为 `str` 的地址（这里的 `+ 0` 没有什么意义，附加值不适用于绝对重定位），链接器需要：
1. 在合并后的全局符号表中查找 `str` 符号，获取符号地址 `0x400020`
2. 计算要填入的值
    - 对于 `R_X86_64_32S` 重定位类型，即直接截断符号地址的低 32 位

    $$
    \text{最终值} = \text{符号地址} \mathbin{\\&} 0xFFFFFFFF = 0x400020 \mathbin{\\&} 0xFFFFFFFF = 0x400020
    $$

    - 验证：这个截断后的值是合法的，通过零扩展可以还原出原始的 64 位值
3. 将 `0x400020` 填入 `text+7` 至 `text+11` 这段空间，得到 `20 00 40 00`。

> [!TIP]
> - 注意字节序的问题。x86-64 使用小端序。
> - 注意要加上基地址。

#### 可执行文件生成

最后，生成可执行文件，填写文件头、段信息等。

在我们的框架下，链接器需要创建一个 `.type == ".exe"` 的 `FLEObject`。其 `.name` 字段可设置为任意字符串（如 `"a.out"`）；`.phdrs` 字段需要设置为一个正确的 `ProgramHeader` 结构，以便加载器能够正确地加载程序。

> [!NOTE]
> 加载器（loader）是操作系统的一部分，负责把可执行文件加载到内存（比如分配空间、建立映射关系），并初始化程序的执行环境（比如初始化栈空间、跳转到程序入口点）。
> 
> 我们实现了一个用户态的加载器，模拟操作系统加载器的行为。通过 `./exec` 命令，你可以加载运行一个 FLE 格式的可执行文件。
> 
> 你将在下学期 ICS 2 课程中学到有关加载器的更多知识。

> [!TIP]
> 上面提到，我们在这个任务中可以把所有节一股脑地扔进 `.load` 段里。我们需要添加一个程序头来描述这个段，你可以直接参考下面这段代码：
> ```cpp
> ProgramHeader header = {
>     .name = ".load", // 描述的是 .load 段
>     .vaddr = 0x400000, // 我们使用固定的加载地址 0x400000
>     .size = size, // 合并后的总大小
>     .flags = PHF::R | PHF::W | PHF::X // 可读、可写、可执行，简单地赋予所有权限
> };
> ```

此外，可执行文件必须指定一个入口点（entry point），也就是程序开始执行的位置。
按惯例，在基于 C 语言开发的程序中，这个入口点通常是名为 `_start` 的函数。链接器需要在合并的全局符号表中找到 `_start` 符号，并将其地址（基地址 + 符号偏移）设置为程序的入口点。

在完成这些工作后，我们就得到了一个完整的 `FLEObject` 结构体，为加载器描述了整个程序理应达成的内存布局。

`FLE_ld` 函数只需返回这个结构体，我们的实验框架会将其序列化为 FLE 格式文件：

```json5
{
    "type": ".exe",             // 表明这是一个可执行文件
    "phdrs": [
        {
            "name": ".load",    // 程序头
            "vaddr": 4194304,   // 固定的加载地址 0x400000
            "size": 46,         // 合并后的总大小
            "flags": 7          // 可读、可写、可执行
        },
    ],
    "entry": 4194304,           // 程序的入口点 0x400000 的十进制表示
    ".load": [                  // 合并后的节内容，只包含机器码
        "🔢: 31 c0 31 d2 0f be 88 20 00 40 00 48 ff c0 01 ca",
        "🔢: 48 83 f8 0a 75 ee 89 d7 b8 3c 00 00 00 0f 05 c3",
        "🔢: 48 65 6c 6c 6f 2c 20 57 6f 72 6c 64 21 00"
    ]
}

```

这样我们就得到了一个完整的可执行文件。

> [!TIP]
> 在实现链接器的过程中，一种可行的思路是采用多遍扫描的方法：
> 1. 第一次扫描：合并所有节的内容
> 2. 第二次扫描：收集并解析所有符号
> 3. 第三次扫描：从上到下依次处理所有重定位项，将计算出的地址回填
> 
> 最后生成程序头，并设置入口点，得到最终的 `FLEObject`。
> 
> 可以思考：为什么要采取这样的扫描顺序？有没有其他更好的方法？

请在 [`src/student/ld.cpp`](/src/student/ld.cpp) 中实现你的链接器。

> [!TIP]
> Start simple, move fast!
>
> 在实现过程中，你可以先处理只有一个输入文件的简单情形。使用 `readfle` 工具检查输出文件的格式是否正确并打印调试信息（比如节的合并过程、符号的新地址、重定位的处理过程）也会对调试很有帮助。参见[调试指南](https://github.com/RUCICS/LinkLab-2024-Assignment/wiki/%E8%B0%83%E8%AF%95%E6%8C%87%E5%8D%97#%E5%9F%BA%E7%A1%80%E5%B7%A5%E5%85%B7)。

完成后，你的链接器就能处理基本的链接任务了。运行测试来验证：

```bash
make test_2
```

## 任务三：实现相对重定位

在任务二中，我们处理了绝对重定位——访问全局变量时使用绝对地址所需要的重定位。函数调用则有所不同，正如我们在汇编一章所学到的，对应汇编级别的相对跳转，在链接时则需要做**相对重定位**。

让我们看一个例子：
```c
// lib.c
int add(int x) {    // 一个简单的函数
    return x + 1;
}

// main.c
extern int add(int);  // 声明：add 函数在别处定义

int main() {
    return add(41);   // 调用 add 函数
}
```

编译器会将这些源文件编译成目标文件。这个函数调用在我们的 FLE 格式中，表示为:

```json5
{
    "type": ".obj",
    ".text": [
        "📤: main 20 0",
        "🔢: f3 0f 1e fa 55 48 89 e5 bf 29 00 00 00 e8",
        "❓: .rel(add - 4)",  // 链接器需要将这里替换成 call 指令的目标地址（四字节）
        "🔢: 5d c3"
    ]
}
```

> [!TIP]
> x86-64 的 call 指令格式是：
> - 一个字节的操作码（`0xE8`）
> - 四个字节的相对偏移量（小端序）

对应的 `Relocation` 结构体：

```cpp
Relocation {
    .type = RelocationType::R_X86_64_PC32,  // 32位相对寻址
    .offset = 14,                           // 重定位位置（在节内的偏移）
    .symbol = "add",                        // 目标符号
    .addend = -4                            // 偏移量调整值
}
```

还记得 [FLE 格式文件](#任务零理解目标文件格式)的约定吗？这里 `.rel(add - 4)` 表示一个相对重定位（`R_X86_64_PC32` 类型），其目标符号是 `add`，附加值为 `-4`。与任务二中的绝对重定位不同，这里存储的不是函数的绝对地址，而是「要跳多远」。这种设计有两个重要的优点：
1. 代码可以加载到内存的任何位置（天然支持位置无关，而不需要动态链接器运行时修正地址）
2. 跳转指令更短（代码体积只要不超过 4GB，就只需要 32 位偏移量）

计算公式非常简单：

$$
\text{偏移量} = S + A - P
$$

其中：
- S (Symbol)：目标符号的地址（比如 `add` 函数的位置）
- A (Addend)：重定位项中的附加值（这就是 `- 4` 派上用场的地方！）
- P (Place)：重定位位置的地址（`call` 指令操作数的位置）

比如，如果：
- `add` 函数在 `0x4000A0`
- `call` 指令的操作数在 `0x400035`
- 附加值是 `-4`

那么：

$$
\text{偏移量} = \texttt{0x4000A0} + (-4) - \texttt{0x400035} = \texttt{0x67}
$$

最终的机器码就是：
```
e8 67 00 00 00   ; call add
```

> [!NOTE]
> 为什么需要 `addend`（附加值）？这与 x86-64 的程序计数器 `%rip` 的工作方式有关：
> ```
>   X:  E8          ; call rel32 的操作码 (1 Byte)
> X+1:  ?? ?? ?? ?? ; 32 位偏移量 (4 Bytes)
> X+5:  ?? ?? ?? ?? ; 下一条指令
> ```
> 1. `call` 指令中的偏移量是相对于程序计数器 `%rip` 的，而在当前指令执行时，`%rip` 已经指向了下一条指令，也就是 $X+5$
> 2. 所以当 CPU 执行到这个 `call` 指令时，实际需要跳转的距离是 $\text{偏移量} = S - (X+5)$
> 3. 链接时，重定位位置的地址是 $P = X+1$，因此，如果公式为 $\text{偏移量} = S + A - P = S + A - (X+1)$，从而得到 $A = -4$，即 `addend` 为 `-4`。
> 
> 简单来说，就是重定位位置的地址，和下一条指令的地址差为 `-4`。
>
> 你将在下下学期的 计算机系统实现 1 中学习到更多关于 CPU 流水线与指令执行的知识。

请在你实现的链接器基础上，进一步支持相对重定位。

> [!TIP]
> 调试相对重定位时，打印每一步的基本信息可能会有所帮助：
> - 目标符号的地址（S）
> - 重定位位置的地址（P）
> - 计算得到的偏移量
> - 最终写入的字节

完成后，运行测试来验证：

```bash
make test_3
```

> [!NOTE]
> 我们在这里解释上一节中，为什么入口点总是名为 `_start` 的函数。
> 
> C 与 C++ 编译器在链接时会默认将 `libc`，即 C 语言标准库链接进来，其最常见的实现是 `glibc`。
> 
> 这个库会定义一个 `_start` 函数，作为程序的低级入口点。这个函数负责从栈上解析命令行参数、初始化 C++ 全局对象、初始化堆内存管理等，并进一步调用 `main` 函数，转移控制权。
>
> 其他语言，如 Go、Rust 等，亦有类似的机制。
>
> 从任务三开始，我们开始包含 [`tests/common/minilibc.h`](./tests/common/minilibc.h)，这个库模拟了 `libc` 的部分功能，提供预定义的 `_start` 函数。

## 任务四：处理符号冲突和局部符号

在前面的任务中，我们假设每个符号只在一个地方定义。但在实际编程中，经常会遇到**同名符号**。让我们看一个例子：

```c
// config.c
int debug_level = 0;  // 默认配置

// main.c
int debug_level = 1;  // 自定义配置
```

这时链接器该怎么办？应该用哪个 `debug_level`？

除了处理同名符号的问题，链接器还需要正确处理符号的可见性。在 C 语言中，我们可以用 `static` 关键字将符号声明为局部的：

```c
// foo.c
static int counter = 0;  // 局部符号，只在本文件可见

// main.c
extern int counter;      // 错误：找不到符号 counter
```

这种情况应该在链接时被发现并报错。因为对于 `main.c` 来说，`foo.c` 中的局部符号 `counter` 相当于未定义符号 —— 它根本就「看不到」这个符号。

为了解决符号冲突和可见性问题，C 语言编译器定义了全局符号的「强度」：

- 强符号（Strong）：普通的全局符号
- 弱符号（Weak）：可以被覆盖的备选项

最常见的用法是，用弱符号来提供可覆盖的默认实现：

```c
// logger.c
__attribute__((weak))        // 标记为弱符号
void init_logger() {         // 默认的初始化函数
    // 使用默认配置
}

// main.c
void init_logger() {         // 强符号会覆盖默认实现
    // 使用自定义配置
}
```

在 FLE 格式中，弱符号用 📎 表示：

```json5
{
    "type": ".obj",
    ".text": [
        "📎: init_logger 20 0",  // 弱符号
        "🔢: 55 48 89 e5 ..."
    ]
}
```

链接时，符号冲突的处理规则很简单：

1. 强符号必须唯一，否则报错
    ```c
    // a.c
    int max(int x, int y) { return x > y ? x : y; }

    // b.c
    int max(int x, int y) { return x > y ? x : y; }  // 错误：重复定义！
    ```

2. 强符号优先于弱符号
    ```c
    // lib.c
    __attribute__((weak))
    void init() { /* 默认实现 */ }

    // main.c
    void init() { /* 自定义实现，会覆盖默认版本 */ }
    ```

3. 多个弱符号时任意选择
    ```c
    // a.c
    __attribute__((weak))
    int debug = 0;

    // b.c
    __attribute__((weak))
    int debug = 1;
    ```

> [!NOTE]
> 在 C++ 中，`inline` 函数或变量通常被编译器实现为 [Vague Linkage](https://gcc.gnu.org/onlinedocs/gcc/Vague-Linkage.html)，行为有点类似于弱符号。模板、定义在类内的成员函数、`constexpr` 变量是隐式 `inline` 的，也会以相同方式实现。
>
> 思考一下，这都是为什么？注意 C++ 的 `inline` 修饰符的含义。

请让你的链接器支持这些规则，并在规则被违反时输出合理的错误信息：
- 对于强符号冲突的情形，应当输出 `"Multiple definition of strong symbol: ${sym.name}`
- 对于找不到符号定义的情形，应当输出 `"Undefined symbol: ${sym.name}`。

> [!TIP]
> 测试脚本采用较为宽松的匹配方式，只要检测到 `stderr` 中的某一行包含符合格式的字符串即认为测试通过。你可以：
> - 通过 `throw std::runtime_error(err_message)` 抛出异常，实验框架会自动捕获并输出错误信息到标准错误流
> - 直接向 `stderr` 输出错误信息（使用 `std::cerr` 或 `fprintf`），然后调用 `exit(1)` 中止程序

> [!IMPORTANT]
> 在实际代码中，除了显式声明的同名局部符号之外，你还会遇到一些特殊的同名局部符号情况。例如，对不同文件中的字符串字面量，编译器可能生成同名的局部符号（比如 `.LC0`）。
> 
> 不同目标文件的同名局部符号是被允许的，因为每个局部符号仅在当前目标文件中可见。

> [!TIP]
> 为了处理这些情况，你需要想办法将来自不同文件的同名局部符号区分开来。你可以在符号解析时在局部符号名前添加目标名字（`FLEObject::name`）作为前缀，并在重定位时使用包含前缀的完整符号名进行重定位查找。调试时，你也可以用完整的符号名来定位问题。

完成后，你的链接器就能正确处理符号冲突和局部符号了。运行测试来验证：

```bash
make test_4
```

## 任务五：处理 64 位地址

到目前为止，我们处理的都是 32 位的地址（`R_X86_64_32` 和 `R_X86_64_PC32`）。但在 64 位系统中，有时我们需要完整的 64 位地址。比如：

```c
// 一个全局数组
int numbers[] = {1, 2, 3, 4};

// 一个指向这个数组的指针
int *ptr = &numbers[0];  // 需要完整的 64 位地址！
```

在链接时，我们需要知道 `numbers` 的地址，并写入数据段作为 `ptr` 符号的值。x86-64 系统上，`int *` 类型是 64 位的，因为其指向的地址是 64 位的。

这种情况下，编译器会生成一个新的重定位类型（`R_X86_64_64`）：

```json5
{
    "type": ".obj",
    ".data": [
        "📤: numbers 16", // numbers 数组
        "🔢: 01 00 00 00", // 1
        "🔢: 02 00 00 00", // 2
        "🔢: 03 00 00 00", // 3
        "🔢: 04 00 00 00"  // 4
    ],
    ".data.rel.local": [
        "📤: ptr 8",
        "❓: .abs64(numbers + 0)" // 需要 numbers 的完整地址
    ]
    ...
}
```

注意那个 `.abs64` —— 这是一个新的重定位类型（`R_X86_64_64`），它告诉链接器：在这里填入符号的完整 64 位地址。

对应的 `Relocation` 结构体如下：

```cpp
Relocation {
    .type = RelocationType::R_X86_64_64,  // 64位绝对寻址
    .offset = 0,                          // 重定位位置
    .symbol = "numbers",                  // 目标符号
    .addend = 0                          // 偏移量
}
```

你的任务是支持这种重定位。处理这种重定位时，计算公式与 [32 位绝对寻址](#重定位) 相同，但写入时需要写入完整的 8 字节。

> [!TIP]
> 使用 64 位整数存储地址，小心整数溢出。

完成后，你的链接器就能处理 64 位地址了。运行测试来验证：

```bash
make test_5
```

## 任务六：分离代码和数据

到目前为止，我们把所有程序的所有数据都放在一个段中。这看起来很方便，但正如我们在 Attack Homework 中所学到的那样，这给了攻击者篡改代码的机会。比如：

```c
void hack() {
    // 1. 修改代码
    void* code = (void*)hack;
    *(char*)code = 0x90;     // 如果代码段可写，程序可以被篡改！

    // 2. 执行数据
    char shellcode[] = {...}; // 一些恶意代码
    ((void(*)())shellcode)(); // 如果数据段可执行，这就是漏洞！
}
```

为了防止这些攻击，现代系统采用内存分段保护机制。在 FLE 格式中，我们的编译器已经将代码划分为了不同的节：

```json5
{
    "type": ".obj",
    "shdrs": [
        {
            "name": ".text",
            "type": 1,
            "flags": 1,
            "addr": 0,
            "offset": 0,
            "size": 26
        },
        {
            "name": ".data",
            "type": 1,
            "flags": 1,
            "addr": 0,
            "offset": 0,
            "size": 4
        },
        {
            "name": ".bss",
            "type": 8,
            "flags": 9,
            "addr": 0,
            "offset": 0,
            "size": 4096
        },
        {
            "name": ".rodata",
            "type": 1,
            "flags": 1,
            "addr": 0,
            "offset": 0,
            "size": 6
        }
    ],
    ".text": [
        "📤: main 26 0",
        "🔢: 55 48 89 e5 bf",
        "❓: .abs(.rodata + 0)",
        "🔢: b8 00 00 00 00 e8",
        "❓: .rel(printf - 4)",
        "🔢: b8 00 00 00 00 5d c3"
    ],
    ".data": [
        "📤: counter 4 0",
        "🔢: 03 00 00 00"   // 已初始化数据
    ],
    ".bss": [
        "📤: buffer 4096 0" // 未初始化数据，大小为 4096
    ],
    ".rodata": [
        "🏷️: .rodata 0 0",
        "🔢: 48 65 6c 6c 6f 00" // "Hello"
    ]
}

```

节头（section headers）是目标文件中的重要元数据，它描述了每个节的属性和位置信息。在我们的 FLE 格式中，每个节头包含：

- `name`：节的名称（如 `.text`、`.data` 等）
- `type`：节的类型（如代码、数据等）
- `flags`：节的属性标志
- `addr`：节在内存中的起始地址
- `offset`：节在文件中的偏移量
- `size`：节的大小

这些信息对链接器来说非常重要——它们描述了目标文件中各个节的组织方式。链接器需要使用这些信息来定位和读取各个节的内容，合并来自不同文件的同类型节，并在生成可执行文件时确定最终的内存布局。

> [!NOTE]
> Linux 的标准 ELF 格式中存在两种重要的视图:
>
> - **链接视图（Linking View）** 由节头（Section Headers）表描述，包含链接时需要的详细信息，如 `.text`、`.data`、`.bss` 等节。这些信息主要用于链接和调试，在最终的可执行文件中并非必需。
> - **运行视图（Execution View）** 由程序头（Program Headers）表描述，定义了程序运行时的内存段（segment）。它描述如何将文件映射到内存，是加载程序（loader）必需的信息，在可执行文件中不可或缺。
>
> 在标准的 ELF 中，一个内存段通常会包含多个具有相同权限需求的节。例如，可读写的数据段通常同时包含 `.data` 和 `.bss` 节，它们会被映射到同一块连续的内存区域中。这种设计可以减少内存碎片，简化程序的加载过程。
>
> 为了让同学们专注于理解链接的基本概念，我们的 FLE 格式采用了简化的设计，始终保持节和段的一对一映射，将每个节都单独映射为一个内存段。这种简化虽然不够优化，但更容易理解和实现。

这些节需要被映射到内存中，具体类型及含义如下：

| 节 | 作用 | 权限
| ---- | ---- | ---- |
| `.text` 节 | 存放程序代码 | `r-x` （读、执行）|
| `.rodata` 节 | 存放常量数据 | `r--`（只读）|
| `.data` 节 | 存放已初始化的全局变量 | `rw-`（读/写）|
| `.bss` 节 | 存放未初始化的全局变量 | `rw-`（读/写）|

你的任务是支持节的内存映射，在程序头中为每个节设置相应的权限级别，并保证每个节的起始地址为 4KB 对齐。此外，你还可以合理安排节的顺序以减少内存碎片。

> [!TIP]
> 我们使用枚举 `enum class PHF` 来定义程序头中的段权限，用二进制值（如 `1`, `2`, `4`）表示 `R`（可读）、`W`（可写）、`X`（可执行）。这些标志可按位或操作组合，例如 `PHF::R | PHF::X` 表示该段既可读又可执行，其中 `PHF::` 表示作用域，是必须的。

> [!TIP]
> 在实际编译生成的目标文件中，你可能会遇到一些特殊命名的节，如 `.text.startup`、`.rodata.str1.1` 等。这些是编译器为了优化目的自动生成的：
> - `.text.startup`：存放程序初始化相关代码（如 `main` 函数）
> - `.text.unlikely/.text.hot`：用于基于概率的代码布局优化
> - `.rodata.str1.1`：存放字符串常量，数字表示对齐要求
>
> 处理这些节时，你可以
> 1. 保持独立，参照对应的标准节设置权限
>   - `.text.*` 设置为 `r-x`（读/执行）
>   - `.rodata.*` 设置为 `r--`（只读）
>   - `.data.*` 设置为 `rw-`（读/写）
>   - `.bss.*` 设置为 `rw-`（读/写）
> 2. 将它们合并到对应的标准节中（相对复杂）
>   - `.text.*` 合并到 `.text`
>   - `.rodata.*` 合并到 `.rodata`
>   - `.data.*` 合并到 `.data`
>   - `.bss.*` 合并到 `.bss`
>
> 合并这些节可以减少内存碎片，简化程序的内存布局，这也是标准链接器的常见做法。

> [!IMPORTANT]
> `.bss` 节相对特殊。由于它专门用于存放零初始化的全局变量（全局变量的默认初始化即为零初始化），为节省空间，目标文件只在节头（`shdrs`）记录了这片空间的大小，而不会实际存储一堆 `0`。
>
> 在可执行文件中，出于相同原因，`.bss` 段的一堆 `0` 依然不应该被实际存储。
>
> 加载器会自动将 `.bss` 段映射为一块初始化为 `0` 的对应的内存区域。

> [!TIP]
> 在 `.bss` 节定义的符号，其 `size` 属性尤其重要——因为这是获取相关内存布局的唯一办法。

完成后，你的链接器就能生成具有正确内存布局和访问权限的可执行文件了。运行测试来验证：

```bash
make test_6
```

## 任务七：验证与总结

恭喜你完成了所有基础任务！现在让我们验证整个链接器的功能。

### 完整测试

```bash
make test
```

你应该看到：

```
Preparing FLE Tools...
✓ FLE Tools compiled successfully
Preparing minilibc...
✓ minilibc compiled successfully

Running 12 test cases...

✓ nm Tool Test [2/2]: Passed
✓ Single File Test [3/3]: Passed
✓ Absolute Addressing Test [3/3]: Passed
✓ Absolute + Relative Addressing Test [4/4]: Passed
✓ Position Independent Executable Test [5/5]: Passed
✓ Strong Symbol Conflict Test [3/3]: Passed
✓ Weak Symbol Override Test [4/4]: Passed
✓ Multiple Weak Symbol Warning [5/5]: Passed
✓ Local Symbol Access Test [7/7]: Passed
✓ 64-bit Absolute Relocation Test [3/3]: Passed
✓ BSS Section Linking Test [5/5]: Passed
✓ Section Permission Control Test [10/10]: Passed
┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━┳━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━┓
┃ Test Case                           ┃ Result ┃  Time ┃     Score ┃ Message             ┃
┡━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╇━━━━━━━━╇━━━━━━━╇━━━━━━━━━━━╇━━━━━━━━━━━━━━━━━━━━━┩
│ nm Tool Test                        │  PASS  │ 0.36s │ 10.0/10.0 │ All steps completed │
│ Single File Test                    │  PASS  │ 0.27s │ 10.0/10.0 │ All steps completed │
│ Absolute Addressing Test            │  PASS  │ 0.29s │ 10.0/10.0 │ All steps completed │
│ Absolute + Relative Addressing Test │  PASS  │ 0.60s │ 10.0/10.0 │ All steps completed │
│ Position Independent Executable     │  PASS  │ 0.93s │ 10.0/10.0 │ All steps completed │
│ Test                                │        │       │           │                     │
│ Strong Symbol Conflict Test         │  PASS  │ 0.70s │ 10.0/10.0 │ All steps completed │
│ Weak Symbol Override Test           │  PASS  │ 0.78s │ 10.0/10.0 │ All steps completed │
│ Multiple Weak Symbol Warning        │  PASS  │ 0.85s │ 10.0/10.0 │ All steps completed │
│ Local Symbol Access Test            │  PASS  │ 0.46s │ 10.0/10.0 │ All steps completed │
│ 64-bit Absolute Relocation Test     │  PASS  │ 0.37s │ 10.0/10.0 │ All steps completed │
│ BSS Section Linking Test            │  PASS  │ 0.77s │ 10.0/10.0 │ All steps completed │
│ Section Permission Control Test     │  PASS  │ 1.27s │ 10.0/10.0 │ All steps completed │
└─────────────────────────────────────┴────────┴───────┴───────────┴─────────────────────┘

╭────────────────────────────────────────────────────────────────────────────────────────╮
│ Total Score: 120.0/120.0 (100.0%)                                                      │
╰────────────────────────────────────────────────────────────────────────────────────────╯

```

### 实验报告要求

请参考[报告模板通用指南](./report/README.md)，基于[实验报告模板](./report/report.md)，完成实验报告。

## 完成本实验

### 提交

使用 GitHub Classroom 进行提交。请你确保所有代码已提交到你的对应仓库，GitHub Actions 会自动运行测试，其输出作为我们的评分依据。

### 评分标准

分数由**正确性评分**和**质量评分**两部分组成，正确性评分占 60%，质量评分占 40%，即：

$$
\text{总分} = \frac{\text{正确性评分}} {\text{正确性满分}} \times 60\% + \text{质量评分} \times 40\%
$$

其中，正确性评分由 GitHub Classroom 自动计算，质量评分由助教根据代码风格和实验报告综合评估，具体衡量因素为：

- 代码风格
  - 代码表达能力强，逻辑清晰，代码简洁，**仅在必要时**添加注释
  - 防御性编程，进行多层次的错误检查
  - 如果你使用 C++ 风格进行编码，请积极使用 C++ 标准库，而非重复造轮子
- 实验报告
  - 实验报告内容完整、格式规范、结构清晰
  - 实验报告内容与代码一致，无明显矛盾，体现出对实验考察知识的基本了解
  - 有思考，有总结，有反思，有创新
  - 对实验提供有价值的反馈和建议，积极参与开源协作

## 调试建议

你可以阅读[调试指南](https://github.com/RUCICS/LinkLab-2024-Assignment/wiki/%E8%B0%83%E8%AF%95%E6%8C%87%E5%8D%97)，了解如何使用评测脚本和 VSCode 调试。

## 进阶内容

> [!TIP]
> 前面的区域以后再来探索吧！

在完成基础任务的旅程之后，更广阔的天地正等待我们去探索。现代链接器早已超越了简单的代码组合，它需要在性能、兼容性、安全性等多个维度上进行取舍（trade-off），以满足复杂软件系统的需求。

以下是一些值得探索的方向，学有余力的同学可以尝试实现：

1. 静态库支持：通过设计一种归档格式（`.a` 文件）来打包多个目标文件，你的链接器可以实现按需链接——只提取程序实际使用的符号所在的目标文件。这不仅节省了磁盘空间，更重要的是避免了不必要的代码段被加载到内存。

2. 动态链接支持：它允许多个进程共享同一份代码，显著降低内存占用。要支持动态链接，你需要生成位置无关可执行文件（PIE），实现全局偏移表（GOT）处理数据引用，以及通过过程链接表（PLT）实现延迟符号绑定。此外，动态链接器还需要处理复杂的符号解析规则和加载时重定位。

3. 符号版本管理：对于维护长期支持的共享库至关重要。通过版本脚本，你可以为每个导出符号指定版本信息，实现多版本共存。这使得库可以在增加新功能的同时保持对旧版本 ABI 的兼容，避免「共享库依赖地狱」。你需要设计版本定义的语法，并在符号解析时正确处理版本信息。

4. 增量链接：着眼于提升开发效率。通过只重新链接发生变化的部分，可以大幅缩短构建时间。这需要你设计一个依赖图来追踪文件间的关系，并能够准确判断符号级别的变化。你还需要考虑如何处理调试信息的增量更新。

5. 调试支持：工程实践中不可或缺的功能。除了保留基本的符号表，完整的调试支持还需要处理 DWARF 格式的调试信息，包括行号表、变量描述、类型信息等。这些信息需要在链接过程中被正确重定位和合并。此外，为了让开发者能够使用 GDB 等标准调试工具，你还需要编写 GDB 脚本来支持 FLE 格式文件的解析，使调试器能够正确显示源码位置、变量值和调用栈等信息。

6. 安全增强：通过支持地址空间随机化（ASLR）和生成位置无关可执行文件（PIE），可以有效防范缓冲区溢出和代码注入攻击。此外，你还可以支持 RELRO（RELocation Read-Only）机制，将 GOT 表设为只读来防止 GOT 表覆写攻击。

这些探索不会被计入评分，但它们承载着更深远的意义。如果你愿意，可以通过 Pull Request 分享你的实现和对实验框架的改进。你的贡献将如涟漪般荡开，让更多追寻技术之美的同学从中受益。

## Acknowledgements

本实验由 22 级图灵班的李知非同学和彭文博同学共同提出并设计完成。在实验设计过程中，21 级图灵班的潘俊达同学为实验文档的编写提供了诸多建设性的建议，在此表示衷心的感谢。

特别感谢柴云鹏教授和王晶教授对本实验的悉心指导与大力支持。他们的专业见解和宝贵建议对实验的完善起到了重要的推动作用。

## 参考资料

1. [可执行文件和加载 - 操作系统（2025春）- jyy](https://jyywiki.cn/OS/2024/lect19.md)
2. [I Executable and Linkable Format (ELF)](https://www.cs.cmu.edu/afs/cs/academic/class/15213-f00/docs/elf.pdf)
3. [CSAPP: Computer Systems A Programmer's Perspective](https://csapp.cs.cmu.edu/)
4. [System V ABI](https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf)
5. [Linkers & Loaders](https://linker.iecc.com/)
6. [How To Write Shared Libraries](https://www.akkadia.org/drepper/dsohowto.pdf)
