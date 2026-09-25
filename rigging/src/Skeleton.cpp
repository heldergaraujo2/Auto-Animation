#include "auto_animation/rigging/Skeleton.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace auto_animation::rigging {
namespace {
animation::Quat quat_multiply(const animation::Quat& a,const animation::Quat& b) noexcept {
    return {
        a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,
        a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
        a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,
        a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z
    };
}
animation::Vec3 rotate(const animation::Quat& q,const animation::Vec3& v) noexcept {
    const animation::Quat p{v.x,v.y,v.z,0.0f};
    const animation::Quat qi{-q.x,-q.y,-q.z,q.w};
    const auto r=quat_multiply(quat_multiply(q,p),qi);
    return {r.x,r.y,r.z};
}
animation::Transform compose(const animation::Transform& parent,const animation::Transform& local) noexcept {
    const auto scaled=animation::Vec3{local.translation.x*parent.scale.x,local.translation.y*parent.scale.y,local.translation.z*parent.scale.z};
    return {
        {parent.translation.x+rotate(parent.rotation,scaled).x,
         parent.translation.y+rotate(parent.rotation,scaled).y,
         parent.translation.z+rotate(parent.rotation,scaled).z},
        animation::normalize(quat_multiply(parent.rotation,local.rotation)),
        {parent.scale.x*local.scale.x,parent.scale.y*local.scale.y,parent.scale.z*local.scale.z}
    };
}
bool finite(float v){return std::isfinite(v);}
bool finite(const animation::Vec3& v){return finite(v.x)&&finite(v.y)&&finite(v.z);}
bool finite(const animation::Quat& q){return finite(q.x)&&finite(q.y)&&finite(q.z)&&finite(q.w);}
}

animation::Vec3 mirror_vector(const animation::Vec3& v,MirrorAxis axis) noexcept {
    auto r=v;
    if(axis==MirrorAxis::X) r.x=-r.x;
    if(axis==MirrorAxis::Y) r.y=-r.y;
    if(axis==MirrorAxis::Z) r.z=-r.z;
    return r;
}
animation::Quat mirror_rotation(const animation::Quat& q,MirrorAxis axis) noexcept {
    // Reflection changes handedness; negate the two components tangent to the mirror plane.
    if(axis==MirrorAxis::X) return {q.x,-q.y,-q.z,q.w};
    if(axis==MirrorAxis::Y) return {-q.x,q.y,-q.z,q.w};
    return {-q.x,-q.y,q.z,q.w};
}
bool Skeleton::has_bone(std::int32_t i) const noexcept { return i>=0 && static_cast<std::size_t>(i)<bones.size(); }
std::int32_t Skeleton::find_bone(const std::string& name) const noexcept {
    for(std::size_t i=0;i<bones.size();++i) if(bones[i].name==name) return static_cast<std::int32_t>(i);
    return -1;
}
std::int32_t Skeleton::add_bone(std::string name,std::int32_t parent,const animation::Transform& bind) {
    if(name.empty() || find_bone(name)>=0 || (parent!=-1&&!has_bone(parent))) return -1;
    Bone b; b.name=std::move(name); b.parent_index=parent; b.bind_local=bind; b.local_pose=bind;
    bones.push_back(std::move(b)); return static_cast<std::int32_t>(bones.size()-1);
}
bool Skeleton::rename_bone(std::int32_t i,std::string name) {
    if(!has_bone(i)||name.empty()||find_bone(name)>=0) return false;
    bones[static_cast<std::size_t>(i)].name=std::move(name); return true;
}
bool Skeleton::set_parent(std::int32_t i,std::int32_t p) {
    if(!has_bone(i)||(p!=-1&&!has_bone(p))||i==p) return false;
    if(p>=0){std::int32_t cursor=p;while(cursor!=-1){if(cursor==i)return false;cursor=bones[cursor].parent_index;}}
    bones[i].parent_index=p; return true;
}
bool Skeleton::remove_bone(std::int32_t i,RemovePolicy policy) {
    if(!has_bone(i)) return false;
    for(const auto& b:bones) if(b.parent_index==i && policy==RemovePolicy::RejectWithChildren) return false;
    const auto old_parent=bones[i].parent_index;
    for(auto& b:bones) if(b.parent_index==i) b.parent_index=old_parent;
    bones.erase(bones.begin()+i);
    for(auto& b:bones) {
        if(b.parent_index>i) --b.parent_index;
    }
    if(selected_bone==i) selected_bone=-1;
    else if(selected_bone>i) --selected_bone;
    return true;
}
bool Skeleton::set_group(std::int32_t i,std::string group){if(!has_bone(i))return false;bones[i].group=std::move(group);return true;}
bool Skeleton::set_pose_transform(std::int32_t i,const animation::Transform& t){if(!has_bone(i)||!bones[i].editable)return false;bones[i].local_pose=t;return true;}
bool Skeleton::reset_pose_to_bind(std::int32_t i){if(!has_bone(i))return false;bones[i].local_pose=bones[i].bind_local;return true;}
bool Skeleton::mirror_pose(std::int32_t i,MirrorAxis axis){
    if(!has_bone(i)||!bones[i].editable)return false;
    auto& t=bones[i].local_pose;t.translation=mirror_vector(t.translation,axis);t.rotation=mirror_rotation(t.rotation,axis);return true;
}
bool Skeleton::mirror_bone_to_sibling(std::int32_t source,std::int32_t target,MirrorAxis axis){
    if(!has_bone(source)||!has_bone(target)||!bones[target].editable)return false;
    bones[target].local_pose=bones[source].local_pose;
    bones[target].local_pose.translation=mirror_vector(bones[target].local_pose.translation,axis);
    bones[target].local_pose.rotation=mirror_rotation(bones[target].local_pose.rotation,axis);
    return true;
}
animation::Transform Skeleton::world_transform(std::int32_t i,bool bind) const {
    if(!has_bone(i)) return {};
    std::vector<std::int32_t> chain;
    for(auto c=i;c!=-1;c=bones[c].parent_index) chain.push_back(c);
    animation::Transform result{};
    result.rotation={0,0,0,1}; result.scale={1,1,1};
    for(auto it=chain.rbegin();it!=chain.rend();++it) result=compose(result,bind?bones[*it].bind_local:bones[*it].local_pose);
    return result;
}
SkeletonValidationResult Skeleton::validate() const {
    SkeletonValidationResult r;
    std::unordered_set<std::string> names;
    for(std::size_t i=0;i<bones.size();++i){
        const auto& b=bones[i];
        if(b.name.empty())r.issues.push_back({SkeletonIssue::Severity::Error,"Bone has an empty name."});
        if(!names.insert(b.name).second)r.issues.push_back({SkeletonIssue::Severity::Error,"Duplicate bone name: "+b.name});
        if(b.parent_index< -1 || (b.parent_index>=0&&!has_bone(b.parent_index)))r.issues.push_back({SkeletonIssue::Severity::Error,"Bone has an invalid parent index: "+b.name});
        if(b.parent_index==static_cast<std::int32_t>(i))r.issues.push_back({SkeletonIssue::Severity::Error,"Bone is its own parent: "+b.name});
        if(!finite(b.bind_local.translation)||!finite(b.bind_local.rotation)||!finite(b.bind_local.scale)||!finite(b.local_pose.translation)||!finite(b.local_pose.rotation)||!finite(b.local_pose.scale))r.issues.push_back({SkeletonIssue::Severity::Error,"Bone has non-finite transform data: "+b.name});
        std::unordered_set<std::int32_t> seen;
        for(auto p=b.parent_index;p!=-1;p=bones[p].parent_index) if(!seen.insert(p).second){r.issues.push_back({SkeletonIssue::Severity::Error,"Hierarchy cycle detected at bone: "+b.name});break;}
    }
    if(bones.empty())r.issues.push_back({SkeletonIssue::Severity::Warning,"Skeleton has no bones."});
    return r;
}
bool SkeletonValidationResult::valid()const noexcept{return std::none_of(issues.begin(),issues.end(),[](const auto&i){return i.severity==SkeletonIssue::Severity::Error;});}
bool SkeletonValidationResult::has_warnings()const noexcept{return std::any_of(issues.begin(),issues.end(),[](const auto&i){return i.severity==SkeletonIssue::Severity::Warning;});}
} // namespace auto_animation::rigging
