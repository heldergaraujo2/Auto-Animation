#pragma once
#include "auto_animation/anatomy/Markers.hpp"
#include "auto_animation/rigging/Skeleton.hpp"
#include <string>
#include <vector>
namespace auto_animation::rigging {
struct AutoRigResult {
    Skeleton skeleton;
    std::vector<std::string> warnings;
    [[nodiscard]] bool valid() const noexcept { return skeleton.validate().valid() && !skeleton.empty(); }
};
[[nodiscard]] AutoRigResult build_rig_from_markers(const anatomy::MarkerSet&);
} // namespace auto_animation::rigging
