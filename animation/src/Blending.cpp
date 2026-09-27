#include "auto_animation/animation/Blending.hpp"

#include <algorithm>

namespace auto_animation::animation {
namespace {
Quat multiply(const Quat& a, const Quat& b) noexcept {
    return {
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z
    };
}
float weight_at(const std::vector<float>* weights, std::size_t i, float fallback) noexcept {
    if (!weights || i >= weights->size()) return fallback;
    return std::clamp((*weights)[i], 0.0f, 1.0f);
}
}

Transform blend_transform(const Transform& a, const Transform& b, float weight) noexcept {
    const float w = std::clamp(weight, 0.0f, 1.0f);
    return interpolate(a, b, w);
}

Pose blend_pose(const Pose& a, const Pose& b, float weight) noexcept {
    Pose result = a;
    const std::size_t count = std::min(a.local_transforms.size(), b.local_transforms.size());
    result.local_transforms.resize(std::max(a.local_transforms.size(), b.local_transforms.size()));
    for (std::size_t i = 0; i < count; ++i) result.local_transforms[i] = blend_transform(a.local_transforms[i], b.local_transforms[i], weight);
    return result;
}

Pose blend_pose_masked(const Pose& a, const Pose& b, const std::vector<float>& weights) noexcept {
    Pose result = a;
    result.local_transforms.resize(std::max(a.local_transforms.size(), b.local_transforms.size()));
    const std::size_t count = std::min(a.local_transforms.size(), b.local_transforms.size());
    for (std::size_t i = 0; i < count; ++i) result.local_transforms[i] = blend_transform(a.local_transforms[i], b.local_transforms[i], i < weights.size() ? weights[i] : 0.0f);
    return result;
}

Pose apply_additive_pose(const Pose& base, const Pose& delta, float weight, const std::vector<float>* weights) noexcept {
    Pose result = base;
    const std::size_t count = std::min(base.local_transforms.size(), delta.local_transforms.size());
    const float global = std::clamp(weight, 0.0f, 1.0f);
    for (std::size_t i = 0; i < count; ++i) {
        const float w = weight_at(weights, i, global);
        auto& dst = result.local_transforms[i];
        const auto& d = delta.local_transforms[i];
        dst.translation.x += d.translation.x * w;
        dst.translation.y += d.translation.y * w;
        dst.translation.z += d.translation.z * w;
        const Quat delta_rotation = slerp({0,0,0,1}, d.rotation, w);
        dst.rotation = normalize(multiply(dst.rotation, delta_rotation));
        dst.scale.x *= 1.0f + (d.scale.x - 1.0f) * w;
        dst.scale.y *= 1.0f + (d.scale.y - 1.0f) * w;
        dst.scale.z *= 1.0f + (d.scale.z - 1.0f) * w;
    }
    return result;
}

} // namespace auto_animation::animation
