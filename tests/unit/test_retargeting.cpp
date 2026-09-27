#include "auto_animation/rigging/Retargeting.hpp"

#include <cmath>
#include <iostream>
#include <string_view>

namespace {
int failures=0;
void expect(bool v,std::string_view m){if(!v){std::cerr<<"FAIL: "<<m<<'\n';++failures;}}
bool near(float a,float b,float e=1e-4f){return std::abs(a-b)<=e;}
}

int main() {
    using namespace auto_animation::animation;
    using namespace auto_animation::rigging;
    Skeleton source, target;
    const auto sr=source.add_bone("Root",-1,{{0,0,0},{0,0,0,1},{1,1,1}});
    source.add_bone("Arm_L",sr,{{1,0,0},{0,0,0,1},{1,1,1}});
    const auto tr=target.add_bone("mixamorig:Root",-1,{{0,0,0},{0,0,0,1},{1,1,1}});
    target.add_bone("Arm_L",tr,{{1,0,0},{0,0,0,1},{1,1,1}});
    auto mapping=build_name_mapping(source,target);
    expect(mapping.mappings.size()==2,"name mapping matches normalized names");
    Pose pose; pose.local_transforms={source.bones[0].bind_local,source.bones[1].bind_local};
    pose.local_transforms[0].translation={2,0,0};
    auto out=retarget_pose(source,pose,target,mapping,{true,2.0f});
    expect(near(out.local_transforms[0].translation.x,4.0f),"root translation scale transfers");
    expect(near(out.local_transforms[1].translation.x,1.0f),"unmodified local target transform remains valid");
    return failures;
}
