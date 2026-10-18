#pragma once

#include <glad/glad.h>
#include <filesystem>
#include <string>

std::string loadShaderSource(const std::filesystem::path& path);

GLuint compileShader(GLenum type, const std::string& source, const std::string& name);

class ShaderProgram {
public:
    ShaderProgram(
        const std::filesystem::path& vertexPath,
        const std::filesystem::path& fragmentPath);
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    void use() const;
    void setMatrix4(const char* name, const float* value) const;
    void setFloat(const char* name, float value) const;
    void setVec3(const char* name, const float* value) const;

private:
    GLuint program_ = 0;
};
