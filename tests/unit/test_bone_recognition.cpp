#include "auto_animation/rigging/BoneRecognition.hpp"
#include <cassert>
int main(){using namespace auto_animation::rigging;Skeleton s;s.add_bone("mixamorig:LeftArm",-1);s.add_bone("RightFoot",-1);auto r=recognize_bones(s);assert(r.size()>=2);assert(r[0].canonical=="left_arm");return 0;}