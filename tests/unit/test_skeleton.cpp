#include "auto_animation/rigging/Skeleton.hpp"

#include <cmath>
#include <iostream>
#include <string_view>

namespace {
int failures=0;
void expect(bool v,std::string_view m){if(!v){std::cerr<<"FAIL: "<<m<<'\n';++failures;}}
bool near(float a,float b,float e=1e-4f){return std::abs(a-b)<=e;}
}

int main(){
    using namespace auto_animation::rigging;
    using namespace auto_animation::animation;

    Skeleton s;
    s.name="TestSkeleton";
    const auto root=s.add_bone("Root",-1);
    const auto arm=s.add_bone("Arm",root,{{1,0,0},{0,0,0,1},{1,1,1}});
    const auto hand=s.add_bone("Hand",arm,{{1,0,0},{0,0,0,1},{1,1,1}});
    expect(root==0&&arm==1&&hand==2,"bones are created with hierarchy");
    expect(s.validate().valid(),"new hierarchy validates");

    expect(s.rename_bone(arm,"Arm_L"),"bone can be renamed");
    expect(s.find_bone("Arm_L")==arm,"renamed bone is discoverable");
    expect(!s.rename_bone(hand,"Arm_L"),"duplicate bone names are rejected");

    const auto world_before_reparent=s.world_transform(hand,true);
    expect(near(world_before_reparent.translation.x,2.0f),"bind world transform composes hierarchy");

    expect(s.set_parent(hand,root),"bone can be reparented");
    expect(s.bones[hand].parent_index==root,"reparent is applied");
    expect(!s.set_parent(root,hand),"cycles are rejected");

    Transform pose{{2,0,0},{0,0,0,1},{1,1,1}};
    expect(s.set_pose_transform(arm,pose),"pose can be edited");
    expect(near(s.world_transform(arm).translation.x,2.0f),"edited pose is reflected in world transform");
    expect(s.reset_pose_to_bind(arm),"pose can be reset");

    expect(s.set_group(arm,"UpperBody"),"bone groups can be assigned");
    expect(s.bones[arm].group=="UpperBody","bone group persists");
    expect(s.set_pivot(arm,{0.1f,0.2f,0.3f}),"bone pivot can be edited");
    expect(near(s.bones[arm].pivot.z,0.3f),"bone pivot persists");
    expect(s.set_editable(hand,false),"bone editability can be controlled");
    expect(!s.set_pose_transform(hand,{{3,0,0},{},{1,1,1}}),"locked bone rejects pose edits");
    expect(s.set_editable(hand,true),"bone can be unlocked");
    expect(s.select_bone(arm)&&s.selected_bone==arm,"bone selection works");
    expect(s.select_bone(-1)&&s.selected_bone==-1,"selection can be cleared");
    expect(s.roots().size()==1&&s.roots()[0]==root,"root inspection works");
    expect(s.children_of(root).size()==2,"child inspection works");

    expect(s.mirror_pose(arm,MirrorAxis::X),"bone pose can be mirrored");
    expect(near(s.bones[arm].local_pose.translation.x,-1.0f),"mirror changes selected axis");
    expect(s.mirror_bone_to_sibling(arm,hand,MirrorAxis::X),"pose can be mirrored to another bone");

    expect(s.set_parent(hand,arm),"child restored for removal test");
    expect(!s.remove_bone(arm,RemovePolicy::RejectWithChildren),"removal with children can be rejected");
    expect(s.remove_bone(arm,RemovePolicy::ReparentChildren),"controlled removal reparents children");
    expect(s.bones.size()==2,"removed bone is erased");

    Skeleton invalid;
    invalid.add_bone("Root",-1);
    invalid.bones[0].parent_index=5;
    expect(!invalid.validate().valid(),"invalid parent index is detected");

    Skeleton duplicate;
    duplicate.add_bone("Root",-1);
    duplicate.bones.push_back(duplicate.bones[0]);
    expect(!duplicate.validate().valid(),"duplicate names are detected");

    return failures;
}
