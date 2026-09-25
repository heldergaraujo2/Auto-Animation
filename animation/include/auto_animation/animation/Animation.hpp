#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace auto_animation::animation {

struct Vec3 { float x=0.0f, y=0.0f, z=0.0f; };
struct Quat { float x=0.0f, y=0.0f, z=0.0f, w=1.0f; };
struct Transform { Vec3 translation{}; Quat rotation{}; Vec3 scale{1.0f,1.0f,1.0f}; };

enum class Interpolation : std::uint8_t { Step, Linear, Spherical };

template <typename T>
struct Key { double time_seconds=0.0; T value{}; Interpolation interpolation=Interpolation::Linear; };

template <typename T>
struct Curve {
    std::vector<Key<T>> keys;
    [[nodiscard]] bool empty() const noexcept { return keys.empty(); }
    [[nodiscard]] std::size_t size() const noexcept { return keys.size(); }
};

struct AnimationTrack {
    std::string bone_name;
    std::int32_t bone_index=-1;
    Curve<Vec3> translation;
    Curve<Quat> rotation;
    Curve<Vec3> scale;
    [[nodiscard]] bool empty() const noexcept { return translation.empty() && rotation.empty() && scale.empty(); }
};

struct AnimationClip {
    std::string name;
    double duration_seconds=0.0;
    double sample_rate=0.0;
    bool looping=false;
    bool root_motion=false;
    std::vector<AnimationTrack> tracks;
};

struct Pose { std::vector<Transform> local_transforms; };
struct AnimationSample { double time_seconds=0.0; std::vector<Transform> local_transforms; };

struct AnimationValidationIssue {
    enum class Severity : std::uint8_t { Warning, Error };
    Severity severity=Severity::Error;
    std::string message;
};

struct AnimationValidationResult {
    std::vector<AnimationValidationIssue> issues;
    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] bool has_warnings() const noexcept;
};

[[nodiscard]] Vec3 lerp(const Vec3&, const Vec3&, float) noexcept;
[[nodiscard]] Quat normalize(const Quat&) noexcept;
[[nodiscard]] Quat nlerp(const Quat&, const Quat&, float) noexcept;
[[nodiscard]] Quat slerp(const Quat&, const Quat&, float) noexcept;
[[nodiscard]] Transform interpolate(const Transform&, const Transform&, float) noexcept;
[[nodiscard]] Vec3 sample(const Curve<Vec3>&, double, const Vec3&);
[[nodiscard]] Quat sample(const Curve<Quat>&, double, const Quat&);
[[nodiscard]] AnimationSample sample_clip(const AnimationClip&, double, const Pose&);
[[nodiscard]] AnimationValidationResult validate(const AnimationClip&);

} // namespace auto_animation::animation
