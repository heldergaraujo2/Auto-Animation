#pragma once

#include "auto_animation/animation/Animation.hpp"
#include <vector>

namespace auto_animation::animation {

[[nodiscard]] Transform blend_transform(const Transform&, const Transform&, float weight) noexcept;
[[nodiscard]] Pose blend_pose(const Pose&, const Pose&, float weight) noexcept;
[[nodiscard]] Pose blend_pose_masked(const Pose&, const Pose&, const std::vector<float>& weights) noexcept;
[[nodiscard]] Pose apply_additive_pose(const Pose&, const Pose&, float weight = 1.0f,
                                       const std::vector<float>* weights = nullptr) noexcept;

} // namespace auto_animation::animation
