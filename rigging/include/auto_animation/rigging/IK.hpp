#pragma once
#include "auto_animation/animation/Animation.hpp"
#include "auto_animation/rigging/Skeleton.hpp"
#include <vector>
namespace auto_animation::rigging {
struct IKChain{std::vector<std::int32_t>bones;};
struct IKGoal{animation::Vec3 target{};std::size_t iterations=12;float tolerance=.001f;};
struct IKResult{bool converged=false;float error=0;std::vector<animation::Vec3>positions;};
IKResult solve_fabrik(const Skeleton&,const IKChain&,const IKGoal&);
bool apply_fabrik_pose(Skeleton&,const IKChain&,const IKGoal&);
}