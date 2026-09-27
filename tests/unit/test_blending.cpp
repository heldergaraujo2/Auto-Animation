#include "auto_animation/animation/Blending.hpp"

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
    Pose a{{{{0,0,0},{0,0,0,1},{1,1,1}}}};
    Pose b{{{{10,0,0},{0,0,0,1},{2,2,2}}}};
    auto half=blend_pose(a,b,0.5f);
    expect(near(half.local_transforms[0].translation.x,5),"pose blending translates");
    expect(near(half.local_transforms[0].scale.x,1.5f),"pose blending scales");
    auto masked=blend_pose_masked(a,b,{0.0f});
    expect(near(masked.local_transforms[0].translation.x,0),"mask preserves base");
    auto additive=apply_additive_pose(a,Pose{{{{2,0,0},{0,0,0,1},{1,1,1}}}},0.5f);
    expect(near(additive.local_transforms[0].translation.x,1),"additive translation works");
    return failures;
}
