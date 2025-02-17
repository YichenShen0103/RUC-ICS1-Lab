#include "fle.hpp"
#include <iomanip>
#include <iostream>

// 辅助函数：获取最长符号名长度
size_t get_max_symbol_name_length(const std::vector<Symbol>& symbols)
{
    size_t max_len = 0;
    for (const auto& sym : symbols) {
        max_len = std::max(max_len, sym.name.length());
    }
    return max_len;
}

// 辅助函数：获取最长节名长度
size_t get_max_section_name_length(const std::vector<SectionHeader>& shdrs)
{
    size_t max_len = 0;
    for (const auto& shdr : shdrs) {
        max_len = std::max(max_len, shdr.name.length());
    }
    return max_len;
}

// 辅助函数：打印分隔线
void print_separator(size_t length)
{
    std::cout << std::string(length, '-') << std::endl;
}

// 辅助函数：格式化十六进制数输出
std::string format_hex(uint64_t value, int width)
{
    std::stringstream ss;
    ss << "0x" << std::hex << std::setfill('0') << std::setw(width) << value;
    return ss.str();
}

void FLE_readfle(const FLEObject& obj)
{
    // 打印文件类型
    std::cout << "File: " << obj.name << std::endl;
    std::cout << "Type: " << obj.type << std::endl;
    std::cout << std::endl;

    // 获取最长节名长度用于对齐
    size_t max_section_name_len = get_max_section_name_length(obj.shdrs);

    // 打印节信息
    std::cout << "Sections:" << std::endl;
    // 打印表头
    std::cout << std::setfill(' ');
    std::cout << std::left << std::setw(max_section_name_len) << "Name" << "  "
              << std::left << std::setw(10) << "Size" << "  "
              << std::left << std::setw(20) << "Flags" << "  "
              << std::left << std::setw(10) << "Addr" << "  "
              << std::left << "Offset" << std::endl;
    print_separator(max_section_name_len + 55);

    for (const auto& shdr : obj.shdrs) {
        std::cout << std::setfill(' ');
        std::cout << std::left << std::setw(max_section_name_len) << shdr.name << "  "
                  << std::left << std::setw(10) << format_hex(shdr.size, 4) << "  ";

        // 打印节标志
        std::vector<std::string> flags;
        if (shdr.flags & static_cast<uint32_t>(SHF::ALLOC))
            flags.push_back("ALLOC");
        if (shdr.flags & static_cast<uint32_t>(SHF::WRITE))
            flags.push_back("WRITE");
        if (shdr.flags & static_cast<uint32_t>(SHF::EXEC))
            flags.push_back("EXEC");
        if (shdr.flags & static_cast<uint32_t>(SHF::NOBITS))
            flags.push_back("NOBITS");

        std::string flag_str;
        for (size_t i = 0; i < flags.size(); i++) {
            flag_str += flags[i];
            if (i < flags.size() - 1)
                flag_str += "|";
        }
        std::cout << std::left << std::setw(20) << flag_str << "  "
                  << std::left << std::setw(10) << format_hex(shdr.addr, 4) << "  "
                  << std::left << format_hex(shdr.offset, 2) << std::endl;
    }
    std::cout << std::endl;

    // 获取最长符号名长度用于对齐
    size_t max_symbol_name_len = get_max_symbol_name_length(obj.symbols);

    // 打印符号表
    std::cout << "Symbols:" << std::endl;
    // 打印表头
    std::cout << std::setfill(' ');
    std::cout << std::left << std::setw(max_symbol_name_len) << "Name" << " "
              << std::left << std::setw(7) << "Type" << " "
              << std::left << std::setw(max_section_name_len) << "Section" << " "
              << std::left << std::setw(10) << "Offset" << " "
              << std::left << "Size" << std::endl;
    print_separator(max_symbol_name_len + max_section_name_len + 40);

    for (const auto& sym : obj.symbols) {
        std::cout << std::setfill(' ');
        std::cout << std::left << std::setw(max_symbol_name_len) << sym.name << " ";

        // 打印符号类型
        std::string type_str;
        switch (sym.type) {
        case SymbolType::LOCAL:
            type_str = "LOCAL ";
            break;
        case SymbolType::WEAK:
            type_str = "WEAK  ";
            break;
        case SymbolType::GLOBAL:
            type_str = "GLOBAL";
            break;
        case SymbolType::UNDEFINED:
            type_str = "UNDEF ";
            break;
        }
        std::cout << std::left << std::setw(7) << type_str << " ";

        // 打印节名和偏移
        std::cout << std::left << std::setw(max_section_name_len) << sym.section << " "
                  << std::left << std::setw(10) << format_hex(sym.offset, 4) << " "
                  << std::left << format_hex(sym.size, 4) << std::endl;
    }
    std::cout << std::endl;

    // 打印重定位信息
    std::cout << "Relocations:" << std::endl;
    for (const auto& [section_name, section] : obj.sections) {
        if (!section.relocs.empty()) {
            std::cout << section_name << ":" << std::endl;
            // 打印表头
            std::cout << std::setfill(' ');
            std::cout << "  " << std::left << std::setw(10) << "Offset"
                      << std::left << std::setw(15) << "Type"
                      << std::left << std::setw(max_symbol_name_len) << "Symbol"
                      << " Addend" << std::endl;
            print_separator(max_symbol_name_len + 35);

            for (const auto& reloc : section.relocs) {
                std::cout << "  " << std::left << std::setw(10) << format_hex(reloc.offset, 2);

                // 打印重定位类型
                std::string type_str;
                switch (reloc.type) {
                case RelocationType::R_X86_64_32:
                    type_str = "R_X86_64_32";
                    break;
                case RelocationType::R_X86_64_PC32:
                    type_str = "R_X86_64_PC32";
                    break;
                case RelocationType::R_X86_64_64:
                    type_str = "R_X86_64_64";
                    break;
                case RelocationType::R_X86_64_32S:
                    type_str = "R_X86_64_32S";
                    break;
                }
                std::cout << std::left << std::setw(15) << type_str
                          << std::left << std::setw(max_symbol_name_len) << reloc.symbol
                          << " " << format_hex(reloc.addend, 8) << std::endl;
            }
            std::cout << std::endl;
        }
    }

    // 如果是可执行文件，打印程序头
    if (obj.type == ".exe" && !obj.phdrs.empty()) {
        std::cout << "Program Headers:" << std::endl;
        // 打印表头
        std::cout << std::setfill(' ');
        std::cout << "  " << std::left << std::setw(20) << "Name"
                  << std::left << std::setw(18) << "Virtual Address"
                  << std::left << std::setw(10) << "Size"
                  << "Flags" << std::endl;
        print_separator(65);

        for (const auto& phdr : obj.phdrs) {
            std::cout << std::setfill(' ');
            std::cout << "  " << std::left << std::setw(20) << phdr.name
                      << std::left << std::setw(18) << format_hex(phdr.vaddr, 8)
                      << std::left << std::setw(10) << format_hex(phdr.size, 4) << " ";

            std::vector<std::string> flags;
            if (phdr.flags & static_cast<uint32_t>(PHF::R))
                flags.push_back("R");
            if (phdr.flags & static_cast<uint32_t>(PHF::W))
                flags.push_back("W");
            if (phdr.flags & static_cast<uint32_t>(PHF::X))
                flags.push_back("X");

            for (size_t i = 0; i < flags.size(); i++) {
                std::cout << flags[i];
                if (i < flags.size() - 1)
                    std::cout << "|";
            }
            std::cout << std::endl;
        }
    }
}