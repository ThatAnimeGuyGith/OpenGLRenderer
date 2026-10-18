#include "Paths.hpp"

#include "Error.hpp"

#include <cstdint>
#include <cstdlib>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#else
#error "executableDir() is not implemented for this platform. Add a branch in Paths.cpp."
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
#elif defined(__APPLE__)
    // macOS has no /proc. The first call fails on purpose and reports the
    // buffer size it needs.
    std::uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    if (size == 0) {
        throw RendererError(
            ErrorCategory::Io,
            "Could not determine the executable path");
    }
    std::vector<char> buffer(size);
    if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
        throw RendererError(
            ErrorCategory::Io,
            "Could not determine the executable path");
    }
    // The returned path can contain symlinks or "..", so resolve it.
    std::error_code error;
    const std::filesystem::path resolved = std::filesystem::canonical(buffer.data(), error);
    return (error ? std::filesystem::path(buffer.data()) : resolved).parent_path();
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

std::filesystem::path userDataDir()
{
#ifdef _WIN32
    const char* localAppData = std::getenv("LOCALAPPDATA");
    if (localAppData == nullptr || *localAppData == '\0') {
        throw RendererError(
            ErrorCategory::Io,
            "Could not determine the LocalAppData directory");
    }
    return std::filesystem::path(localAppData) / "OpenGLRenderer";
#elif defined(__APPLE__)
    const char* home = std::getenv("HOME");
    if (home == nullptr || *home == '\0') {
        throw RendererError(
            ErrorCategory::Io,
            "Could not determine the home directory");
    }
    return std::filesystem::path(home) / "Library" / "Application Support" / "OpenGLRenderer";
#else
    const char* xdgDataHome = std::getenv("XDG_DATA_HOME");
    if (xdgDataHome != nullptr && *xdgDataHome != '\0')
        return std::filesystem::path(xdgDataHome) / "openglrenderer";

    const char* home = std::getenv("HOME");
    if (home == nullptr || *home == '\0') {
        throw RendererError(
            ErrorCategory::Io,
            "Could not determine the home directory");
    }
    return std::filesystem::path(home) / ".local" / "share" / "openglrenderer";
#endif
}

std::filesystem::path userSettingsPath()
{
    const std::filesystem::path settingsPath = userDataDir() / "settings.json";
    std::error_code error;
    std::filesystem::create_directories(settingsPath.parent_path(), error);
    if (error) {
        throw RendererError(
            ErrorCategory::Io,
            "Could not create the user data directory: " + error.message());
    }

    if (!std::filesystem::exists(settingsPath, error)) {
        if (error) {
            throw RendererError(
                ErrorCategory::Io,
                "Could not inspect the user settings file: " + error.message());
        }

        std::filesystem::copy_file(
            assetPath("config/settings.json"),
            settingsPath,
            std::filesystem::copy_options::none,
            error);
        if (error) {
            throw RendererError(
                ErrorCategory::Io,
                "Could not create the user settings file: " + error.message());
        }
    }

    return settingsPath;
}
