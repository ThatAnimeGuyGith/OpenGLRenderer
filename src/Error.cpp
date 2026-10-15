#include "Error.hpp"

#include <format>

RendererError::RendererError(ErrorCategory category, const std::string& message)
    : std::runtime_error(std::format("[{}] {}", toString(category), message))
    , category_(category)
{
}

ErrorCategory RendererError::category() const
{
    return category_;
}

const char* toString(ErrorCategory category)
{
    switch (category) {
    case ErrorCategory::Config:
        return "Config";
    case ErrorCategory::Window:
        return "Window";
    case ErrorCategory::Graphics:
        return "Graphics";
    case ErrorCategory::Shader:
        return "Shader";
    case ErrorCategory::Io:
        return "Io";
    }
    return "Unknown";
}

int exitCode(ErrorCategory category)
{
    switch (category) {
    case ErrorCategory::Config:
        return 2;
    case ErrorCategory::Window:
        return 3;
    case ErrorCategory::Graphics:
        return 4;
    case ErrorCategory::Shader:
        return 5;
    case ErrorCategory::Io:
        return 6;
    }
    return 1;
}
