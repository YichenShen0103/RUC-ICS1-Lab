#pragma once

#include <algorithm>
#include <array>
#include <cstdio>
#include <fmt/format.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

// 执行系统命令并返回输出结果
inline std::string execute_command(std::string_view cmd)
{
    auto command_with_stderr = fmt::format("LANG=C {} 2>/dev/null", cmd);

    std::unique_ptr<FILE, decltype(&pclose)> pipe(
        popen(command_with_stderr.c_str(), "r"),
        pclose);

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    std::string result;
    std::array<char, 128> buffer {};
    while (std::size_t bytes_read = std::fread(buffer.data(), 1, buffer.size(), pipe.get())) {
        result.append(buffer.data(), bytes_read);
    }

    return result;
}

// 检查容器是否包含元素
template <typename Container, typename T>
constexpr bool contains(const Container& container, const T& value)
{
    return std::find(std::begin(container), std::end(container), value) != std::end(container);
}
