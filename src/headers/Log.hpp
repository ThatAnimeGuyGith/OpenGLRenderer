#pragma once

#include <format>
#include <string>
#include <string_view>
#include <utility>

namespace Log {

void write(std::string_view level, const std::string& message);
void error(std::string_view message);

template <typename... Args>
void info(std::format_string<Args...> fmt, Args&&... args)
{
    write("INFO", std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void warn(std::format_string<Args...> fmt, Args&&... args)
{
    write("WARN", std::format(fmt, std::forward<Args>(args)...));
}

}