#pragma once

#include "auto_animation/animation/Animation.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace auto_animation::rigging {

enum class MirrorAxis : std::uint8_t { X, Y, Z };
enum class RemovePolicy : std::uint8_t { RejectWithChildren, ReparentChildren };

struct Bone {
    std::string name;
    std::int32_t parent_index = -1;
    animation::Transform bind_local{};
    animation::Transform local_pose{};
    animation::Vec3 pivot{};
    std::string group;
    bool editable = true;
    std::array<float, 16> inverse_bind_matrix{};
};

struct SkeletonIssue {
    enum class Severity : std::uint8_t { Warning, Error };
    Severity severity = Severity::Error;
    std::string message;
};

struct SkeletonValidationResult {
    std::vector<SkeletonIssue> issues;
    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] bool has_warnings() const noexcept;
};

struct Skeleton {
    std::string name;
    std::vector<Bone> bones;
    std::int32_t selected_bone = -1;

    [[nodiscard]] bool empty() const noexcept { return bones.empty(); }
    [[nodiscard]] bool has_bone(std::int32_t index) const noexcept;
    [[nodiscard]] std::int32_t find_bone(const std::string& name) const noexcept;

    std::int32_t add_bone(std::string name, std::int32_t parent_index,
                          const animation::Transform& local_bind = {});
    bool rename_bone(std::int32_t index, std::string new_name);
    bool set_parent(std::int32_t index, std::int32_t new_parent);
    bool remove_bone(std::int32_t index, RemovePolicy policy = RemovePolicy::RejectWithChildren);
    bool set_group(std::int32_t index, std::string group);
    bool set_pose_transform(std::int32_t index, const animation::Transform& transform);
    bool reset_pose_to_bind(std::int32_t index);
    bool mirror_pose(std::int32_t index, MirrorAxis axis);
    bool mirror_bone_to_sibling(std::int32_t source_index, std::int32_t target_index, MirrorAxis axis);

    [[nodiscard]] animation::Transform world_transform(
        std::int32_t index, bool bind_pose = false) const;
    [[nodiscard]] SkeletonValidationResult validate() const;
};

[[nodiscard]] animation::Vec3 mirror_vector(const animation::Vec3&, MirrorAxis) noexcept;
[[nodiscard]] animation::Quat mirror_rotation(const animation::Quat&, MirrorAxis) noexcept;

} // namespace auto_animation::rigging
