#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace im_cpp::core
{

enum class LogLevel : std::uint8_t
{
    notSet,
    debug,
    info,
    warning,
    error,
    critical,
};

[[nodiscard]] std::optional<LogLevel> parseLogLevel(std::string_view value);
[[nodiscard]] std::string_view toString(LogLevel level) noexcept;

} // namespace im_cpp::core
