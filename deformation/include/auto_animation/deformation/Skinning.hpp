#pragma once
#include "auto_animation/animation/Animation.hpp"
#include "auto_animation/rigging/Skeleton.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <vector>
namespace auto_animation::deformation {
struct SkinVertex { animation::Vec3 position{}; std::array<std::int32_t,4> bone_indices{{-1,-1,-1,-1}}; std::array<float,4> bone_weights{{0,0,0,0}}; };
struct SkinningOptions { std::size_t max_influences=4; float falloff_power=2.0f; bool normalize_weights=true; };
struct SkinningResult { std::vector<std::array<std::int32_t,4>> indices; std::vector<std::array<float,4>> weights; std::vector<std::string> warnings; bool valid() const noexcept{return !weights.empty()&&indices.size()==weights.size();} };
SkinningResult generate_heat_weights(const std::vector<animation::Vec3>&,const rigging::Skeleton&,const SkinningOptions&={});
animation::Vec3 apply_lbs(const SkinVertex&,const rigging::Skeleton&);
animation::Vec3 apply_dual_quaternion(const SkinVertex&,const rigging::Skeleton&);
}