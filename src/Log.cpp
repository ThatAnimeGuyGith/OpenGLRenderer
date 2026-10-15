#include "Log.hpp"

#include <iostream>

namespace Log {

void write(std::string_view level, const std::string& message)
{
    std::clog << std::format("[{}] {}\n", level, message);
}

void error(std::string_view message, std::source_location loc)
{
    std::clog << std::format(
        "[ERROR] {}:{}: {}\n",
        loc.file_name(),
        loc.line(),
        message);
}

}
