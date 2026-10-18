#include "ObjMesh.hpp"

#include "Error.hpp"
#include "Log.hpp"
#include "ObjPaths.hpp"

#include <tiny_obj_loader.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <vector>

namespace {
struct Vertex {
    std::array<float, 3> position;
    std::array<float, 3> normal;
};

std::array<float, 3> normalize(std::array<float, 3> value)
{
    const float length = std::sqrt(value[0] * value[0] + value[1] * value[1] + value[2] * value[2]);
    if (length <= std::numeric_limits<float>::epsilon())
        return {0.0f, 1.0f, 0.0f};
    return {value[0] / length, value[1] / length, value[2] / length};
}

std::array<float, 3> subtract(const std::array<float, 3>& left, const std::array<float, 3>& right)
{
    return {left[0] - right[0], left[1] - right[1], left[2] - right[2]};
}

std::array<float, 3> cross(const std::array<float, 3>& left, const std::array<float, 3>& right)
{
    return {
        left[1] * right[2] - left[2] * right[1],
        left[2] * right[0] - left[0] * right[2],
        left[0] * right[1] - left[1] * right[0]
    };
}
}

ObjMesh::ObjMesh(const std::filesystem::path& path)
{
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning;
    std::string error;
    const std::string pathString = path.string();
    const std::string basePath = objMaterialBasePath(path).generic_string() + "/";
    if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, pathString.c_str(), basePath.c_str(), true))
        throw RendererError(ErrorCategory::Io, "Failed to load OBJ mesh '" + pathString + "': " + error);
    if (!warning.empty())
        Log::warn("OBJ loader warning for '{}': {}", pathString, warning);
    if (attributes.vertices.empty())
        throw RendererError(ErrorCategory::Io, "OBJ mesh contains no vertices: " + pathString);

    std::array<float, 3> minimum{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    std::array<float, 3> maximum{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};
    for (std::size_t offset = 0; offset + 2 < attributes.vertices.size(); offset += 3) {
        for (std::size_t axis = 0; axis < 3; ++axis) {
            minimum[axis] = std::min(minimum[axis], attributes.vertices[offset + axis]);
            maximum[axis] = std::max(maximum[axis], attributes.vertices[offset + axis]);
        }
    }
    const std::array<float, 3> center{(minimum[0] + maximum[0]) * 0.5f, (minimum[1] + maximum[1]) * 0.5f, (minimum[2] + maximum[2]) * 0.5f};
    const float extent = std::max({maximum[0] - minimum[0], maximum[1] - minimum[1], maximum[2] - minimum[2]});
    if (extent <= std::numeric_limits<float>::epsilon())
        throw RendererError(ErrorCategory::Io, "OBJ mesh has zero size: " + pathString);

    std::vector<Vertex> vertices;
    for (const auto& shape : shapes) {
        const auto& indices = shape.mesh.indices;
        if (indices.size() % 3 != 0)
            throw RendererError(ErrorCategory::Io, "OBJ mesh is not triangulated: " + pathString);
        for (std::size_t face = 0; face < indices.size(); face += 3) {
            std::array<std::array<float, 3>, 3> positions{};
            for (std::size_t corner = 0; corner < 3; ++corner) {
                const int index = indices[face + corner].vertex_index;
                if (index < 0 || static_cast<std::size_t>(index) * 3 + 2 >= attributes.vertices.size())
                    throw RendererError(ErrorCategory::Io, "OBJ mesh has an invalid vertex index: " + pathString);
                const std::size_t offset = static_cast<std::size_t>(index) * 3;
                positions[corner] = {(attributes.vertices[offset] - center[0]) * 2.0f / extent, (attributes.vertices[offset + 1] - center[1]) * 2.0f / extent, (attributes.vertices[offset + 2] - center[2]) * 2.0f / extent};
            }
            const auto faceNormal = normalize(cross(subtract(positions[1], positions[0]), subtract(positions[2], positions[0])));
            for (std::size_t corner = 0; corner < 3; ++corner) {
                const int index = indices[face + corner].normal_index;
                std::array<float, 3> normal = faceNormal;
                if (index >= 0 && static_cast<std::size_t>(index) * 3 + 2 < attributes.normals.size()) {
                    const std::size_t offset = static_cast<std::size_t>(index) * 3;
                    normal = normalize({attributes.normals[offset], attributes.normals[offset + 1], attributes.normals[offset + 2]});
                }
                vertices.push_back({positions[corner], normal});
            }
        }
    }
    if (vertices.empty() || vertices.size() > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()))
        throw RendererError(ErrorCategory::Io, "OBJ mesh has no drawable triangles: " + pathString);

    glGenVertexArrays(1, &vertexArray_);
    glGenBuffers(1, &vertexBuffer_);
    glBindVertexArray(vertexArray_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    vertexCount_ = static_cast<GLsizei>(vertices.size());
    Log::info("Loaded OBJ mesh '{}' with {} vertices", pathString, vertexCount_);
}

ObjMesh::~ObjMesh()
{
    if (vertexBuffer_ != 0)
        glDeleteBuffers(1, &vertexBuffer_);
    if (vertexArray_ != 0)
        glDeleteVertexArrays(1, &vertexArray_);
}

void ObjMesh::draw() const
{
    glBindVertexArray(vertexArray_);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
}
