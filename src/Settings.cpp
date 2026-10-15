#include "Settings.hpp"

#include "Error.hpp"
#include "Log.hpp"

#include <cstdint>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
constexpr std::int64_t kMaxWindowSize = 16384;

// Reads window.<key>, or returns `fallback` when the key is absent. A value of
// the wrong type is reported with the key name, which nlohmann's own message
// leaves out.
template <typename T>
T readValue(const json& window, const char* key, const T& fallback, const std::string& path)
{
    if (!window.contains(key)) {
        Log::warn("{}: window.{} is missing; using the default value", path, key);
        return fallback;
    }

    try {
        return window.at(key).get<T>();
    } catch (const json::type_error&) {
        throw RendererError(
            ErrorCategory::Config,
            std::format("{}: window.{} has the wrong type", path, key));
    }
}

// Reads window.<key> as a size. The value is read as a signed 64-bit integer
// and range-checked before it becomes unsigned, so "-5" is reported as a
// config error instead of wrapping around to a huge number.
unsigned int readWindowSize(
    const json& window,
    const char* key,
    unsigned int fallback,
    const std::string& path)
{
    const std::int64_t value = readValue<std::int64_t>(window, key, static_cast<std::int64_t>(fallback), path);
    if (value < 1 || value > kMaxWindowSize) {
        throw RendererError(
            ErrorCategory::Config,
            std::format("{}: window.{} must be between 1 and {}, got {}", path, key, kMaxWindowSize, value));
    }
    return static_cast<unsigned int>(value);
}
}

Settings loadSettings(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        throw RendererError(
            ErrorCategory::Config,
            "Could not open settings file: " + path);
    }

    Settings settings;  // starts from the defaults in Settings.hpp
    try {
        json data;
        file >> data;

        if (!data.is_object()) {
            throw RendererError(
                ErrorCategory::Config,
                path + ": the top level must be a JSON object");
        }

        // A missing "window" section keeps every default. A "window" that is
        // not an object is a mistake worth reporting.
        if (!data.contains("window"))
            return settings;
        const json& window = data.at("window");
        if (!window.is_object()) {
            throw RendererError(
                ErrorCategory::Config,
                path + ": \"window\" must be an object");
        }

        settings.width = readWindowSize(window, "width", settings.width, path);
        settings.height = readWindowSize(window, "height", settings.height, path);
        settings.title = readValue(window, "title", settings.title, path);
        settings.wireframe = readValue(window, "wireframe", settings.wireframe, path);
        settings.vSync = readValue(window, "vSync", settings.vSync, path);
        return settings;
    } catch (const json::exception& e) {
        throw RendererError(
            ErrorCategory::Config,
            path + ": " + e.what());
    }
}
