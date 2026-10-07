#pragma once

#include <string_view>

namespace im_cpp::core {

[[nodiscard]] constexpr std::string_view application_name() noexcept {
    return "im-cpp";
}

} // namespace im_cpp::core
