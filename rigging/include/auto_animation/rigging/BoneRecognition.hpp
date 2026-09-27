#pragma once
#include "auto_animation/rigging/Skeleton.hpp"
#include <string>
#include <vector>
namespace auto_animation::rigging{struct BoneMatch{std::int32_t bone=-1;std::string canonical;float confidence=0;};std::vector<BoneMatch>recognize_bones(const Skeleton&);}