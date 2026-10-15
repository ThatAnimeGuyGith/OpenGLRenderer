#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>

class Shader {
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void use() const;
    GLuint id() const noexcept;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setMat4(const std::string& name, const float* value) const;

private:
    GLint getUniformLocation(const std::string& name) const;

    GLuint programId = 0;
    mutable std::unordered_map<std::string, GLint> uniformLocations;
};