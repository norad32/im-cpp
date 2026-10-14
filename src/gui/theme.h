#pragma once

#include <spdlog/common.h>

namespace im_cpp::gui::theme
{

struct Color
{
    float red;
    float green;
    float blue;
    float alpha;
};

inline constexpr float maximumColorChannel = 255.0F;
inline constexpr Color traceLog{
    .red = 166.0F / maximumColorChannel,
    .green = 166.0F / maximumColorChannel,
    .blue = 166.0F / maximumColorChannel,
    .alpha = 1.0F,
};
inline constexpr Color debugLog{
    .red = 140.0F / maximumColorChannel,
    .green = 191.0F / maximumColorChannel,
    .blue = 1.0F,
    .alpha = 1.0F,
};
inline constexpr Color infoLog{
    .red = 166.0F / maximumColorChannel,
    .green = 217.0F / maximumColorChannel,
    .blue = 166.0F / maximumColorChannel,
    .alpha = 1.0F,
};
inline constexpr Color warningLog{
    .red = 1.0F,
    .green = 204.0F / maximumColorChannel,
    .blue = 89.0F / maximumColorChannel,
    .alpha = 1.0F,
};
inline constexpr Color errorLog{
    .red = 1.0F,
    .green = 102.0F / maximumColorChannel,
    .blue = 102.0F / maximumColorChannel,
    .alpha = 1.0F,
};
inline constexpr Color neutralLog{
    .red = 217.0F / maximumColorChannel,
    .green = 217.0F / maximumColorChannel,
    .blue = 217.0F / maximumColorChannel,
    .alpha = 1.0F,
};
inline constexpr Color background{.red = 0.08F, .green = 0.08F, .blue = 0.09F, .alpha = 1.0F};

[[nodiscard]] inline Color logLevelColor(spdlog::level::level_enum level) noexcept
{
    switch (level)
    {
        case spdlog::level::trace :
            return traceLog;
        case spdlog::level::debug :
            return debugLog;
        case spdlog::level::info :
            return infoLog;
        case spdlog::level::warn :
            return warningLog;
        case spdlog::level::err :
        case spdlog::level::critical :
            return errorLog;
        default :
            return neutralLog;
    }
}

} // namespace im_cpp::gui::theme
