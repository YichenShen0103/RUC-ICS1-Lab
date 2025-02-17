# LinkLab 报告

姓名：xxx
学号：2023202128

## Part A: 思路简述

链接器的设计主要分为六个部分，具体每一个步骤所完成的操作如下：

- step 0：为了进行各部分间通信，定义了如下基本的中间存储结构，各个结构存储的内容如注释所示。以下在具体实现板块中将具体介绍各个结构的用途。

```c++
unordered_map<string, pair<uint64_t, uint64_t>> SecInfo; // start address, size
unordered_set<string> undefineSymbols; // symbol name of undefined
unordered_map<sstring, pair<SymbolType, uint64_t>> defineSymbols; // size, addr
unordered_map<string, unordered_set<string>> localSymbols; // file name, symbol name
unordered_set<string> nameOfSec; // name of sections in each .obj file
vector<pair<int, uint64_t>> outputSecInfo(4, { 0, 0 }); // secClass, size
vector<uint64_t> outputSecHead(4); // start address
vector<uint64_t> outputSecTail(4); // end address
```

- step 1：**预处理步骤**，**源文件节信息登记**和**可执行文件的内存规划**

- step 2：模拟内存加载**读入机器码**，计算各个源文件节在最终可执行文件的偏移量

- step 3：依据上一步的修正偏移量处理**符号偏移量修正**和**符号冲突**

- step 4：处理**重定位**
- step 5：**收尾操作**，生成返回的可执行文件



## Part B: 具体实现分析

### 符号解析实现
遍历所有待链接的文件，对于每个文件遍历它的符号表，以下将介绍对于一个特定的符号如何进行解析：

如果这个符号是UNDEFINED类型，在一个set（undefineSymbols）中记录这个符号的名字，先不报错；

如果这个符号是LOCAL类型，那么如果这个符号在全局强符号已有定义或者这个文件中这个局部符号已经定义过了，那么会报错重定义；否则更新一个set（locals）将局部变量存入；

其他情况下，**为了简化程序，当我们没有证据证明这个符号有冲突时，必须推定这个符号是没有冲突的**，即需要写入（覆盖）定义，这样我们只要考虑特殊情况即可，即这个符号在locals或者defineSymbols中已经定义过了。在这种情况下，不同符号需要不同的特殊处理：

- 如果这个符号是弱符号，判断已定义过的这个符号是不是也是弱符号，如果是，那么用随机函数进行一次选择要不要跳过这一轮循环，否则直接跳过这一轮遍历。
- 如果这个符号是强符号，判断已定义过的符号是不是也是强符号，如果是，报重定义错，否则它符合我们的推定，可以执行写入。



最后对undefineSymbols进行一轮遍历，看看在defineSymbols中有没有符号存在，如果没有报未定义错。

> ⚠ 注意
>
> 此处有一个关键的逻辑，即undefineSymbols中的符号有没有可能在locals中定义？答案是不可能，因为编译器不会允许一个局部变量先引用在定义，这样的obj文件不可能产生。

以下是处理符号（step 3）的伪代码：

```c++
For each obj in objects:
	Initialize set locals
	For each s in obj.symbols:
		if s is UNDEFINED: undefineSymbols.insert(s.name) // Lazy evaluation
        else if s is LOCAL:
			// Update offset...
			if find s in locals or (defineSymbols and is not WEAK): throw runtime_error "Redefinition"
            else: locals.insert(s.name)
            update defineSymbols storing type and address
        else:
			// Update offset...
			if find s in locals or defineSymbols:
				if s is WEAK:
					if find s in defineSymbols and is also WEAK: // random choice continue or not...
					else: continue
             	else:
					if (find s in locals or (defineSymbols and is not WEAK): 
                        throw runtime_error "Multiple definition"
			update defineSymbols storing type and address
	// store [filename, locals] to a hash table localSymbols                       
// Is there any undefined symbols?
For each s in undefineSymbols:
	if find s in defineSymbols: throw runtime_error "Undefined"
```



### 重定位处理
遍历所有待链接的文件，对于每个文件遍历它的所有节再遍历每个节的重定义条目，以下将介绍对于一个特定的条目如何进行处理：

我们首先推定不是相对地址重定位，根据公式（define address + addend）算出一个addr，接下来分情况讨论

如果条目的重定义类型是绝对地址重定义，这种情况下有两种类型：

- 32位，先判断符不符合类型（有符号，无符号）的基本要求，不符合则报错“错误的类型”，然后对地址取低32位，并检测新地址是不是合法，不合法则报错“地址不允许访问”
- 64位，不做处理，直接判断地址是不是合法

如果条目是相对地址重定位，那么再根据公式减去一个call指令地址求出偏移量。

最后计算需要填入重定位地址的位置，依照32位或64位进行小端填入。

这一部分（step 4）的伪代码实现如下：

```c++
For each obj in objects:
	Load locals from localSymbols[filename]
	For each sections in obj:
		For each reloc in sections.relocs:
			rtype <- reloctype
            addr <- define addr + addend
            if rtype is absoluteReloc:
				if rtype is unsigned 32bits:
					if higher 32-bits of addr is not 0: throw runtime_error "Wrong type"
                else if rtype is signed 32bits:
					if higher 32-bits of addr is not same as 31-bit: throw runtime_error "Wrong type"
                if rtype is 32bits: addr &= 0xffffffff
                if not 0x400000 <= addr <= MaxAddr: throw runtime_error "Permission denied"
			else: addr <- addr - (SecAddr + reloc.offset)
            addrToChange <- reloc.offset + SecAddr - 0x400000;
			// Change bits in vector machine_code...
```



### 段合并策略
段合并的核心涉及step 1、step 2和step 5，首先进行内存规划，然后模拟加载可执行文件后内存的排布并进行相应分割，并没有专门的步骤却很自然地实现了这一部分。下面是具体实现：

首先，在step 1（预处理阶段）我们先不急于查看各个节里有什么内容，我们先遍历一遍所有的obj文件中的节头表，并对各个节进行**分类**，也就是最终它们在可执行文件中会被归并到哪个节中，并通过累计计算得到每个最终节的大小。为了让**内存碎片最小化，我们希望需要填充最多0的一个节放在内存的最后**，也就是（节大小 % 4096）最小的放在最后。由此我们就得到了内存规划。

然后，在step 2中，我们依照我们的规划顺序将obj文件中的内容读入，遍历4次所有文件，每次只关注一个最终节，如果当前节不会归类到我们关注的最终节，那么就跳过，如此就实现了合并；用machine_code来模拟加载时内存的排布，每当一个最终节开始读取时，存入一个首地址，读取完了存入一个尾地址，并填充0直到（当前地址 % 4096 == 0）来实现对齐。

在step 5的过程中，只需根据我们的首位地址对machine_code进行切片操作即可。

以下是实现段合并关键步骤的伪代码实现。

```c++
// step 1
For each obj in objects:
	For each shdr in obj.shdrs：
        Load shdr.size into SecInfo[filename + " " + shdr.name]
        nameOfSec.insert(shdr.name)
        classOfSec <- get_class(shdr.name) // Using string matching in STL, maping section classes into 0~3
        update size in outputSecInfo[classOfSec]
sort(outputSecInfo.begin(), outputSecInfo.end(), [&](pair<int, uint64_t>& a, pair<int, uint64_t>& b) {
    return a.second % 4096 > b.second % 4096; // "second" storing the size of sections 
})

// step 2
curAddr <- 0x400000
For each pair in outputSecInfo:
	selectSec <- pair.first // which section we focus on currently
    outputSecHead[selectSec] <- curAddr
    For each obj in objects:
		For each section in obj.sections:
			classOfSec <- get_class(section  name)
            if classOfSec != selectSec: continue
            update SecInfo[filename + " " + secName] with curAddr // the address of this section in exe
            curAddr <- curAddr + Section size
            For each data in section.data: machine_code.push_back(data)
            if selectSec == 3: Push 0 in machine_code simulating memory allocation
    outputSecTail[selectSec] <- curAddr // the end of the section we focus on
    While CurAddr % 4096: machine_code.push_back(0) // memory alignment

// step 5
vector<uint8_t> slicedCode
For i in range(3): // Python is the best language in the world !!!
	slicedCode.assign(
        	machine_code.begin() + outputSecHead[i] - 0x400000, 
        	machine_code.begin() + outputSecTail[i] - 0x400000
	)
	// assign slicedCode to section data...
// ... some other codes here ...
```



## Part C: 关键难点解决
1. 内存排布规划

这个部分在前文中的描述是相对简单的，但是具体实现起来会遇到很多的问题，这个难点主要集中在数据结构的设计上：

- 节名称是一个不方便作为标签的数据类型，因为这必须用哈希表存储，这意味着我们后续再进行排序以及顺序读取又需要借助其他数据结构，发挥不了哈希表的优势。同时一份数据存两次对内存有明显的浪费
- 如果使用数组，由于我们要根据大小进行排序，同时又希望通过节名称以最小的代价快速检索到某个最终节的信息，这个时候虽然能发挥顺序访问的优势，但是随机访问的访问效率又是一个挑战
- 我们在内存排布规划过程中所需要的数据信息和最终进行分割所需要的信息是不一样的，一者需要大小，一者需要首位地址

我的解决方法核心是将最终节的类型也映射到一个0~3的数字空间中即如下：

```c++
std::unordered_map<std::string, int> classify = {
    { ".text", 0 },
    { ".rodata", 1 },
    { ".data", 2 },
    { ".bss", 3 }
};
```

同时利用多个数组进行存储，不同存储结构解决不同的需求，注意到我们有以下三个数据结构：

```c++
vector<pair<int, uint64_t>> outputSecInfo(4, { 0, 0 }); // secClass, size
vector<uint64_t> outputSecHead(4); // start address
vector<uint64_t> outputSecTail(4); // end address
```

由于我们希望在模拟内存的排布时按照排序后的排名顺序访问各个节，那么我们就设计一个下标就是排名的数组，我们在排序、顺序访问时需要的数据都放到这里，而刚好在其他地方我们并不需要依据这个排名，而是依据节名称进行随机访问，这个时候将节名称映射到数字上就有优势了，我们可以利用简单的数组进行存放其他信息。

具体到使用中，就可以看到这样处理的好处，预处理阶段我们先读入节信息到outputSecInfo中，这个数组专门由于前期处理，在后期只作为顺序读取的依据，因此只要读入节序号和节大小即可。再利用标准库中的sort函数，手动设置比较标准进行排序即可保证可以通过排名作为唯一索引查找到节名称、节大小。这极大的便利了后续的模拟内存加载过程。

如此一来，各个过程都充分发挥了数组顺序访问、随机访问的优势，并且在性能上，排序、顺序访问、后续模拟过程中的顺序读取近似达到了最优。



2. 局部变量符号解析和重定位的名称冲突问题

由于局部符号可能会有名称的冲突，但实际上它们是不同的符号只是在不同而定文件中，所以如何区分不同的符号成为了一个难点，具体有以下体现：

- 如果简单地使用”文件名+符号名“的方法进行重命名，并和全局变量一起存储，这种方法在一些情况下是可行的，但是这在性能上是有所舍弃的，因为我们在进行符号解析和重定位的时候就要在全局的所有局部变量中进行查找，当复杂工程中文件较多较复杂的情况下要处理的哈希冲突就更多
- 此外，文件名加符号名也有可能出现名字的冲突，如果有一个全局变量的命名刚好和局部变量加上文件名后一样，或者两个文件名之间有子串关系就可能导致问题的出现，而一旦出现冲突，如何处理冲突就是一个非常大的问题。这个问题的根源在于局部变量的作用域和全局变量是不同的，不能通过改变名称就一劳永逸

为了解决这个问题，我们不妨回归到最接近原始模式的方法，也就是让局部变量单独存储，那如何避免不同文件之间的局部变量冲突呢？那就把不同文件的局部变量分开存储！这样既保证了作用域问题的正确，也极大提升了性能，在处理符号冲突和重定位的时候，我们只要在当前文件的局部变量表里查询即可，这极大提升了性能。这个数据结构的定义如下：

```c++
unordered_map<string, unordered_set<string>> localSymbols; // file name, symbol name
```



在处理每个文件的各种符号时，由于我们是遍历文件的方式进行的，每次遍历文件只把当前文件的局部变量载入，这样在处理这个文件的时候别的文件的局部符号是不可见的，这就从根源上解决了名称问题冲突。



## Part D: 实验反馈 (5分)
个人认为实验的设计非常好，因为链接是一个比较抽象但又很重要的内容，对于这样的内容，我认为自己手搓一个toy linker是一个非常好的达成理解的方式，既有一定的成就感，又有趣味性。

在难度方面个人认为主要难点在算法思想上比较需要一些统筹，这个难点是合理的，难度也是合理的。

至于文档方面，对于任务的介绍是非常详细的，但是我个人认为对于这个FLE格式的介绍略微有些抽象，特别是既有一个json的可视化又有各类结构体的定义要看，看完了会觉得懂了，但是实际写起程序来还要反复翻看，并且确认题目中的demo是怎么呈现的，如果可以加一个更大更全面的demo来帮助理解是更好的，比如像测试案例里一样给几个文件，他们的.cpp文件、.fle文件和**最终链接出来的可执行文件**（我觉得这个很重要，要给一个输出的实例，这样更清晰）。坦白讲我个人在开始阶段花了很长时间来理解题目所需要输出的文件和各个节，特别是任务二（那个题目的demo是不是链接过程中还有优化，所以很难理解），需要是什么格式。

实验框架我个人觉得做的很好，源码也易于阅读，可以很好的理解，如果能加一个对于fle格式翻译成汇编语言的工具会更好，以及更方便代码调试的工具会更好。

另外我觉得可以将静态库和共享库（特别是共享库的内容）放进必做当中，虽然会提升难度，但是我觉得共享库这块内容是比较抽象的，如果能手工模拟一遍会更好。


## 参考资料
本实验不必参考任何文献。
