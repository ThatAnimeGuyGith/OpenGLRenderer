#pragma once

#include <stdexcept>
#include <string>

enum class ErrorCategory {
    Config,
    Window,
    Graphics,
    Shader,
    Io
};

class RendererError : public std::runtime_error {
public:
    RendererError(ErrorCategory category, const std::string& message);

    ErrorCategory category() const;

private:
    ErrorCategory category_;
};

const char* toString(ErrorCategory category);
int exitCode(ErrorCategory category);