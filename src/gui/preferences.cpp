#include "gui/preferences.h"
#include "gui/preferences_store.h"
#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <unistd.h>

namespace im_cpp::gui
{
namespace
{

[[nodiscard]] std::optional<std::filesystem::path> preferencesPath()
{
    if (const char * configDirectory = std::getenv("XDG_CONFIG_HOME");
        configDirectory != nullptr && std::filesystem::path(configDirectory).is_absolute())
    {
        return std::filesystem::path(configDirectory) / "im-cpp" / "preferences.conf";
    }
    if (const char * homeDirectory = std::getenv("HOME"); homeDirectory != nullptr && !std::string_view(homeDirectory).empty())
    {
        return std::filesystem::path(homeDirectory) / ".config" / "im-cpp" / "preferences.conf";
    }
    return std::nullopt;
}

class TemporaryPreferencesFile
{
public:
    explicit TemporaryPreferencesFile(const std::filesystem::path & path) : path_(path.string() + ".XXXXXX")
    {
        descriptor_ = mkstemp(path_.data());
        if (descriptor_ == -1)
        {
            throw std::system_error(errno, std::generic_category(), "Cannot create temporary preferences file");
        }
    }

    ~TemporaryPreferencesFile()
    {
        if (descriptor_ != -1)
        {
            close(descriptor_);
        }
        if (!committed_)
        {
            unlink(path_.c_str());
        }
    }

    TemporaryPreferencesFile(const TemporaryPreferencesFile &) = delete;
    TemporaryPreferencesFile & operator=(const TemporaryPreferencesFile &) = delete;

    void write(std::string_view contents)
    {
        while (!contents.empty())
        {
            const ssize_t bytesWritten = ::write(descriptor_, contents.data(), contents.size());
            if (bytesWritten == -1)
            {
                if (errno == EINTR)
                {
                    continue;
                }
                throw std::system_error(errno, std::generic_category(), "Cannot write preferences file");
            }
            contents.remove_prefix(static_cast<std::size_t>(bytesWritten));
        }
    }

    void commit(const std::filesystem::path & destination)
    {
        if (fsync(descriptor_) == -1)
        {
            throw std::system_error(errno, std::generic_category(), "Cannot sync preferences file");
        }
        if (close(descriptor_) == -1)
        {
            descriptor_ = -1;
            throw std::system_error(errno, std::generic_category(), "Cannot close preferences file");
        }
        descriptor_ = -1;
        if (rename(path_.c_str(), destination.c_str()) == -1)
        {
            throw std::system_error(errno, std::generic_category(), "Cannot replace preferences file");
        }
        committed_ = true;
    }

private:
    std::string path_;
    int descriptor_ = -1;
    bool committed_ = false;
};

} // namespace

Preferences loadPreferences()
{
    Preferences preferences;
    const auto path = preferencesPath();
    if (!path)
    {
        return preferences;
    }
    std::ifstream file(*path);
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
    if (!path)
    {
        return;
    }
    std::filesystem::create_directories(path->parent_path());
    std::ostringstream contents;
    contents << "logs_panel_visible " << static_cast<int>(preferences.showLogPanel) << '\n';
    contents << "logs_autoscroll " << static_cast<int>(preferences.autoScroll) << '\n';
    contents << "logs_panel_height " << preferences.logPanelHeight << '\n';
    contents << "logs_panel_collapsed " << static_cast<int>(preferences.logPanelCollapsed) << '\n';
    TemporaryPreferencesFile temporaryFile(*path);
    temporaryFile.write(contents.str());
    temporaryFile.commit(*path);
}

} // namespace im_cpp::gui
