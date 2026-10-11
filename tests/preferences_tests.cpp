#include "gui/preferences.h"
#include "gui/preferences_store.h"
#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <filesystem>
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
    const EnvironmentVariableGuard configDirectoryGuard("XDG_CONFIG_HOME", configDirectory);

    const auto preferences = im_cpp::gui::loadPreferences();

    REQUIRE_FALSE(preferences.showLogPanel);
    REQUIRE(preferences.autoScroll);
    std::filesystem::remove_all(configDirectory);
}

TEST_CASE("savePreferences_Should_RoundTripSettings")
{
    const auto configDirectory = std::filesystem::temp_directory_path() / "im-cpp-pref-roundtrip";
    std::filesystem::remove_all(configDirectory);
    const EnvironmentVariableGuard configDirectoryGuard("XDG_CONFIG_HOME", configDirectory);
    const im_cpp::gui::Preferences expected{.showLogPanel = true, .autoScroll = false};

    im_cpp::gui::savePreferences(expected);
    const auto actual = im_cpp::gui::loadPreferences();

    REQUIRE(actual.showLogPanel);
    REQUIRE_FALSE(actual.autoScroll);
    std::filesystem::remove_all(configDirectory);
}
