#include "gui/application.h"
#include "core/gui_log_sink.h"
#include "gui/preferences.h"
#include "gui/preferences_store.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <utility>
#include <vector>

namespace im_cpp::gui
{
namespace
{

constexpr float minimumLogPanelHeight = 96.0F;
constexpr float initialMainWindowWidth = 520.0F;
constexpr float initialMainWindowHeight = 250.0F;
constexpr int initialApplicationWindowWidth = 1280;
constexpr int initialApplicationWindowHeight = 800;
constexpr int mutedGrayChannel = 166;
constexpr int debugRedChannel = 140;
constexpr int debugGreenChannel = 191;
constexpr int maximumColorChannel = 255;
constexpr int infoGreenChannel = 217;
constexpr int warningGreenChannel = 204;
constexpr int warningBlueChannel = 89;
constexpr int errorGreenChannel = 102;
constexpr int neutralGrayChannel = 217;
constexpr float backgroundRed = 0.08F;
constexpr float backgroundGreen = 0.08F;
constexpr float backgroundBlue = 0.09F;
constexpr float opaqueAlpha = 1.0F;
constexpr float logPanelHeightRatio = 0.33F;
constexpr float bottomScrollPosition = 1.0F;
constexpr std::size_t maxDisplayedEntries = core::defaultGuiLogCapacity;

[[nodiscard]] ImVec4 colorFromChannels(int red, int green, int blue)
{
    constexpr float channelMaximum = static_cast<float>(maximumColorChannel);
    return {static_cast<float>(red) / channelMaximum,
            static_cast<float>(green) / channelMaximum,
            static_cast<float>(blue) / channelMaximum,
            opaqueAlpha};
}

[[nodiscard]] ImVec4 levelColor(spdlog::level::level_enum level)
{
    switch (level)
    {
        case spdlog::level::trace :
            return colorFromChannels(mutedGrayChannel, mutedGrayChannel, mutedGrayChannel);
        case spdlog::level::debug :
            return colorFromChannels(debugRedChannel, debugGreenChannel, maximumColorChannel);
        case spdlog::level::info :
            return colorFromChannels(mutedGrayChannel, infoGreenChannel, mutedGrayChannel);
        case spdlog::level::warn :
            return colorFromChannels(maximumColorChannel, warningGreenChannel, warningBlueChannel);
        case spdlog::level::err :
        case spdlog::level::critical :
            return colorFromChannels(maximumColorChannel, errorGreenChannel, errorGreenChannel);
        default :
            return colorFromChannels(neutralGrayChannel, neutralGrayChannel, neutralGrayChannel);
    }
}

class GlfwSession
{
public:
    GlfwSession(const GlfwSession &) = delete;
    GlfwSession & operator=(const GlfwSession &) = delete;
    GlfwSession(GlfwSession &&) = delete;
    GlfwSession & operator=(GlfwSession &&) = delete;

    GlfwSession()
    {
        if (glfwInit() == GLFW_FALSE)
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    }

    ~GlfwSession() { glfwTerminate(); }
};

struct WindowDeleter
{
    void operator()(GLFWwindow * window) const noexcept
    {
        if (window != nullptr)
        {
            glfwDestroyWindow(window);
        }
    }
};

class ImGuiContext
{
public:
    ImGuiContext()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
    }

    ~ImGuiContext() { ImGui::DestroyContext(); }

    ImGuiContext(const ImGuiContext &) = delete;
    ImGuiContext & operator=(const ImGuiContext &) = delete;
    ImGuiContext(ImGuiContext &&) = delete;
    ImGuiContext & operator=(ImGuiContext &&) = delete;
};

class GlfwBackend
{
public:
    explicit GlfwBackend(GLFWwindow * window) : initialized_(ImGui_ImplGlfw_InitForOpenGL(window, true))
    {
        if (!initialized_)
        {
            throw std::runtime_error("Failed to initialize ImGui GLFW backend");
        }
    }

    ~GlfwBackend()
    {
        if (initialized_)
        {
            ImGui_ImplGlfw_Shutdown();
        }
    }

    GlfwBackend(const GlfwBackend &) = delete;
    GlfwBackend & operator=(const GlfwBackend &) = delete;
    GlfwBackend(GlfwBackend &&) = delete;
    GlfwBackend & operator=(GlfwBackend &&) = delete;

private:
    bool initialized_ = false;
};

class OpenGLBackend
{
public:
    OpenGLBackend() : initialized_(ImGui_ImplOpenGL3_Init("#version 330"))
    {
        if (!initialized_)
        {
            throw std::runtime_error("Failed to initialize ImGui OpenGL backend");
        }
    }

    ~OpenGLBackend()
    {
        if (initialized_)
        {
            ImGui_ImplOpenGL3_Shutdown();
        }
    }

    OpenGLBackend(const OpenGLBackend &) = delete;
    OpenGLBackend & operator=(const OpenGLBackend &) = delete;
    OpenGLBackend(OpenGLBackend &&) = delete;
    OpenGLBackend & operator=(OpenGLBackend &&) = delete;

private:
    bool initialized_ = false;
};

void appendEntries(std::vector<core::GuiLogEntry> & entries, const std::vector<core::GuiLogEntry> & newEntries)
{
    for (const auto & entry : newEntries)
    {
        if (entries.size() == maxDisplayedEntries)
        {
            entries.erase(entries.begin());
        }
        entries.push_back(entry);
    }
}

void drawMainWindow(spdlog::logger & logger, Preferences & preferences, bool & shouldClose)
{
    ImGui::SetNextWindowSize(ImVec2(initialMainWindowWidth, initialMainWindowHeight), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("im-cpp"))
    {
        ImGui::Text("Hello I'm C++!");
        if (ImGui::Button(preferences.showLogPanel ? "Hide Logs (§)" : "Show Logs (§)"))
        {
            preferences.showLogPanel = !preferences.showLogPanel;
            savePreferences(preferences);
        }
        ImGui::SameLine();
        if (ImGui::Button("Generate Test Logs"))
        {
            logger.log(
                spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::debug, "Generated a debug message");
            logger.log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
                       spdlog::level::info,
                       "Generated an information message");
            logger.log(
                spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::warn, "Generated a warning message");
            logger.log(
                spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::err, "Generated an error message");
        }
        ImGui::SameLine();
        if (ImGui::Button("Close"))
        {
            shouldClose = true;
        }
    }
    ImGui::End();
}

void drawLogEntries(const std::vector<core::GuiLogEntry> & entries,
                    const Preferences & preferences,
                    bool newEntriesWereAdded,
                    bool & followScroll)
{
    ImGui::BeginChild("LogEntries", ImVec2(0.0F, 0.0F), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
    const bool wasAtBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY();
    for (const auto & entry : entries)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, levelColor(entry.level));
        ImGui::TextWrapped("%s", entry.formattedMessage.c_str());
        ImGui::PopStyleColor();
    }
    if (preferences.autoScroll && (newEntriesWereAdded || (followScroll && wasAtBottom)))
    {
        ImGui::SetScrollHereY(bottomScrollPosition);
    }
    followScroll = preferences.autoScroll;
    ImGui::EndChild();
}

void drawLogPanelContents(core::GuiLogSink & logSink,
                          Preferences & preferences,
                          std::vector<core::GuiLogEntry> & entries,
                          bool & followScroll)
{
    const auto newEntries = logSink.drain();
    appendEntries(entries, newEntries);
    const bool newEntriesWereAdded = !newEntries.empty();
    if (ImGui::Checkbox("Autoscroll", &preferences.autoScroll))
    {
        savePreferences(preferences);
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
    {
        entries.clear();
        logSink.clear();
    }
    ImGui::Separator();
    drawLogEntries(entries, preferences, newEntriesWereAdded, followScroll);
}

void drawLogPanel(core::GuiLogSink & logSink,
                  Preferences & preferences,
                  std::vector<core::GuiLogEntry> & entries,
                  bool & followScroll)
{
    if (!preferences.showLogPanel)
    {
        return;
    }
    const ImGuiViewport * viewport = ImGui::GetMainViewport();
    const bool wasVisible = preferences.showLogPanel;
    const float maximumPanelHeight = std::max(minimumLogPanelHeight, viewport->WorkSize.y * 0.8F);
    if (preferences.logPanelHeight <= 0.0F)
    {
        preferences.logPanelHeight =
            std::clamp(viewport->WorkSize.y * logPanelHeightRatio, minimumLogPanelHeight, maximumPanelHeight);
    }
    preferences.logPanelHeight = std::clamp(preferences.logPanelHeight, minimumLogPanelHeight, maximumPanelHeight);
    const float collapsedPanelHeight = ImGui::GetFrameHeight();
    const float panelHeight = preferences.logPanelCollapsed ? collapsedPanelHeight : preferences.logPanelHeight;
    const float minimumPanelHeight = preferences.logPanelCollapsed ? collapsedPanelHeight : minimumLogPanelHeight;
    ImGui::SetNextWindowSizeConstraints(ImVec2(viewport->WorkSize.x, minimumPanelHeight),
                                        ImVec2(viewport->WorkSize.x, maximumPanelHeight));
    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - panelHeight),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, preferences.logPanelHeight), ImGuiCond_Appearing);
    ImGui::SetNextWindowCollapsed(preferences.logPanelCollapsed, ImGuiCond_Appearing);
    constexpr auto panelFlags = static_cast<ImGuiWindowFlags>(static_cast<unsigned int>(ImGuiWindowFlags_NoMove) |
                                                              static_cast<unsigned int>(ImGuiWindowFlags_NoSavedSettings));
    const bool windowContentsVisible = ImGui::Begin("Logs", &preferences.showLogPanel, panelFlags);
    const bool isCollapsed = ImGui::IsWindowCollapsed();
    if (isCollapsed != preferences.logPanelCollapsed)
    {
        preferences.logPanelCollapsed = isCollapsed;
        savePreferences(preferences);
    }
    if (windowContentsVisible && !isCollapsed)
    {
        drawLogPanelContents(logSink, preferences, entries, followScroll);
        const float updatedPanelHeight = ImGui::GetWindowSize().y;
        if (std::abs(updatedPanelHeight - preferences.logPanelHeight) > 1.0F)
        {
            preferences.logPanelHeight = updatedPanelHeight;
            savePreferences(preferences);
        }
    }
    ImGui::End();
    if (preferences.showLogPanel != wasVisible)
    {
        savePreferences(preferences);
    }
}

void renderFrame(GLFWwindow * window)
{
    ImGui::Render();
    int displayWidth = 0;
    int displayHeight = 0;
    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
    glViewport(0, 0, displayWidth, displayHeight);
    glClearColor(backgroundRed, backgroundGreen, backgroundBlue, opaqueAlpha);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void drawFrame(GLFWwindow * window,
               spdlog::logger & logger,
               core::GuiLogSink & logSink,
               Preferences & preferences,
               bool & shouldClose,
               std::vector<core::GuiLogEntry> & entries,
               bool & followScroll)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent, false) && !ImGui::GetIO().WantTextInput &&
        !ImGui::GetIO().WantCaptureKeyboard)
    {
        preferences.showLogPanel = !preferences.showLogPanel;
        savePreferences(preferences);
    }
    drawMainWindow(logger, preferences, shouldClose);
    drawLogPanel(logSink, preferences, entries, followScroll);
    renderFrame(window);
}

} // namespace

Application::Application(std::shared_ptr<spdlog::logger> logger, std::shared_ptr<core::GuiLogSink> logSink) :
    logger_(std::move(logger)), logSink_(std::move(logSink)), preferences_(loadPreferences())
{
}

int Application::run()
{
    SPDLOG_LOGGER_DEBUG(logger_, "GUI starting");
    const GlfwSession glfwSession;
    const std::unique_ptr<GLFWwindow, WindowDeleter> window{
        glfwCreateWindow(initialApplicationWindowWidth, initialApplicationWindowHeight, "im-cpp", nullptr, nullptr)};
    if (!window)
    {
        throw std::runtime_error("Failed to create GUI window");
    }

    glfwMakeContextCurrent(window.get());
    glfwSwapInterval(1);
    const ImGuiContext guiContext;
    ImGui::StyleColorsDark();
    const GlfwBackend glfwBackend(window.get());
    const OpenGLBackend openGLBackend;

    ImGui::GetIO().IniFilename = nullptr;
    std::vector<core::GuiLogEntry> entries;
    bool followScroll = true;
    while (glfwWindowShouldClose(window.get()) == GLFW_FALSE && !shouldClose_)
    {
        glfwPollEvents();
        drawFrame(window.get(), *logger_, *logSink_, preferences_, shouldClose_, entries, followScroll);
    }

    SPDLOG_LOGGER_INFO(logger_, "GUI closed");
    return 0;
}

} // namespace im_cpp::gui
