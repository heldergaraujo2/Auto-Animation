#pragma once

#include "auto_animation/anatomy/Markers.hpp"

#include <string>

namespace auto_animation::anatomy {

[[nodiscard]] bool save_marker_profile(const MarkerSet& markers,
                                       const std::string& path,
                                       std::string& error);

[[nodiscard]] bool load_marker_profile(const std::string& path,
                                       MarkerSet& out,
                                       std::string& error);

} // namespace auto_animation::anatomy
