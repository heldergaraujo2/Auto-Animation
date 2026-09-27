#pragma once
#include "auto_animation/animation/Animation.hpp"
#include "auto_animation/rigging/Skeleton.hpp"
#include <cstdint>
namespace auto_animation::motion {
struct FlyIdleRig { std::int32_t root=-1, body=-1, wing_left=-1, wing_right=-1, leg_left=-1, leg_right=-1; };
struct FlyIdleParameters { double duration_seconds=2.0, sample_rate=30.0; float vertical_amplitude=2.0f, body_pitch_degrees=2.0f, wing_sweep_degrees=22.0f, wing_roll_degrees=8.0f, left_leg_bend_degrees=12.0f, right_leg_bend_degrees=18.0f, left_leg_drop=-0.03f, right_leg_drop=0.04f; };
[[nodiscard]] animation::AnimationClip generate_fly_idle(const rigging::Skeleton&, const FlyIdleRig&, const FlyIdleParameters& = {});
} // namespace auto_animation::motion
