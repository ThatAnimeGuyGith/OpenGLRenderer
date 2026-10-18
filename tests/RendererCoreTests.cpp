#include "Error.hpp"
#include "ModelList.hpp"
#include "ObjMeshData.hpp"
#include "Rotation.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {
void require(bool condition, std::string_view message)
{
    if (!condition)
        throw std::runtime_error(std::string(message));
}

void requireIoError(const std::vector<float>& positions, const std::vector<ObjMeshIndex>& indices, std::string_view detail)
{
    try {
        (void)processObjMeshVertices(positions, {}, indices, "test mesh");
    } catch (const RendererError& error) {
        require(error.category() == ErrorCategory::Io, "Expected an IO error");
        require(std::string_view(error.what()).find(detail) != std::string_view::npos, "Unexpected error detail");
        return;
    }
    throw std::runtime_error("Expected OBJ processing to fail");
}

void testZeroSizeMeshIsRejected()
{
    requireIoError({0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, {{0, -1}, {1, -1}, {2, -1}}, "zero size");
}

void testMissingNormalsUseFaceNormal()
{
    const auto vertices = processObjMeshVertices(
        {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
        {}, {{0, -1}, {1, -1}, {2, -1}}, "test mesh");
    require(vertices.size() == 3, "Expected one triangle");
    for (const auto& vertex : vertices)
        require(std::abs(vertex.normal[2] - 1.0f) < 0.0001f, "Missing normals did not use the face normal");
}

void testInvalidIndicesAreRejected()
{
    requireIoError({0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f}, {{0, -1}, {1, -1}, {3, -1}}, "invalid vertex index");
}

void testRotationPausesAndResumes()
{
    RotationState rotation;
    advanceRotation(rotation, 2.0f);
    require(std::abs(rotation.angleRadians - 0.7f) < 0.0001f, "Rotation did not advance by delta time");
    rotation.enabled = false;
    advanceRotation(rotation, 10.0f);
    require(std::abs(rotation.angleRadians - 0.7f) < 0.0001f, "Paused rotation advanced");
    rotation.enabled = true;
    advanceRotation(rotation, 1.0f);
    require(std::abs(rotation.angleRadians - 1.05f) < 0.0001f, "Rotation did not resume from the paused angle");
}

void testBuiltInModelList()
{
    const auto models = builtInModels();
    require(models.size() >= 2, "Expected at least two built-in models");
    require(models[0].name == "Cube", "Cube is the default model");
    require(isBuiltInModelIndex(models.size() - 1), "Last model index should be valid");
    require(!isBuiltInModelIndex(models.size()), "Out-of-range model index should be invalid");
}
}

int main()
{
    const std::vector<std::pair<std::string_view, void (*)()>> tests{
        {"zero-size mesh", testZeroSizeMeshIsRejected},
        {"missing normals", testMissingNormalsUseFaceNormal},
        {"invalid indices", testInvalidIndicesAreRejected},
        {"rotation", testRotationPausesAndResumes},
        {"model list", testBuiltInModelList}
    };
    unsigned int failures = 0;
    for (const auto& [name, test] : tests) {
        try {
            test();
            std::cout << "PASS: " << name << '\n';
        } catch (const std::exception& error) {
            ++failures;
            std::cerr << "FAIL: " << name << ": " << error.what() << '\n';
        }
    }
    return failures == 0 ? 0 : 1;
}
