#include "auto_animation/rigging/Retargeting.hpp"

#include <algorithm>
#include <cctype>
#include <unordered_map>

namespace auto_animation::rigging {
namespace {
std::string normalized(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    const std::string prefixes[] = {"mixamorig:", "mixamorig_", "bip01_", "bip01 "};
    for (const auto& prefix : prefixes)
        if (value.rfind(prefix, 0) == 0) { value.erase(0, prefix.size()); break; }
    for (char& c : value) if (c == ' ' || c == '-' || c == '.') c = '_';
    return value;
}
}

RetargetResult build_name_mapping(const Skeleton& source, const Skeleton& target) {
    RetargetResult result;
    std::unordered_map<std::string, std::int32_t> targets;
    for (std::size_t i = 0; i < target.bones.size(); ++i) targets[normalized(target.bones[i].name)] = static_cast<std::int32_t>(i);
    for (std::size_t i = 0; i < source.bones.size(); ++i) {
        const auto key = normalized(source.bones[i].name);
        const auto it = targets.find(key);
        if (it != targets.end()) result.mappings.push_back({static_cast<std::int32_t>(i), it->second});
        else result.warnings.push_back("No target bone matched source bone: " + source.bones[i].name);
    }
    return result;
}

animation::Pose retarget_pose(const Skeleton& source, const animation::Pose& source_pose,
                              const Skeleton& target, const RetargetResult& mapping,
                              const RetargetOptions& options) {
    (void)source;
    animation::Pose result;
    result.local_transforms.reserve(target.bones.size());
    for (const auto& bone : target.bones) result.local_transforms.push_back(bone.bind_local);
    for (const auto& map : mapping.mappings) {
        if (map.source_index < 0 || map.target_index < 0 ||
            static_cast<std::size_t>(map.source_index) >= source_pose.local_transforms.size() ||
            static_cast<std::size_t>(map.target_index) >= result.local_transforms.size()) continue;
        const auto& src = source_pose.local_transforms[static_cast<std::size_t>(map.source_index)];
        auto& dst = result.local_transforms[static_cast<std::size_t>(map.target_index)];
        dst.rotation = src.rotation;
        dst.scale = src.scale;
        if (options.transfer_root_translation && target.bones[static_cast<std::size_t>(map.target_index)].parent_index < 0) {
            dst.translation = {
                src.translation.x * options.translation_scale,
                src.translation.y * options.translation_scale,
                src.translation.z * options.translation_scale
            };
        }
    }
    return result;
}

} // namespace auto_animation::rigging
