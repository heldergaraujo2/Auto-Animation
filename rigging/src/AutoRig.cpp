#include "auto_animation/rigging/AutoRig.hpp"
#include <array>
namespace auto_animation::rigging {
namespace {
struct Rule { anatomy::MarkerType type; anatomy::MarkerType parent; const char* name; };
constexpr Rule rules[] = {
 {anatomy::MarkerType::Root,anatomy::MarkerType::Custom,"Root"},
 {anatomy::MarkerType::Pelvis,anatomy::MarkerType::Root,"Pelvis"},
 {anatomy::MarkerType::Spine,anatomy::MarkerType::Pelvis,"Spine"},
 {anatomy::MarkerType::Chest,anatomy::MarkerType::Spine,"Chest"},
 {anatomy::MarkerType::Neck,anatomy::MarkerType::Chest,"Neck"},
 {anatomy::MarkerType::Head,anatomy::MarkerType::Neck,"Head"},
 {anatomy::MarkerType::ShoulderLeft,anatomy::MarkerType::Chest,"Shoulder_L"},
 {anatomy::MarkerType::ElbowLeft,anatomy::MarkerType::ShoulderLeft,"Elbow_L"},
 {anatomy::MarkerType::WristLeft,anatomy::MarkerType::ElbowLeft,"Wrist_L"},
 {anatomy::MarkerType::HandLeft,anatomy::MarkerType::WristLeft,"Hand_L"},
 {anatomy::MarkerType::ShoulderRight,anatomy::MarkerType::Chest,"Shoulder_R"},
 {anatomy::MarkerType::ElbowRight,anatomy::MarkerType::ShoulderRight,"Elbow_R"},
 {anatomy::MarkerType::WristRight,anatomy::MarkerType::ElbowRight,"Wrist_R"},
 {anatomy::MarkerType::HandRight,anatomy::MarkerType::WristRight,"Hand_R"},
 {anatomy::MarkerType::HipLeft,anatomy::MarkerType::Pelvis,"Hip_L"},
 {anatomy::MarkerType::KneeLeft,anatomy::MarkerType::HipLeft,"Knee_L"},
 {anatomy::MarkerType::AnkleLeft,anatomy::MarkerType::KneeLeft,"Ankle_L"},
 {anatomy::MarkerType::FootLeft,anatomy::MarkerType::AnkleLeft,"Foot_L"},
 {anatomy::MarkerType::HipRight,anatomy::MarkerType::Pelvis,"Hip_R"},
 {anatomy::MarkerType::KneeRight,anatomy::MarkerType::HipRight,"Knee_R"},
 {anatomy::MarkerType::AnkleRight,anatomy::MarkerType::KneeRight,"Ankle_R"},
 {anatomy::MarkerType::FootRight,anatomy::MarkerType::AnkleRight,"Foot_R"},
 {anatomy::MarkerType::WingRootLeft,anatomy::MarkerType::Chest,"WingRoot_L"},
 {anatomy::MarkerType::WingJointLeft,anatomy::MarkerType::WingRootLeft,"WingJoint_L"},
 {anatomy::MarkerType::WingTipLeft,anatomy::MarkerType::WingJointLeft,"WingTip_L"},
 {anatomy::MarkerType::WingRootRight,anatomy::MarkerType::Chest,"WingRoot_R"},
 {anatomy::MarkerType::WingJointRight,anatomy::MarkerType::WingRootRight,"WingJoint_R"},
 {anatomy::MarkerType::WingTipRight,anatomy::MarkerType::WingJointRight,"WingTip_R"},
 {anatomy::MarkerType::TailRoot,anatomy::MarkerType::Pelvis,"TailRoot"},
 {anatomy::MarkerType::TailSegment,anatomy::MarkerType::TailRoot,"TailSegment"},
 {anatomy::MarkerType::TailTip,anatomy::MarkerType::TailSegment,"TailTip"},
 {anatomy::MarkerType::Jaw,anatomy::MarkerType::Head,"Jaw"},
 {anatomy::MarkerType::Horn,anatomy::MarkerType::Head,"Horn"},
 {anatomy::MarkerType::Antenna,anatomy::MarkerType::Head,"Antenna"},
 {anatomy::MarkerType::Fin,anatomy::MarkerType::Chest,"Fin"},
};
const anatomy::AnatomicalMarker* marker(const anatomy::MarkerSet& s, anatomy::MarkerType t){return s.find(t);}
}
AutoRigResult build_rig_from_markers(const anatomy::MarkerSet& markers){
    AutoRigResult result; result.skeleton.name=markers.profile.empty()?"AutoRig":markers.profile+"Rig";
    std::array<std::int32_t,static_cast<std::size_t>(anatomy::MarkerType::Custom)+1> ids{}; ids.fill(-1);
    for(const auto& rule:rules){
        const auto* m=marker(markers,rule.type); if(!m) continue;
        std::int32_t parent=-1;
        if(rule.parent!=anatomy::MarkerType::Custom){
            const auto pi=static_cast<std::size_t>(rule.parent);
            if(pi<ids.size() && ids[pi]>=0) parent=ids[pi];
            else result.warnings.push_back(std::string("Missing parent marker for ")+rule.name);
        }
        animation::Transform bind{};
        bind.translation=m->position;
        if(parent>=0){
            const auto* pm=marker(markers,rule.parent);
            if(pm) bind.translation={m->position.x-pm->position.x,m->position.y-pm->position.y,m->position.z-pm->position.z};
        }
        const auto id=result.skeleton.add_bone(rule.name,parent,bind);
        if(id>=0) ids[static_cast<std::size_t>(rule.type)]=id;
    }
    if(result.skeleton.empty()) result.warnings.push_back("No recognized anatomical markers were supplied.");
    return result;
}
} // namespace auto_animation::rigging
