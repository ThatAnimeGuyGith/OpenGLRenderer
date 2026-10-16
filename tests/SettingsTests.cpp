#include "Error.hpp"
#include "Settings.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace {
class TemporaryDirectory {
public:
    TemporaryDirectory()
    {
        static unsigned int sequence = 0;
        const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
        path_ = std::filesystem::temp_directory_path() /
            ("OpenGLRenderer-settings-tests-" + std::to_string(timestamp) + "-" + std::to_string(sequence++));
        std::filesystem::create_directories(path_);
    }

    ~TemporaryDirectory()
    {
        std::error_code error;
        std::filesystem::remove_all(path_, error);
    }

    std::filesystem::path writeSettings(std::string_view contents) const
    {
        const std::filesystem::path path = path_ / "settings.json";
        std::ofstream file(path);
        if (!file)
            throw std::runtime_error("Could not create temporary settings file");
        file << contents;
        if (!file)
            throw std::runtime_error("Could not write temporary settings file");
        return path;
    }

    std::filesystem::path missingSettingsPath() const
    {
        return path_ / "missing.json";
    }

private:
    std::filesystem::path path_;
};

class ScopedCLogCapture {
public:
    ScopedCLogCapture()
        : previousBuffer_(std::clog.rdbuf(output_.rdbuf()))
    {
    }

    ~ScopedCLogCapture()
    {
        std::clog.rdbuf(previousBuffer_);
    }

    std::string output() const
    {
        return output_.str();
    }

private:
    std::ostringstream output_;
    std::streambuf* previousBuffer_;
};

void require(bool condition, std::string_view message)
{
    if (!condition)
        throw std::runtime_error(std::string(message));
}

void requireConfigError(const std::filesystem::path& path, std::string_view expectedMessage)
{
    try {
        (void)loadSettings(path.string());
    } catch (const RendererError& error) {
        require(error.category() == ErrorCategory::Config, "Expected a Config error");
        require(
            std::string_view(error.what()).find(expectedMessage) != std::string_view::npos,
            "Config error did not include the expected detail");
        return;
    }
    throw std::runtime_error("Expected loadSettings to throw a Config error");
}

void testValidConfiguration()
{
    TemporaryDirectory directory;
    const auto path = directory.writeSettings(
        R"({"window":{"width":1280,"height":720,"title":"Test Window","wireframe":true,"vSync":false}})");

    const Settings settings = loadSettings(path.string());
    require(settings.width == 1280, "Valid width was not loaded");
    require(settings.height == 720, "Valid height was not loaded");
    require(settings.title == "Test Window", "Valid title was not loaded");
    require(settings.wireframe, "Valid wireframe setting was not loaded");
    require(!settings.vSync, "Valid vSync setting was not loaded");
}

void testMissingFileIsReported()
{
    TemporaryDirectory directory;
    requireConfigError(directory.missingSettingsPath(), "Could not open settings file");
}

void testMissingWindowSectionKeepsDefaults()
{
    TemporaryDirectory directory;
    const auto path = directory.writeSettings(R"({"other":true})");

    ScopedCLogCapture logCapture;
    const Settings settings = loadSettings(path.string());
    require(settings.width == 800, "Missing window section changed the default width");
    require(settings.height == 600, "Missing window section changed the default height");
    require(settings.title == "OpenGL Renderer", "Missing window section changed the default title");
    require(!settings.wireframe, "Missing window section changed the default wireframe setting");
    require(settings.vSync, "Missing window section changed the default vSync setting");
    require(logCapture.output().empty(), "Missing window section unexpectedly logged missing-key warnings");
}

void testMissingKeysKeepDefaults()
{
    TemporaryDirectory directory;
    const auto path = directory.writeSettings(R"({"window":{"width":1024,"wireframe":true}})");

    ScopedCLogCapture logCapture;
    const Settings settings = loadSettings(path.string());
    require(settings.width == 1024, "Present width was not loaded");
    require(settings.height == 600, "Missing height did not keep its default");
    require(settings.title == "OpenGL Renderer", "Missing title did not keep its default");
    require(settings.wireframe, "Present wireframe setting was not loaded");
    require(settings.vSync, "Missing vSync did not keep its default");
    const std::string warnings = logCapture.output();
    require(warnings.find(path.string()) != std::string::npos, "Missing-key warning omitted the settings path");
    require(warnings.find("window.height is missing") != std::string::npos, "Missing-key warning omitted the key name");
    require(warnings.find("window.title is missing") != std::string::npos, "Missing-key warning omitted the title key");
    require(warnings.find("window.vSync is missing") != std::string::npos, "Missing-key warning omitted the vSync key");
}

void testMalformedJsonIsReported()
{
    TemporaryDirectory directory;
    const auto path = directory.writeSettings(R"({"window":)");
    requireConfigError(path, path.string());
}

void testInvalidObjectShapesAreReported()
{
    TemporaryDirectory directory;
    const auto topLevelPath = directory.writeSettings(R"([])");
    requireConfigError(topLevelPath, "top level must be a JSON object");

    const auto windowPath = directory.writeSettings(R"({"window":[]})");
    requireConfigError(windowPath, "window\" must be an object");
}

void testIncorrectValueTypesAreReported()
{
    TemporaryDirectory directory;
    const std::vector<std::pair<std::string, std::string>> invalidValues{
        {R"({"window":{"width":"800"}})", "window.width has the wrong type"},
        {R"({"window":{"height":600.5}})", "window.height has the wrong type"},
        {R"({"window":{"title":false}})", "window.title has the wrong type"},
        {R"({"window":{"wireframe":"false"}})", "window.wireframe has the wrong type"},
        {R"({"window":{"vSync":1}})", "window.vSync has the wrong type"}
    };

    for (const auto& [contents, expectedMessage] : invalidValues) {
        const auto path = directory.writeSettings(contents);
        requireConfigError(path, expectedMessage);
    }
}

void testWindowSizeBoundaries()
{
    TemporaryDirectory directory;
    const auto path = directory.writeSettings(R"({"window":{"width":1,"height":16384}})");

    const Settings settings = loadSettings(path.string());
    require(settings.width == 1, "Minimum window width was rejected");
    require(settings.height == 16384, "Maximum window height was rejected");
}

void testOutOfRangeWindowSizesAreReported()
{
    TemporaryDirectory directory;
    const std::vector<std::string> invalidValues{"0", "-1", "16385", "18446744073709551615"};

    for (const auto& value : invalidValues) {
        const auto path = directory.writeSettings("{\"window\":{\"width\":" + value + "}}");
        requireConfigError(path, "window.width must be between 1 and 16384");
    }
}

void testWindowSectionMustBeAnObject()
{
    TemporaryDirectory directory;
    const auto path = directory.writeSettings(R"({"window":null})");
    requireConfigError(path, "window\" must be an object");
}

}

int main()
{
    const std::vector<std::pair<std::string_view, void (*)()>> tests{
        {"valid configuration", testValidConfiguration},
        {"missing file", testMissingFileIsReported},
        {"missing window section", testMissingWindowSectionKeepsDefaults},
        {"missing keys", testMissingKeysKeepDefaults},
        {"malformed JSON", testMalformedJsonIsReported},
        {"invalid object shapes", testInvalidObjectShapesAreReported},
        {"incorrect value types", testIncorrectValueTypesAreReported},
        {"window size boundaries", testWindowSizeBoundaries},
        {"out-of-range window sizes", testOutOfRangeWindowSizesAreReported},
        {"window section type", testWindowSectionMustBeAnObject}
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

    std::cout << tests.size() - failures << '/' << tests.size() << " tests passed\n";
    return failures == 0 ? 0 : 1;
}