#include "gui/preferences.h"
#include "gui/preferences_store.h"
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

namespace
{

constexpr int overwriteExisting = 1;

class EnvironmentVariableGuard
{
public:
    EnvironmentVariableGuard(const char * name, const std::filesystem::path & value) : name_(name)
    {
        if (const char * previousValue = std::getenv(name_); previousValue != nullptr)
        {
            previousValue_.emplace(previousValue);
        }
        setenv(name_, value.c_str(), overwriteExisting);
    }

    ~EnvironmentVariableGuard()
    {
        if (previousValue_)
        {
            setenv(name_, previousValue_->c_str(), overwriteExisting);
        }
        else
        {
            unsetenv(name_);
        }
    }

    EnvironmentVariableGuard(const EnvironmentVariableGuard &) = delete;
    EnvironmentVariableGuard & operator=(const EnvironmentVariableGuard &) = delete;
    EnvironmentVariableGuard(EnvironmentVariableGuard &&) = delete;
    EnvironmentVariableGuard & operator=(EnvironmentVariableGuard &&) = delete;

private:
    const char * name_;
    std::optional<std::string> previousValue_;
};

} // namespace

TEST_CASE("loadPreferences_Should_UseDefaultsWhenFileIsMissing")
{
    const auto configDirectory = std::filesystem::temp_directory_path() / "im-cpp-pref-defaults";
    std::filesystem::remove_all(configDirectory);
    EnvironmentVariableGuard configDirectoryGuard("XDG_CONFIG_HOME", configDirectory);

    const auto preferences = im_cpp::gui::loadPreferences();

    REQUIRE_FALSE(preferences.showLogPanel);
    REQUIRE(preferences.autoScroll);
    REQUIRE_FALSE(preferences.logPanelCollapsed);
    REQUIRE(std::abs(preferences.logPanelHeight) < 0.001F);
    std::filesystem::remove_all(configDirectory);
}

TEST_CASE("savePreferences_Should_RoundTripSettings")
{
    const auto configDirectory = std::filesystem::temp_directory_path() / "im-cpp-pref-roundtrip";
    std::filesystem::remove_all(configDirectory);
    EnvironmentVariableGuard configDirectoryGuard("XDG_CONFIG_HOME", configDirectory);
    const im_cpp::gui::Preferences expected{
        .showLogPanel = true, .autoScroll = false, .logPanelCollapsed = true, .logPanelHeight = 268.5F};

    im_cpp::gui::savePreferences(expected);
    const auto actual = im_cpp::gui::loadPreferences();

    REQUIRE(std::filesystem::is_regular_file(configDirectory / "im-cpp" / "preferences.conf"));
    REQUIRE(actual.showLogPanel);
    REQUIRE_FALSE(actual.autoScroll);
    REQUIRE(actual.logPanelCollapsed);
    REQUIRE(std::abs(actual.logPanelHeight - 268.5F) < 0.001F);
    std::filesystem::remove_all(configDirectory);
}

TEST_CASE("savePreferences_Should_ReplaceSymlinkWithoutWritingToItsTarget")
{
    const auto configDirectory = std::filesystem::temp_directory_path() / "im-cpp-pref-symlink";
    const auto externalFile = std::filesystem::temp_directory_path() / "im-cpp-pref-symlink-target";
    std::filesystem::remove_all(configDirectory);
    std::filesystem::remove(externalFile);
    EnvironmentVariableGuard configDirectoryGuard("XDG_CONFIG_HOME", configDirectory);
    const auto preferencesDirectory = configDirectory / "im-cpp";
    std::filesystem::create_directories(preferencesDirectory);
    {
        std::ofstream target(externalFile);
        target << "preserve this file\n";
    }
    std::filesystem::create_symlink(externalFile, preferencesDirectory / "preferences.conf");

    im_cpp::gui::savePreferences(im_cpp::gui::Preferences{.showLogPanel = true});

    std::ifstream target(externalFile);
    const std::string targetContents((std::istreambuf_iterator<char>(target)), std::istreambuf_iterator<char>());
    REQUIRE(targetContents == "preserve this file\n");
    REQUIRE_FALSE(std::filesystem::is_symlink(preferencesDirectory / "preferences.conf"));
    std::filesystem::remove_all(configDirectory);
    std::filesystem::remove(externalFile);
}

TEST_CASE("savePreferences_Should_NotUseRelativeXdgConfigHome")
{
    const auto configDirectory = std::filesystem::temp_directory_path() / "im-cpp-pref-relative";
    std::filesystem::remove_all(configDirectory);
    EnvironmentVariableGuard configDirectoryGuard("XDG_CONFIG_HOME", "relative-config-home");
    EnvironmentVariableGuard homeDirectoryGuard("HOME", configDirectory);

    im_cpp::gui::savePreferences(im_cpp::gui::Preferences{.showLogPanel = true});

    REQUIRE(std::filesystem::is_regular_file(configDirectory / ".config" / "im-cpp" / "preferences.conf"));
    std::filesystem::remove_all(configDirectory);
}
