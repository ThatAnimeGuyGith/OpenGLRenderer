#include "Log.hpp"

#include <iostream>

namespace Log {

void write(std::string_view level, const std::string& message)
{
    std::clog << std::format("[{}] {}\n", level, message);
}

void error(std::string_view message)
{
    write("ERROR", std::string(message));
}

}
