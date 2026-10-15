#pragma once

#include <format>
#include <source_location>
#include <string>
#include <string_view>

namespace Log {

void write(std::string_view level, const std::string& message);
void error(std::string_view message, std::source_location loc = std::source_location::current());

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
