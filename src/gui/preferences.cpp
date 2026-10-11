#include "gui/preferences.h"
#include "gui/preferences_store.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <string_view>

namespace im_cpp::gui
{
namespace
{

[[nodiscard]] std::filesystem::path preferencesPath()
{
    if (const char * configDirectory = std::getenv("XDG_CONFIG_HOME");
        configDirectory != nullptr && !std::string_view(configDirectory).empty())
    {
        return std::filesystem::path(configDirectory) / "im-cpp" / "imgui.ini";
    }
    if (const char * homeDirectory = std::getenv("HOME"); homeDirectory != nullptr && !std::string_view(homeDirectory).empty())
    {
        return std::filesystem::path(homeDirectory) / ".config" / "im-cpp" / "imgui.ini";
    }
    return std::filesystem::temp_directory_path() / "im-cpp" / "imgui.ini";
}

} // namespace

Preferences loadPreferences()
{
    Preferences preferences;
    std::ifstream file(preferencesPath());
    std::string key;
    int value = 0;
    while (file >> key >> value)
    {
        if (key == "logs_panel_visible")
        {
            preferences.showLogPanel = value != 0;
        }
        else if (key == "logs_autoscroll")
        {
            preferences.autoScroll = value != 0;
        }
    }
    return preferences;
}

void savePreferences(const Preferences & preferences)
{
    const auto path = preferencesPath();
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path, std::ios::trunc);
    file << "logs_panel_visible " << static_cast<int>(preferences.showLogPanel) << '\n';
    file << "logs_autoscroll " << static_cast<int>(preferences.autoScroll) << '\n';
}

} // namespace im_cpp::gui
