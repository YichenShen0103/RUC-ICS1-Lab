#define FMT_HEADER_ONLY
#include "fle.hpp"
#include "string_utils.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fmt/format.h>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace {
// 符号表项结构
struct Symbol {
    char binding;
    std::string type;
    std::string section;
    unsigned int offset;
    unsigned int size;
    std::string name;

    // 添加构造函数使用 std::regex_match 的结果初始化
    static Symbol from_regex_match(const std::smatch& match, std::string_view section)
    {
        return {
            .binding = match[2].str()[0],
            .type = match[3].str(),
            .section = std::string { section },
            .offset = static_cast<unsigned int>(std::stoul(match[1].str(), nullptr, 16)),
            .size = static_cast<unsigned int>(std::stoul(match[5].str(), nullptr, 16)),
            .name = match[6].str()
        };
    }
};

// 重定位类型到格式的映射
struct RelocationFormat {
    std::string_view format;
    size_t size;
};

constexpr auto RELOCATION_FORMATS = std::array {
    std::pair { "R_X86_64_PC32"sv, RelocationFormat { ".rel"sv, 4 } },
    std::pair { "R_X86_64_PLT32"sv, RelocationFormat { ".rel"sv, 4 } },
    std::pair { "R_X86_64_64"sv, RelocationFormat { ".abs64"sv, 8 } },
    std::pair { "R_X86_64_32"sv, RelocationFormat { ".abs"sv, 4 } },
    std::pair { "R_X86_64_32S"sv, RelocationFormat { ".abs32s"sv, 4 } }
};

// 解析符号表
std::vector<Symbol> parse_symbols(const std::string& binary, std::string_view section)
{
    static const std::regex symbol_pattern {
        R"(^([0-9a-fA-F]+)\s+(l|g|w)\s+(\w+)?\s+([.a-zA-Z0-9_]+)\s+([0-9a-fA-F]+)\s+(.*)$)"
    };

    std::vector<Symbol> symbols;
    const auto symbol_dump = execute_command(fmt::format("objdump -t {}", binary));

    for (const auto& line : splitlines(symbol_dump)) {
        if (std::smatch match; std::regex_match(line, match, symbol_pattern)) {
            auto sym = Symbol::from_regex_match(match, section);
            if (match[4].str() != section) {
                continue;
            }
            symbols.push_back(sym);
        }
    }

    std::sort(symbols.begin(), symbols.end(), [](const Symbol& a, const Symbol& b) {
        return a.offset < b.offset;
    });

    return symbols;
}

// 生成符号行
std::string format_symbol_line(const Symbol& sym)
{
    switch (sym.binding) {
    case 'l':
        return fmt::format("🏷️: {} {} {}", sym.name, sym.size, sym.offset);
    case 'g':
        return fmt::format("📤: {} {} {}", sym.name, sym.size, sym.offset);
    case 'w':
        return fmt::format("📎: {} {} {}", sym.name, sym.size, sym.offset);
    default:
        throw std::runtime_error(fmt::format("Unsupported symbol binding: {}", sym.binding));
    }
}

// 解析重定位信息
std::map<int, std::pair<int, std::string>> parse_relocations(
    const std::string& binary, std::string_view section)
{
    static const std::regex reloc_pattern {
        R"(^\s*([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+(\S+)\s+([0-9a-fA-F]+)\s+(.*)$)"
    };

    std::map<int, std::pair<int, std::string>> relocations;
    const auto reloc_dump = execute_command(fmt::format("readelf -r {}", binary));
    bool in_section = false;

    for (const auto& line : splitlines(reloc_dump)) {
        if (str_contains(line, "Relocation section")) {
            in_section = str_contains(line, fmt::format("'.rela{}'", section));
            continue;
        }

        if (!in_section)
            continue;

        if (std::smatch match; std::regex_match(line, match, reloc_pattern)) {
            const int offset = std::stoi(match[1].str(), nullptr, 16);
            std::string symbol = match[5].str();

            if (const auto at_pos = symbol.find('@'); at_pos != std::string::npos) {
                symbol.resize(at_pos);
            }

            const std::string reloc_type = match[3].str();
            const auto format_it = std::find_if(RELOCATION_FORMATS.begin(), RELOCATION_FORMATS.end(),
                [reloc_type](const auto& pair) { return pair.first == reloc_type; });

            if (format_it == RELOCATION_FORMATS.end()) {
                throw std::runtime_error(fmt::format("Unsupported relocation type: {}", reloc_type));
            }

            const auto& [_, format] = *format_it;
            relocations.emplace(offset,
                std::pair { static_cast<int>(format.size),
                    fmt::format("{}({})", format.format, symbol) });
        }
    }
    return relocations;
}

std::vector<std::string> elf_to_fle(
    const std::string& binary, std::string_view section, bool is_bss = false)
{
    std::vector<std::string> result;
    const auto symbols = parse_symbols(binary, section);

    // BSS段只需处理符号
    if (is_bss) {
        for (const auto& sym : symbols) {
            result.push_back(format_symbol_line(sym));
        }
        return result;
    }

    // 获取节数据和重定位信息
    const auto section_data = execute_command(
        fmt::format("objcopy --dump-section {}=/dev/stdout {}", section, binary));
    const auto relocations = parse_relocations(binary, section);

    // 处理数据
    int skip = 0;
    std::vector<uint8_t> holding;
    holding.reserve(16);

    auto dump_holding = [&result](const std::vector<uint8_t>& holding) {
        if (holding.empty())
            return;

        std::string hex_dump;
        hex_dump.reserve(3 * holding.size());
        for (auto byte : holding) {
            fmt::format_to(std::back_inserter(hex_dump), "{:02x} ", byte);
        }
        result.push_back(fmt::format("🔢: {}", trim(hex_dump)));
    };

    for (size_t i = 0; i < section_data.size(); ++i) {
        // 处理符号
        for (const auto& sym : symbols) {
            if (sym.offset == i) {
                dump_holding(holding);
                holding.clear();
                result.push_back(format_symbol_line(sym));
            }
        }

        // 处理重定位
        if (const auto it = relocations.find(i); it != relocations.end()) {
            dump_holding(holding);
            holding.clear();
            const auto& [size, reloc] = it->second;
            result.push_back(fmt::format("❓: {}", reloc));
            skip = size;
        }

        if (skip > 0) {
            --skip;
        } else {
            holding.push_back(section_data[i]);
            if (holding.size() == 16) {
                dump_holding(holding);
                holding.clear();
            }
        }
    }
    dump_holding(holding);

    return result;
}

} // anonymous namespace

// 编译选项
constexpr auto COMPILER_FLAGS = std::array {
    "-static"sv,
    "-fno-common"sv,
    "-nostdlib"sv,
    "-ffreestanding"sv,
    "-fno-asynchronous-unwind-tables"sv,
};

void FLE_cc(const std::vector<std::string>& options)
{
    // std::cout << fmt::format("options: {}\n", join(options, " "));

    // 确定输出文件名
    const auto output_it = std::find(options.begin(), options.end(), "-o");
    const std::string binary = (output_it != options.end() && std::next(output_it) != options.end())
        ? *std::next(output_it)
        : "a.out";
    // std::cout << fmt::format("binary: {}\n", binary);

    // 编译命令
    std::vector<std::string> gcc_cmd = { "gcc", "-c" };
    gcc_cmd.insert(gcc_cmd.end(), COMPILER_FLAGS.begin(), COMPILER_FLAGS.end());
    gcc_cmd.insert(gcc_cmd.end(), options.begin(), options.end());

    // std::cerr << "running: " << join(gcc_cmd, " ") << "\n";

    if (std::system(join(gcc_cmd, " ").c_str()) != 0) {
        throw std::runtime_error("gcc compilation failed");
    }

    // 解析目标文件
    const auto objdump_output = execute_command(fmt::format("objdump -h {}", binary));
    FLEWriter writer;
    writer.set_type(".obj");

    // 处理每个节
    static const std::regex section_pattern {
        R"(^\s*([0-9]+)\s+(\.(\w|\.)+)\s+([0-9a-fA-F]+)\s+.*$)"
    };

    auto lines = splitlines(objdump_output);
    std::vector<SectionHeader> section_headers;
    std::vector<std::pair<std::string, bool>> sections_to_process;
    size_t current_offset = 0;

    // 第一遍扫描:收集节头信息
    for (auto it = lines.begin(); it != lines.end(); ++it) {
        std::smatch match;
        if (!std::regex_match(*it, match, section_pattern)) {
            continue;
        }

        const std::string section_name = match[2];

        // std::cerr << "section_name: " << section_name << "\n";
        const std::string flags_line = *++it;

        // 解析节标志
        std::vector<std::string> flags;
        std::istringstream ss(flags_line);
        std::string flag;
        while (std::getline(ss, flag, ',')) {
            flags.push_back(trim(flag));
        }
        size_t size = std::stoul(match[4].str(), nullptr, 16);

        // 检查是否需要处理该节
        if (!contains(flags, "ALLOC") || str_contains(section_name, "note.gnu.property") || size == 0) {
            continue;
        }

        // 设置节标志
        uint32_t sh_flags = static_cast<uint32_t>(SHF::ALLOC);
        if (contains(flags, "WRITE")) {
            sh_flags |= static_cast<uint32_t>(SHF::WRITE);
        }
        if (contains(flags, "EXECINSTR")) {
            sh_flags |= static_cast<uint32_t>(SHF::EXEC);
        }

        const bool is_nobits = !contains(flags, "CONTENTS");
        if (is_nobits) {
            sh_flags |= static_cast<uint32_t>(SHF::NOBITS);
        }

        // 创建节头
        section_headers.push_back(SectionHeader {
            .name = section_name,
            .type = static_cast<uint32_t>(is_nobits ? 8 : 1),
            .flags = sh_flags,
            .addr = 0,
            .offset = current_offset,
            .size = size,
        });

        current_offset += size;
        sections_to_process.emplace_back(section_name, is_nobits);
    }

    // 先写入所有节头
    writer.write_section_headers(section_headers);

    // 第二遍:写入节数据
    for (const auto& [section_name, is_nobits] : sections_to_process) {
        writer.begin_section(section_name);
        for (const auto& line : elf_to_fle(binary, section_name, is_nobits)) {
            writer.write_line(line);
        }
        writer.end_section();
    }

    // 写入输出文件
    const std::filesystem::path input_path { binary };
    const auto output_path = input_path.parent_path() / fmt::format("{}.fle", input_path.stem().string());
    // std::cout << fmt::format("output_path: {}\n", output_path.string());
    writer.write_to_file(output_path.string());

    std::filesystem::remove(binary);
}