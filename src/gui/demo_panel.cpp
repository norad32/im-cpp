#include "gui/demo_panel.h"
#include "gui/preferences.h"
#include <imgui.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>

namespace im_cpp::gui
{
namespace
{

constexpr float initialDemoPanelWidth = 520.0F;
constexpr float initialDemoPanelHeight = 250.0F;

}

void drawDemoPanel(spdlog::logger & logger,
                   Preferences & preferences,
                   bool & shouldClose,
                   DemoPanelState & state)
{
    const ImGuiViewport * viewport = ImGui::GetMainViewport();
    if (state.width <= 0.0F)
    {
        state.width = initialDemoPanelWidth;
        state.height = initialDemoPanelHeight;
    }
    ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(state.width, state.height), ImGuiCond_FirstUseEver);
    const bool windowContentsVisible = ImGui::Begin("im-cpp");
    state.width = ImGui::GetWindowSize().x;
    state.height = ImGui::GetWindowSize().y;
    if (windowContentsVisible)
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
            logger.log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
                       spdlog::level::trace,
                       "Generated a trace message");
            logger.log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
                       spdlog::level::debug,
                       "Generated a debug message");
            logger.log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
                       spdlog::level::info,
                       "Generated an information message");
            logger.log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
                       spdlog::level::warn,
                       "Generated a warning message");
            logger.log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
                       spdlog::level::err,
                       "Generated an error message");
        }
        ImGui::SameLine();
        if (ImGui::Button("Close"))
        {
            shouldClose = true;
        }
    }
    ImGui::End();
}

}
