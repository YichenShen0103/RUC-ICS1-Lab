#pragma once
#include "nlohmann/json.hpp"
#include <string>
#include <string_view>

using json = nlohmann::ordered_json;

// 字符串处理函数
inline std::string get_basename(std::string_view path)
{
    return std::filesystem::path(path).filename().string();
}

inline std::string get_filename_without_extension(std::string_view path)
{
    return std::filesystem::path(path).stem().string();
}

// 文件路径相关函数
inline std::string trim(std::string_view s)
{
    if (s.empty())
        return std::string();

    const auto start = std::min(s.find_first_not_of(" \t"), s.size());
    const auto end = s.find_last_not_of(" \t");

    // 如果字符串全是空白字符
    if (end == std::string_view::npos)
        return std::string();

    return std::string(s.substr(start, end - start + 1));
}

inline std::string trim(std::string_view s, std::string_view chars)
{
    s.remove_prefix(std::min(s.find_first_not_of(chars), s.size()));
    s.remove_suffix(s.size() - s.find_last_not_of(chars) - 1);
    return std::string(s);
}

inline std::vector<std::string> splitlines(std::string_view s)
{
    std::vector<std::string> lines;
    std::istringstream ss(s.data());
    std::string line;
    while (std::getline(ss, line, '\n')) {
        lines.push_back(line);
    }
    return lines;
}

inline std::string join(const std::vector<std::string>& v, std::string_view delim)
{
    std::string result;
    for (const auto& s : v) {
        if (!result.empty())
            result += delim;
        result += s;
    }
    return result;
}

inline bool starts_with(std::string_view s, std::string_view prefix)
{
    return s.find(prefix) == 0;
}

// 检查字符串是否包含子串
inline constexpr bool str_contains(std::string_view str, std::string_view sub)
{
    return str.find(sub) != std::string_view::npos;
}