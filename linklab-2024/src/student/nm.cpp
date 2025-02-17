#include "fle.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

void FLE_nm(const FLEObject& obj)
{
    // TODO: 实现符号表显示工具
    // 1. 遍历所有符号
    //    - 跳过未定义符号 (section为空的情况)
    //    - 使用16进制格式显示符号地址

    // 2. 确定符号类型字符
    //    - 处理弱符号: 代码段用'W'，其他段用'V'
    //    - 根据段类型(.text/.data/.bss/.rodata)和符号类型(GLOBAL/LOCAL)确定显示字符
    //    - 全局符号用大写字母，局部符号用小写字母

    // 3. 按格式输出
    //    - [地址] [类型] [符号名]
    //    - 地址使用16位十六进制，左侧补0

    // build a hash table to store start addr of each section
    std::unordered_map<std::string, size_t> headOfSections;
    for (SectionHeader sh : obj.shdrs)
        headOfSections[sh.name] = sh.addr;

    for (Symbol s : obj.symbols) {
        if (s.section == "")
            continue; // Undefined Symbols
        std::string sec = s.section; // get section of symbol s
        size_t addr = headOfSections[sec] + s.offset; // calculate the address
        SymbolType st = s.type; // get type of symbol s
        char TypeChar;
        if (st == SymbolType::WEAK)
            TypeChar = (sec == ".text") ? 'W' : 'V'; // weak symbol
        else
            TypeChar = st == (SymbolType::GLOBAL) ? toupper(sec[1]) : sec[1]; // others
        std::cout << std::setw(16) << std::setfill('0') << std::hex << addr << " "
                  << TypeChar << " "
                  << s.name << std::endl; // output in format
    }
}