#include <tiny_obj_loader.h>

#include "renderer/ObjMesh.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
struct Vertex {
    std::array<float, 3> position;
    std::array<float, 3> normal;
};

std::array<float, 3> positionAt(
    const tinyobj::attrib_t& attributes,
    int index,
    const std::array<float, 3>& center,
    float scale
)
{
    if (index < 0 || static_cast<std::size_t>(index) * 3 + 2 >= attributes.vertices.size())
        throw std::runtime_error("OBJ mesh contains an invalid vertex index");

    const std::size_t offset = static_cast<std::size_t>(index) * 3;
    return {
        (attributes.vertices[offset] - center[0]) * scale,
        (attributes.vertices[offset + 1] - center[1]) * scale,
        (attributes.vertices[offset + 2] - center[2]) * scale
    };
}

std::array<float, 3> normalize(const std::array<float, 3>& vector)
{
    const float length = std::sqrt(
        vector[0] * vector[0] +
        vector[1] * vector[1] +
        vector[2] * vector[2]
    );
    if (length <= std::numeric_limits<float>::epsilon())
        return {0.0f, 1.0f, 0.0f};

    return {vector[0] / length, vector[1] / length, vector[2] / length};
}

std::array<float, 3> normalAt(
    const tinyobj::attrib_t& attributes,
    int index,
    const std::array<float, 3>& fallback
)
{
    if (index < 0 || static_cast<std::size_t>(index) * 3 + 2 >= attributes.normals.size())
        return fallback;

    const std::size_t offset = static_cast<std::size_t>(index) * 3;
    return normalize({
        attributes.normals[offset],
        attributes.normals[offset + 1],
        attributes.normals[offset + 2]
    });
}

void release(GLuint& vertexArray, GLuint& vertexBuffer) noexcept
{
    if (vertexBuffer != 0)
        glDeleteBuffers(1, &vertexBuffer);
    if (vertexArray != 0)
        glDeleteVertexArrays(1, &vertexArray);
    vertexBuffer = 0;
    vertexArray = 0;
}
}

ObjMesh::ObjMesh(const std::string& path)
{
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning;
    std::string error;
    const std::size_t lastSeparator = path.find_last_of("/\\");
    const std::string basePath = lastSeparator == std::string::npos
        ? std::string()
        : path.substr(0, lastSeparator + 1);

    if (!tinyobj::LoadObj(
            &attributes,
            &shapes,
            &materials,
            &warning,
            &error,
            path.c_str(),
            basePath.c_str(),
            true
        )) {
        throw std::runtime_error("Failed to load OBJ mesh '" + path + "': " + error);
    }
    if (!warning.empty())
        std::cerr << "OBJ loader warning for '" << path << "': " << warning;

    if (attributes.vertices.empty())
        throw std::runtime_error("OBJ mesh contains no vertices: " + path);

    std::array<float, 3> minimum = {
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max()
    };
    std::array<float, 3> maximum = {
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::lowest()
    };

    for (std::size_t offset = 0; offset + 2 < attributes.vertices.size(); offset += 3) {
        for (std::size_t axis = 0; axis < 3; ++axis) {
            minimum[axis] = std::min(minimum[axis], attributes.vertices[offset + axis]);
            maximum[axis] = std::max(maximum[axis], attributes.vertices[offset + axis]);
        }
    }

    const std::array<float, 3> center = {
        (minimum[0] + maximum[0]) * 0.5f,
        (minimum[1] + maximum[1]) * 0.5f,
        (minimum[2] + maximum[2]) * 0.5f
    };
    const float largestExtent = std::max({
        maximum[0] - minimum[0],
        maximum[1] - minimum[1],
        maximum[2] - minimum[2]
    });
    if (largestExtent <= std::numeric_limits<float>::epsilon())
        throw std::runtime_error("OBJ mesh has zero size: " + path);
    const float scale = 2.0f / largestExtent;

    std::vector<Vertex> vertices;
    for (const tinyobj::shape_t& shape : shapes) {
        const std::vector<tinyobj::index_t>& indices = shape.mesh.indices;
        if (indices.size() % 3 != 0)
            throw std::runtime_error("OBJ loader produced a non-triangulated mesh: " + path);

        for (std::size_t face = 0; face < indices.size(); face += 3) {
            const std::array<float, 3> positions[] = {
                positionAt(attributes, indices[face].vertex_index, center, scale),
                positionAt(attributes, indices[face + 1].vertex_index, center, scale),
                positionAt(attributes, indices[face + 2].vertex_index, center, scale)
            };
            const std::array<float, 3> edgeA = {
                positions[1][0] - positions[0][0],
                positions[1][1] - positions[0][1],
                positions[1][2] - positions[0][2]
            };
            const std::array<float, 3> edgeB = {
                positions[2][0] - positions[0][0],
                positions[2][1] - positions[0][1],
                positions[2][2] - positions[0][2]
            };
            const std::array<float, 3> faceNormal = normalize({
                edgeA[1] * edgeB[2] - edgeA[2] * edgeB[1],
                edgeA[2] * edgeB[0] - edgeA[0] * edgeB[2],
                edgeA[0] * edgeB[1] - edgeA[1] * edgeB[0]
            });

            for (std::size_t corner = 0; corner < 3; ++corner) {
                vertices.push_back({
                    positions[corner],
                    normalAt(attributes, indices[face + corner].normal_index, faceNormal)
                });
            }
        }
    }

    if (vertices.empty())
        throw std::runtime_error("OBJ mesh contains no triangle faces: " + path);
    if (vertices.size() > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()))
        throw std::runtime_error("OBJ mesh has too many vertices to draw: " + path);

    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);
    if (vertexArray == 0 || vertexBuffer == 0) {
        release(vertexArray, vertexBuffer);
        throw std::runtime_error("Failed to allocate GPU buffers for OBJ mesh: " + path);
    }

    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
        vertices.data(),
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, normal))
    );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    vertexCount = static_cast<GLsizei>(vertices.size());
}

ObjMesh::~ObjMesh()
{
    release(vertexArray, vertexBuffer);
}

void ObjMesh::draw() const
{
    glBindVertexArray(vertexArray);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
