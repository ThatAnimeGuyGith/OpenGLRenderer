#include "Paths.hpp"

#include "Error.hpp"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <unistd.h>
#endif

std::filesystem::path executableDir()
{
#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (length == 0 || length == MAX_PATH) {
        throw RendererError(
            ErrorCategory::Io,
            "Could not determine the executable path");
    }
    return std::filesystem::path(buffer).parent_path();
#else
    char buffer[4096];
    const ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (length < 0) {
        throw RendererError(
            ErrorCategory::Io,
            "Could not determine the executable path");
    }
    buffer[length] = '\0';
    return std::filesystem::path(buffer).parent_path();
#endif
}

std::filesystem::path assetPath(const std::filesystem::path& relative)
{
    return executableDir() / "assets" / relative;
}
