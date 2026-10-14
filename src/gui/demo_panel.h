#pragma once

#include "gui/preferences.h"
#include <imgui.h>
#include <spdlog/logger.h>

namespace im_cpp::gui
{

struct DemoPanelState
{
    float width = 0.0F;
    float height = 0.0F;
};

void drawDemoPanel(spdlog::logger & logger,
                   Preferences & preferences,
                   bool & shouldClose,
                   DemoPanelState & state);

}
