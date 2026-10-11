#pragma once

#include "gui/preferences.h"

namespace im_cpp::gui
{

[[nodiscard]] Preferences loadPreferences();
void savePreferences(const Preferences & preferences);

} // namespace im_cpp::gui
