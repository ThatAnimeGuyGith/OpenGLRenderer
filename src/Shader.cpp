#include "Shader.hpp"

#include "Error.hpp"
#include "Log.hpp"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
std::unordered_map<GLuint, std::unordered_map<std::string, GLint>> uniformLocationCache;

GLint uniformLocation(GLuint program, const char* name)
{
    auto& locations = uniformLocationCache[program];
    const auto found = locations.find(name);
    if (found != locations.end())
        return found->second;

    const GLint location = glGetUniformLocation(program, name);
    locations.emplace(name, location);
    if (location < 0)
        Log::warn("Shader program {} has no active uniform named '{}'", program, name);
    return location;
}
}

std::string loadShaderSource(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        throw RendererError(
            ErrorCategory::Io,
            "Failed to open shader file: " + path.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, const std::string& source, const std::string& name)
{
    const GLuint shader = glCreateShader(type);
    const char* sourcePtr = source.c_str();
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        std::string infoLog;
        if (logLength > 0) {
            std::vector<char> buffer(static_cast<size_t>(logLength));
            glGetShaderInfoLog(shader, logLength, nullptr, buffer.data());
            infoLog.assign(buffer.data());
        }

        glDeleteShader(shader);
        throw RendererError(
            ErrorCategory::Shader,
            "Failed to compile " + name + ":\n" + infoLog);
    }

    Log::info("Compiled shader {}", name);
    return shader;
}

ShaderProgram::ShaderProgram(
    const std::filesystem::path& vertexPath,
    const std::filesystem::path& fragmentPath)
{
    const std::string vertexSource = loadShaderSource(vertexPath);
    const std::string fragmentSource = loadShaderSource(fragmentPath);

    const GLuint vertexShader =
        compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath.string());

    GLuint fragmentShader = 0;
    try {
        fragmentShader =
            compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath.string());
    } catch (...) {
        glDeleteShader(vertexShader);
        throw;
    }

    program_ = glCreateProgram();
    uniformLocationCache.erase(program_);
    glAttachShader(program_, vertexShader);
    glAttachShader(program_, fragmentShader);
    glLinkProgram(program_);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success = 0;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &logLength);

        std::string infoLog;
        if (logLength > 0) {
            std::vector<char> buffer(static_cast<size_t>(logLength));
            glGetProgramInfoLog(program_, logLength, nullptr, buffer.data());
            infoLog.assign(buffer.data());
        }

        glDeleteProgram(program_);
        program_ = 0;
        throw RendererError(
            ErrorCategory::Shader,
            "Failed to link shader program:\n" + infoLog);
    }

    Log::info("Linked shader program");
}

ShaderProgram::~ShaderProgram()
{
    if (program_ != 0) {
        uniformLocationCache.erase(program_);
        glDeleteProgram(program_);
    }
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : program_(std::exchange(other.program_, 0))
{
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
{
    if (this != &other) {
        if (program_ != 0) {
            uniformLocationCache.erase(program_);
            glDeleteProgram(program_);
        }
        program_ = std::exchange(other.program_, 0);
    }
    return *this;
}

void ShaderProgram::use() const
{
    glUseProgram(program_);
}

void ShaderProgram::setMatrix4(const char* name, const float* value) const
{
    const GLint location = uniformLocation(program_, name);
    if (location >= 0)
        glUniformMatrix4fv(location, 1, GL_FALSE, value);
}

void ShaderProgram::setFloat(const char* name, float value) const
{
    const GLint location = uniformLocation(program_, name);
    if (location >= 0)
        glUniform1f(location, value);
}

void ShaderProgram::setVec3(const char* name, const float* value) const
{
    const GLint location = uniformLocation(program_, name);
    if (location >= 0)
        glUniform3fv(location, 1, value);
}
