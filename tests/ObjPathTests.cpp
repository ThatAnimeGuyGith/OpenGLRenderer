#include "ObjPaths.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {
void require(bool condition, const char* message)
{
    if (!condition)
        throw std::runtime_error(message);
}

void testRelativeFilenameUsesCurrentDirectory()
{
    require(
        objMaterialBasePath("model.obj") == std::filesystem::current_path(),
        "A relative OBJ filename must resolve material libraries from the current directory");
}

void testFilenameWithParentKeepsParentDirectory()
{
    require(
        objMaterialBasePath("assets/models/model.obj") == std::filesystem::path("assets/models"),
        "An OBJ filename with a parent directory must keep that directory");
}

void testAbsoluteFilenameKeepsParentDirectory()
{
    const std::filesystem::path objPath = std::filesystem::current_path() / "models" / "model.obj";
    require(
        objMaterialBasePath(objPath) == objPath.parent_path(),
        "An absolute OBJ filename must keep its parent directory");
}
}

int main()
{
    try {
        testRelativeFilenameUsesCurrentDirectory();
        testFilenameWithParentKeepsParentDirectory();
        testAbsoluteFilenameKeepsParentDirectory();
        std::cout << "3/3 tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
