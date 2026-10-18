#include "ObjMeshData.hpp"

#include "Error.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {
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

std::array<float, 3> positionAt(const std::vector<float>& positions, int index, std::string_view meshName)
{
    if (index < 0 || static_cast<std::size_t>(index) * 3 + 2 >= positions.size())
        throw RendererError(ErrorCategory::Io, "OBJ mesh has an invalid vertex index: " + std::string(meshName));
    const std::size_t offset = static_cast<std::size_t>(index) * 3;
    return {positions[offset], positions[offset + 1], positions[offset + 2]};
}
}

std::vector<ProcessedMeshVertex> processObjMeshVertices(
    const std::vector<float>& positions,
    const std::vector<float>& normals,
    const std::vector<ObjMeshIndex>& indices,
    std::string_view meshName)
{
    if (positions.empty())
        throw RendererError(ErrorCategory::Io, "OBJ mesh contains no vertices: " + std::string(meshName));
    if (positions.size() % 3 != 0)
        throw RendererError(ErrorCategory::Io, "OBJ mesh has incomplete vertex data: " + std::string(meshName));
    if (indices.size() % 3 != 0)
        throw RendererError(ErrorCategory::Io, "OBJ mesh is not triangulated: " + std::string(meshName));

    std::array<float, 3> minimum{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    std::array<float, 3> maximum{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};
    for (std::size_t offset = 0; offset < positions.size(); offset += 3) {
        for (std::size_t axis = 0; axis < 3; ++axis) {
            minimum[axis] = std::min(minimum[axis], positions[offset + axis]);
            maximum[axis] = std::max(maximum[axis], positions[offset + axis]);
        }
    }
    const std::array<float, 3> center{(minimum[0] + maximum[0]) * 0.5f, (minimum[1] + maximum[1]) * 0.5f, (minimum[2] + maximum[2]) * 0.5f};
    const float extent = std::max({maximum[0] - minimum[0], maximum[1] - minimum[1], maximum[2] - minimum[2]});
    if (extent <= std::numeric_limits<float>::epsilon())
        throw RendererError(ErrorCategory::Io, "OBJ mesh has zero size: " + std::string(meshName));

    std::vector<ProcessedMeshVertex> vertices;
    vertices.reserve(indices.size());
    for (std::size_t face = 0; face < indices.size(); face += 3) {
        std::array<std::array<float, 3>, 3> facePositions{};
        for (std::size_t corner = 0; corner < 3; ++corner) {
            const auto position = positionAt(positions, indices[face + corner].vertexIndex, meshName);
            facePositions[corner] = {(position[0] - center[0]) * 2.0f / extent, (position[1] - center[1]) * 2.0f / extent, (position[2] - center[2]) * 2.0f / extent};
        }
        const auto faceNormal = normalize(cross(subtract(facePositions[1], facePositions[0]), subtract(facePositions[2], facePositions[0])));
        for (std::size_t corner = 0; corner < 3; ++corner) {
            const int normalIndex = indices[face + corner].normalIndex;
            std::array<float, 3> normal = faceNormal;
            if (normalIndex >= 0 && static_cast<std::size_t>(normalIndex) * 3 + 2 < normals.size()) {
                const std::size_t offset = static_cast<std::size_t>(normalIndex) * 3;
                normal = normalize({normals[offset], normals[offset + 1], normals[offset + 2]});
            }
            vertices.push_back({facePositions[corner], normal});
        }
    }
    if (vertices.empty())
        throw RendererError(ErrorCategory::Io, "OBJ mesh has no drawable triangles: " + std::string(meshName));
    return vertices;
}
