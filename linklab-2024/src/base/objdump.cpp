#include "fle.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

void FLE_objdump(const FLEObject& obj, FLEWriter& writer)
{
    writer.set_type(obj.type);

    // 如果是可执行文件，写入程序头和入口点
    if (obj.type == ".exe") {
        writer.write_program_headers(obj.phdrs);
        writer.write_entry(obj.entry);
    }

    // 预处理：构建符号表索引
    std::map<std::string, std::map<size_t, std::vector<Symbol>>> symbol_index;
    for (const auto& sym : obj.symbols) {
        if (sym.type != SymbolType::UNDEFINED) {
            symbol_index[sym.section][sym.offset].push_back(sym);
        }
    }

    std::vector<std::tuple<std::string, size_t, FLESection>> sections;
    for (const auto& pair : obj.sections) {
        const auto& name = pair.first;
        const auto& section = pair.second;
        auto shdr = std::find_if(obj.shdrs.begin(), obj.shdrs.end(), [&](const auto& shdr) {
            return shdr.name == name;
        });
        if (shdr == obj.shdrs.end()) {
            sections.push_back({ name, 0, section });
            continue;
        }
        sections.push_back({ name, shdr->offset, section });
    }
    std::sort(sections.begin(), sections.end(), [](const auto& a, const auto& b) {
        return std::get<1>(a) < std::get<1>(b);
    });

    // 写入所有段的内容
    for (const auto& [name, _, section] : sections) {
        writer.begin_section(name);

        // 收集所有断点（符号和重定位的位置）
        std::vector<size_t> breaks;
        for (const auto& sym : obj.symbols) {
            if (sym.section == name) { // only collect symbols for current section
                breaks.push_back(sym.offset);
            }
        }
        for (const auto& reloc : section.relocs) {
            breaks.push_back(reloc.offset);
        }
        std::sort(breaks.begin(), breaks.end());
        breaks.erase(std::unique(breaks.begin(), breaks.end()), breaks.end());

        size_t pos = 0;
        while (pos < section.data.size()) {
            // 1. 检查当前位置是否有符号
            auto section_it = symbol_index.find(name);
            if (section_it != symbol_index.end()) {
                auto offset_it = section_it->second.find(pos);
                if (offset_it != section_it->second.end()) {
                    for (const auto& sym : offset_it->second) {
                        std::string line;
                        switch (sym.type) {
                        case SymbolType::LOCAL:
                            line = "🏷️: " + sym.name;
                            break;
                        case SymbolType::WEAK:
                            line = "📎: " + sym.name;
                            break;
                        case SymbolType::GLOBAL:
                            line = "📤: " + sym.name;
                            break;
                        default:
                            [[unlikely]] throw std::runtime_error("unknown symbol type");
                        }
                        line += " " + std::to_string(sym.size) + " " + std::to_string(sym.offset);
                        writer.write_line(line);
                    }
                }
            }

            for (const auto& reloc : section.relocs) {
                if (reloc.offset == pos) {
                    std::string reloc_format;
                    if (reloc.type == RelocationType::R_X86_64_PC32) {
                        reloc_format = ".rel";
                    } else if (reloc.type == RelocationType::R_X86_64_32) {
                        reloc_format = ".abs";
                    } else if (reloc.type == RelocationType::R_X86_64_64) {
                        reloc_format = ".abs64";
                    } else if (reloc.type == RelocationType::R_X86_64_32S) {
                        reloc_format = ".abs32s";
                    }

                    std::stringstream ss;
                    ss << "❓: " << reloc_format << "(" << reloc.symbol << ", 0x"
                       << std::hex << reloc.addend << ")";
                    writer.write_line(ss.str());
                }
            }

            // 2. 找出下一个断点
            size_t next_break = section.data.size();
            for (size_t brk : breaks) {
                if (brk > pos) {
                    next_break = brk;
                    break;
                }
            }

            // 3. 输出数据，每16字节一组
            while (pos < next_break) {
                std::stringstream ss;
                ss << "🔢: ";
                size_t chunk_size = std::min({
                    size_t(16), // 最大16字节
                    next_break - pos, // 到下一个断点
                    section.data.size() - pos // 剩余数据
                });

                for (size_t i = 0; i < chunk_size; ++i) {
                    ss << std::hex << std::setw(2) << std::setfill('0')
                       << static_cast<int>(section.data[pos + i]);
                    if (i < chunk_size - 1) {
                        ss << " ";
                    }
                }
                writer.write_line(ss.str());
                pos += chunk_size;
            }

            // 4. 如果是重定位，跳过字节
            auto reloc_it = std::find_if(section.relocs.begin(), section.relocs.end(),
                [pos](const auto& r) { return r.offset == pos; });
            if (reloc_it != section.relocs.end()) {
                pos += (reloc_it->type == RelocationType::R_X86_64_64) ? 8 : 4;
            }
        }

        writer.end_section();
    }
}