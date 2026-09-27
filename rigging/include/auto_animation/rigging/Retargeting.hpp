#pragma once

#include "auto_animation/animation/Animation.hpp"
#include "auto_animation/rigging/Skeleton.hpp"
#include <string>
#include <vector>

namespace auto_animation::rigging {

struct BoneMapping { std::int32_t source_index = -1; std::int32_t target_index = -1; };

struct RetargetOptions {
    bool transfer_root_translation = true;
    float translation_scale = 1.0f;
};

struct RetargetResult {
    std::vector<BoneMapping> mappings;
    std::vector<std::string> warnings;
    [[nodiscard]] bool usable() const noexcept { return !mappings.empty(); }
};

[[nodiscard]] RetargetResult build_name_mapping(const Skeleton& source, const Skeleton& target);
[[nodiscard]] animation::Pose retarget_pose(const Skeleton& source, const animation::Pose& source_pose,
                                             const Skeleton& target, const RetargetResult& mapping,
                                             const RetargetOptions& options = {});

} // namespace auto_animation::rigging
