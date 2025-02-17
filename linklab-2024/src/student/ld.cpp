#include "fle.hpp"
#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <unordered_set>
#include <vector>
std::random_device rd;
std::mt19937 gen(rd()); // 使用随机设备初始化生成器
std::uniform_int_distribution<> dis(0, 1); // 生成 0 或 1
std::unordered_map<std::string, int> classify = {
    { ".text", 0 },
    { ".rodata", 1 },
    { ".data", 2 },
    { ".bss", 3 }
};

int get_class(const std::string& str)
{
    for (const auto& [key, value] : classify) {
        if (str.find(key) != std::string::npos) {
            return value;
        }
    }
    return -1; // 如果没有找到特征子串，返回一个无效的优先级
}

FLEObject FLE_ld(const std::vector<FLEObject>& objects)
{
    FLEObject Executable;
    Executable.name = "a.out";
    Executable.type = ".exe";
    std::vector<ProgramHeader> phdrs;
    std::vector<uint8_t> machine_code;
    FLESection text;
    FLESection data;
    FLESection rodata;
    FLESection bss;
    ProgramHeader textHeader;
    ProgramHeader rodataHeader;
    ProgramHeader dataHeader;
    ProgramHeader bssHeader;
    textHeader.name = ".text";
    rodataHeader.name = ".rodata";
    dataHeader.name = ".data";
    bssHeader.name = ".bss";

    std::unordered_map<std::string, std::pair<uint64_t, uint64_t>> SecInfo; // start address, size
    std::unordered_set<std::string> undefineSymbols;
    std::unordered_map<std::string, std::pair<SymbolType, uint64_t>> defineSymbols; // size, addr
    std::unordered_map<std::string, std::unordered_set<std::string>> localSymbols; // file name, symbol name
    std::unordered_set<std::string> nameOfSec;
    std::vector<std::pair<int, uint64_t>> outputSecInfo(4, { 0, 0 }); // secClass, size
    std::vector<uint64_t> outputSecHead(4);
    std::vector<uint64_t> outputSecTail(4);

    // step 1. 读一遍确认各个节的大小，方便后续计算内存碎片的最佳规划
    for (FLEObject obj : objects) {
        std::string fname = obj.name;
        for (SectionHeader shdr : obj.shdrs) {
            SecInfo[fname + " " + shdr.name].second = shdr.size;

            nameOfSec.insert(shdr.name);
            int classOfSec = get_class(shdr.name);
            outputSecInfo[classOfSec].first = classOfSec;
            outputSecInfo[classOfSec].second += shdr.size;
        }
    }
    std::sort(outputSecInfo.begin(), outputSecInfo.end(), [&](std::pair<int, uint64_t>& a, std::pair<int, uint64_t>& b) {
        return a.second % 4096 > b.second % 4096;
    }); // 实现对于最终更少内存碎片的规划

    // step 2. read machine code in each sections in order
    uint64_t curAddr = 0x400000;
    for (auto pair : outputSecInfo) {
        int selectSec = pair.first; // 当前处理哪个最终节
        outputSecHead[selectSec] = curAddr; // 确定最终节的起始地址
        for (FLEObject obj : objects) {
            std::string fname = obj.name;
            for (auto pair : obj.sections) {
                FLESection sec = pair.second;
                std::string secName = pair.first;
                int classOfSec = get_class(secName);
                if (classOfSec != selectSec)
                    continue; // 以上主要实现每个文件中节的筛选，最终选出属于当前关注的最终节的节

                SecInfo[fname + " " + secName].first = curAddr; // 标记各个原节在最终文件的起始位置
                curAddr += SecInfo[fname + " " + secName].second; // 更新curAddr，等价于在循环中自增
                for (auto data : sec.data) {
                    machine_code.push_back(data);
                } // 循环实现读入数据
                if (selectSec == 3) {
                    for (uint64_t _ = 0; _ < SecInfo[fname + " " + secName].second; ++_)
                        machine_code.push_back(0);
                }
            }
        }
        outputSecTail[selectSec] = curAddr; // 确定当前最终节有意义的结尾
        for (; curAddr % 4096; curAddr++)
            machine_code.push_back(0); // 实现补0操作，完成4096的对齐
    }

    // step 3. Dealing with symbols including offsets and conflicts
    for (FLEObject obj : objects) {
        std::string fname = obj.name;
        std::unordered_set<std::string> locals;
        for (Symbol s : obj.symbols) {
            if (s.type == SymbolType::UNDEFINED)
                undefineSymbols.insert(s.name);
            else if (s.type == SymbolType::LOCAL) {
                s.offset = s.offset + SecInfo[fname + " " + s.section].first - 0x400000;
                if (locals.find(s.name) != locals.end() || (defineSymbols.find(s.name) != defineSymbols.end() && defineSymbols[s.name].first != SymbolType::WEAK))
                    throw std::runtime_error("Redefinition of symbol \"" + s.name + "\"!");
                else
                    locals.insert(s.name);
                defineSymbols[fname + " " + s.name].second = 0x400000 + s.offset;
                defineSymbols[fname + " " + s.name].first = s.type;
            } else {
                s.offset = s.offset + SecInfo[fname + " " + s.section].first - 0x400000;
                if (defineSymbols.find(s.name) != defineSymbols.end() || locals.find(s.name) != locals.end()) {
                    if (s.type == SymbolType::WEAK) {
                        if (defineSymbols.find(s.name) != defineSymbols.end() && defineSymbols[s.name].first == SymbolType::WEAK) {
                            bool random_bool = dis(gen);
                            if (random_bool)
                                continue;
                        } else
                            continue;
                    } else {
                        if ((defineSymbols.find(s.name) != defineSymbols.end() && defineSymbols[s.name].first != SymbolType::WEAK) || locals.find(s.name) != locals.end())
                            throw std::runtime_error("Multiple definition of strong symbol: " + s.name);
                    }
                }
                defineSymbols[s.name].second = 0x400000 + s.offset;
                defineSymbols[s.name].first = s.type;
            }
        }
        localSymbols[fname] = locals;
    }
    for (std::string sn : undefineSymbols) {
        if (defineSymbols.find(sn) == defineSymbols.end())
            throw std::runtime_error("Undefined symbol: " + sn);
    }

    // step 4. Relocation
    for (FLEObject obj : objects) {
        std::string fname = obj.name;
        std::unordered_set<std::string> locals = localSymbols[fname];
        for (auto pair : obj.sections) {
            FLESection sec = pair.second;
            std::string sn = pair.first;
            for (Relocation reloc : sec.relocs) {
                RelocationType rtype = reloc.type;
                uint64_t addr;
                if (defineSymbols.find(reloc.symbol) == defineSymbols.end())
                    addr = defineSymbols[fname + " " + reloc.symbol].second + reloc.addend;
                else
                    addr = defineSymbols[reloc.symbol].second + reloc.addend;
                if (rtype != RelocationType::R_X86_64_PC32) {
                    if (rtype == RelocationType::R_X86_64_32) {
                        if (addr ^ (addr & 0xffffffff))
                            throw std::runtime_error("Symbol " + reloc.symbol + " has wrong relocation type!");
                    } else if (rtype == RelocationType::R_X86_64_32S) {
                        if (addr >> 32 != 0 && addr >> 32 != 0xffffffff)
                            throw std::runtime_error("Symbol " + reloc.symbol + " has wrong relocation type!");
                    }
                    if (rtype != RelocationType::R_X86_64_64)
                        addr &= 0xffffffff;
                    if (addr < 0x400000 || addr > curAddr)
                        throw std::runtime_error("Symbol " + reloc.symbol + " is relocated to invalid address " + std::to_string(addr) + "!");
                } else {
                    addr = (int32_t)addr - (SecInfo[fname + " " + sn].first + reloc.offset);
                }
                uint64_t addrToChange = reloc.offset + SecInfo[fname + " " + sn].first - 0x400000;
                machine_code[addrToChange] = addr & 0xff;
                machine_code[addrToChange + 1] = (addr >> 8) & 0xff;
                machine_code[addrToChange + 2] = (addr >> 16) & 0xff;
                machine_code[addrToChange + 3] = (addr >> 24) & 0xff;
                if (rtype == RelocationType::R_X86_64_64) {
                    machine_code[addrToChange + 3] = (addr >> 32) & 0xff;
                    machine_code[addrToChange + 3] = (addr >> 40) & 0xff;
                    machine_code[addrToChange + 3] = (addr >> 48) & 0xff;
                    machine_code[addrToChange + 3] = (addr >> 56) & 0xff;
                }
            }
        }
    }

    // step 5. Dealing with output format
    std::vector<std::vector<uint8_t>> slicedCode(4);
    slicedCode[0].assign(machine_code.begin() + outputSecHead[0] - 0x400000, machine_code.begin() + outputSecTail[0] - 0x400000);
    slicedCode[1].assign(machine_code.begin() + outputSecHead[1] - 0x400000, machine_code.begin() + outputSecTail[1] - 0x400000);
    slicedCode[2].assign(machine_code.begin() + outputSecHead[2] - 0x400000, machine_code.begin() + outputSecTail[2] - 0x400000);
    slicedCode[3] = std::vector<u_int8_t>(0);

    text.data = slicedCode[0];
    rodata.data = slicedCode[1];
    data.data = slicedCode[2];

    textHeader.vaddr = outputSecHead[0];
    rodataHeader.vaddr = outputSecHead[1];
    dataHeader.vaddr = outputSecHead[2];
    bssHeader.vaddr = outputSecHead[3];
    textHeader.size = outputSecTail[0] - outputSecHead[0];
    rodataHeader.size = outputSecTail[1] - outputSecHead[1];
    dataHeader.size = outputSecTail[2] - outputSecHead[2];
    bssHeader.size = outputSecTail[3] - outputSecHead[3];
    textHeader.flags = 5;
    rodataHeader.flags = 4;
    dataHeader.flags = 6;
    bssHeader.flags = 6;
    phdrs.push_back(textHeader);
    phdrs.push_back(rodataHeader);
    phdrs.push_back(dataHeader);
    phdrs.push_back(bssHeader);

    std::map<std::string, FLESection> secmap;
    secmap[".rodata"] = rodata;
    secmap[".text"] = text;
    secmap[".data"] = data;
    secmap[".bss"] = bss;
    Executable.phdrs = phdrs;
    Executable.entry = defineSymbols["_start"].second;
    Executable.sections = secmap;

    return Executable;
}
