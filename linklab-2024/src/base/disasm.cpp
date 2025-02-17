#include "fle.hpp"
#include "string_utils.hpp"
#include "utils.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

// 辅助函数：格式化地址
std::string format_address(uint64_t addr)
{
    std::stringstream ss;
    ss << std::hex << std::setw(4) << std::setfill('0') << addr;
    return ss.str();
}

// 辅助函数：获取重定位类型的字符串表示
std::string get_reloc_type_str(RelocationType type)
{
    switch (type) {
    case RelocationType::R_X86_64_32:
        return "R_X86_64_32";
    case RelocationType::R_X86_64_PC32:
        return "R_X86_64_PC32";
    case RelocationType::R_X86_64_64:
        return "R_X86_64_64";
    case RelocationType::R_X86_64_32S:
        return "R_X86_64_32S";
    default:
        return "UNKNOWN";
    }
}

// 辅助函数：判断是否是代码段
bool is_code_section(const std::string& section_name)
{
    return section_name.find(".text") != std::string::npos;
}

// 辅助函数：格式化数据字节
std::string format_data_bytes(const std::vector<uint8_t>& data, size_t offset, size_t max_len = 16)
{
    std::stringstream ss;
    for (size_t i = 0; i < max_len && offset + i < data.size(); ++i) {
        if (i > 0)
            ss << " ";
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[offset + i]);
    }
    return ss.str();
}

// 辅助函数：获取字符串实际长度
size_t get_string_length(const std::vector<uint8_t>& data, size_t offset)
{
    size_t len = 0;
    while (offset + len < data.size() && data[offset + len] != 0) {
        len++;
    }
    return len + 1; // 包含结尾的 null 字符
}

// 辅助函数：格式化字符串内容为注释
std::string format_string_comment(const std::vector<uint8_t>& data, size_t offset, size_t len)
{
    std::stringstream ss;
    ss << "# \"";
    for (size_t i = 0; i < len - 1; ++i) { // -1 to exclude null terminator
        char c = static_cast<char>(data[offset + i]);
        if (c == '\n')
            ss << "\\n";
        else if (c == '\t')
            ss << "\\t";
        else if (c == '\r')
            ss << "\\r";
        else if (c == '\"')
            ss << "\\\"";
        else if (c == '\\')
            ss << "\\\\";
        else if (isprint(c))
            ss << c;
        else
            ss << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
    }
    ss << "\"";
    return ss.str();
}

void FLE_disasm(const FLEObject& obj, const std::string& section_name)
{
    // 查找指定的段
    auto it = obj.sections.find(section_name);
    if (it == obj.sections.end()) {
        throw std::runtime_error("Section not found: " + section_name);
    }

    const auto& section = it->second;
    const auto& data = section.data;

    if (data.empty()) {
        throw std::runtime_error("Section is empty");
    }

    // 创建偏移到重定位信息的映射
    std::map<uint64_t, std::vector<const Relocation*>> reloc_map;
    // 创建偏移到符号的映射
    std::map<uint64_t, const Symbol*> symbol_map;

    if (obj.type == ".obj") {
        for (const auto& reloc : section.relocs) {
            reloc_map[reloc.offset].push_back(&reloc);
        }
    }

    for (const auto& sym : obj.symbols) {
        if (sym.section == section_name) {
            symbol_map[sym.offset] = &sym;
        }
    }

    std::cout << "Disassembly of section " << section_name << ":" << std::endl;

    // 如果是数据段，直接显示数据
    if (!is_code_section(section_name)) {
        // 首先获取所有符号的偏移量并排序
        std::vector<std::pair<uint64_t, const Symbol*>> sorted_symbols;
        for (const auto& [offset, sym] : symbol_map) {
            sorted_symbols.push_back({ offset, sym });
        }
        std::sort(sorted_symbols.begin(), sorted_symbols.end());

        // 遍历所有符号
        for (const auto& [sym_offset, sym] : sorted_symbols) {
            std::cout << std::endl;
            std::cout << sym->name << ":" << std::endl;

            // 确定数据长度
            size_t data_len = sym->size;
            bool is_string = false;
            if (section_name.find(".rodata.str") != std::string::npos) {
                // 对于字符串段，使用实际字符串长度
                data_len = get_string_length(data, sym_offset);
                is_string = true;
            }

            // 按每16字节一行输出数据
            for (size_t i = 0; i < data_len; i += 16) {
                size_t chunk_size = std::min(size_t(16), data_len - i);
                std::cout << format_address(sym_offset + i) << ": "
                          << std::left << std::setfill(' ') << std::setw(50)
                          << format_data_bytes(data, sym_offset + i, chunk_size);

                // 对于第一行，输出重定位信息和字符串内容
                if (i == 0) {
                    if (obj.type == ".obj") {
                        auto reloc_it = reloc_map.find(sym_offset);
                        if (reloc_it != reloc_map.end()) {
                            std::cout << "# ";
                            for (const auto* reloc : reloc_it->second) {
                                std::cout << get_reloc_type_str(reloc->type) << " "
                                          << reloc->symbol;
                                if (reloc->addend != 0) {
                                    std::cout << std::showpos << std::dec << reloc->addend;
                                }
                                std::cout << " ";
                            }
                        }
                    }
                    if (is_string) {
                        if (obj.type == ".obj")
                            std::cout << "  ";
                        std::cout << format_string_comment(data, sym_offset, data_len);
                    }
                }
                std::cout << std::endl;
            }
        }
        return;
    }

    // 对于代码段，使用 objdump 反汇编
    // 创建临时文件来存储段数据
    std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
    std::string temp_file = (temp_dir / "section.bin").string();

    // 写入段数据
    std::ofstream out(temp_file, std::ios::binary);
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
    out.close();

    // 构造 objdump 命令
    std::stringstream cmd;
    cmd << "objdump -D -b binary -m i386:x86-64 " << temp_file;

    try {
        std::string output = execute_command(cmd.str());

        std::istringstream iss(output);
        std::string line;
        bool start_processing = false;
        uint64_t next_addr = 0;

        while (std::getline(iss, line)) {
            if (line.find("Disassembly of section") != std::string::npos) {
                start_processing = true;
                continue;
            }

            if (!start_processing || line.empty() || line.find('>') != std::string::npos)
                continue;

            line = trim(line);
            if (line.empty() || !std::isxdigit(line[0]))
                continue;

            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                // 提取当前指令的地址
                std::string addr_str = line.substr(0, colon_pos);
                uint64_t addr;
                std::stringstream ss;
                ss << std::hex << addr_str;
                ss >> addr;

                // 检查是否有符号在这个地址
                if (obj.type == ".obj") {
                    auto sym_it = symbol_map.find(addr);
                    if (sym_it != symbol_map.end()) {
                        std::cout << std::endl; // 在符号前添加空行
                        std::cout << sym_it->second->name << ":" << std::endl;
                    }
                }

                // 提取机器码和指令
                std::string rest = line.substr(colon_pos + 1);
                rest = trim(rest);

                size_t instr_pos = rest.find_first_not_of("0123456789abcdef ");
                if (instr_pos != std::string::npos) {
                    std::string bytes = trim(rest.substr(0, instr_pos));
                    std::string instr = trim(rest.substr(instr_pos));

                    // 如果是.obj文件，去掉objdump的注释
                    if (obj.type == ".obj") {
                        size_t comment_pos = instr.find('#');
                        if (comment_pos != std::string::npos) {
                            instr = trim(instr.substr(0, comment_pos));
                        }
                    }

                    // 计算指令长度
                    std::istringstream byte_stream(bytes);
                    std::string byte;
                    size_t instr_len = 0;
                    while (byte_stream >> byte) {
                        if (byte.length() == 2)
                            instr_len++;
                    }
                    next_addr = addr + instr_len;

                    // 基本输出
                    std::cout << format_address(addr) << ": "
                              << std::left << std::setfill(' ') << std::setw(30) << bytes
                              << std::left << std::setw(30) << instr;

                    // 检查这条指令范围内的重定位信息
                    if (obj.type == ".obj") {
                        bool has_reloc = false;
                        for (uint64_t offset = addr; offset < next_addr; offset++) {
                            auto reloc_it = reloc_map.find(offset);
                            if (reloc_it != reloc_map.end()) {
                                if (!has_reloc) {
                                    std::cout << "# ";
                                    has_reloc = true;
                                }
                                for (const auto* reloc : reloc_it->second) {
                                    std::cout << get_reloc_type_str(reloc->type) << " "
                                              << reloc->symbol;
                                    if (reloc->addend != 0) {
                                        std::cout << std::showpos << std::dec << reloc->addend;
                                    }
                                    std::cout << " ";
                                }
                            }
                        }
                    }
                    std::cout << std::endl;
                }
            }
        }

        std::filesystem::remove(temp_file);

    } catch (const std::exception& e) {
        std::filesystem::remove(temp_file);
        throw;
    }
}