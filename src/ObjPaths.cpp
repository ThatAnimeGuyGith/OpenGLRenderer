#include "ObjPaths.hpp"

std::filesystem::path objMaterialBasePath(const std::filesystem::path& objPath)
{
    const std::filesystem::path parent = objPath.parent_path();
    return parent.empty() ? std::filesystem::current_path() : parent;
}
