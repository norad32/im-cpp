#include "gui/preferences.h"
#include "gui/preferences_store.h"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <sstream>
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
    std::string value;
    while (file >> key >> value)
    {
        if (key == "logs_panel_visible")
        {
            preferences.showLogPanel = value == "1";
        }
        else if (key == "logs_autoscroll")
        {
            preferences.autoScroll = value == "1";
        }
        else if (key == "logs_panel_height")
        {
            std::istringstream valueStream(value);
            if (valueStream >> preferences.logPanelHeight)
            {
                preferences.logPanelHeight = std::max(0.0F, preferences.logPanelHeight);
            }
        }
        else if (key == "logs_panel_collapsed")
        {
            preferences.logPanelCollapsed = value == "1";
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
    file << "logs_panel_height " << preferences.logPanelHeight << '\n';
    file << "logs_panel_collapsed " << static_cast<int>(preferences.logPanelCollapsed) << '\n';
}

} // namespace im_cpp::gui
