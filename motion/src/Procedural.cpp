#include "auto_animation/motion/Procedural.hpp"
#include <algorithm>
#include <cmath>
namespace auto_animation::motion {
namespace {
constexpr float kPi=3.14159265358979323846f;
animation::Quat mul(const animation::Quat&a,const animation::Quat&b)noexcept{return{a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};}
animation::Quat axis_angle(animation::Vec3 a,float deg)noexcept{const float h=deg*kPi/180.0f*0.5f,s=std::sin(h);return animation::normalize({a.x*s,a.y*s,a.z*s,std::cos(h)});}
bool valid(const rigging::Skeleton&s,std::int32_t i)noexcept{return i>=0&&static_cast<std::size_t>(i)<s.bones.size();}
animation::Transform rotate(const animation::Transform&b,animation::Vec3 a,float deg)noexcept{auto r=b;r.rotation=animation::normalize(mul(b.rotation,axis_angle(a,deg)));return r;}
void track(animation::AnimationClip&c,const rigging::Skeleton&s,std::int32_t i,const std::vector<double>&times,const std::vector<animation::Transform>&v){if(!valid(s,i))return;animation::AnimationTrack t;t.bone_index=i;t.bone_name=s.bones[static_cast<std::size_t>(i)].name;for(std::size_t n=0;n<times.size();++n){t.translation.keys.push_back({times[n],v[n].translation,animation::Interpolation::Linear});t.rotation.keys.push_back({times[n],v[n].rotation,animation::Interpolation::Spherical});t.scale.keys.push_back({times[n],v[n].scale,animation::Interpolation::Linear});}c.tracks.push_back(std::move(t));}
}
animation::AnimationClip generate_fly_idle(const rigging::Skeleton&s,const FlyIdleRig&r,const FlyIdleParameters&p){
 animation::AnimationClip c;c.name="FlyIdle";c.duration_seconds=std::max(0.1,p.duration_seconds);c.sample_rate=std::max(1.0,p.sample_rate);c.looping=true;c.root_motion=false;
 const double d=c.duration_seconds;const std::vector<double> times{0,d*.25,d*.5,d*.75,d};
 const auto base=[&](std::int32_t i){return valid(s,i)?s.bones[static_cast<std::size_t>(i)].bind_local:animation::Transform{};};
 if(valid(s,r.root)){auto b=base(r.root);std::vector<animation::Transform>v;for(std::size_t i=0;i<times.size();++i){auto t=b;t.translation.z+=std::sin(static_cast<float>(i)*kPi*.5f)*p.vertical_amplitude;v.push_back(t);}track(c,s,r.root,times,v);}
 if(valid(s,r.body)){auto b=base(r.body);std::vector<animation::Transform>v;for(std::size_t i=0;i<times.size();++i)v.push_back(rotate(b,{1,0,0},std::sin(static_cast<float>(i)*kPi*.5f)*p.body_pitch_degrees));track(c,s,r.body,times,v);}
 if(valid(s,r.wing_left)){auto b=base(r.wing_left);std::vector<animation::Transform>v;for(std::size_t i=0;i<times.size();++i){float q=std::sin(static_cast<float>(i)*kPi*.5f);v.push_back(rotate(rotate(b,{0,1,0},q*p.wing_sweep_degrees),{1,0,0},std::sin(static_cast<float>(i)*kPi*.5f+kPi*.5f)*p.wing_roll_degrees));}track(c,s,r.wing_left,times,v);}
 if(valid(s,r.wing_right)){auto b=base(r.wing_right);std::vector<animation::Transform>v;for(std::size_t i=0;i<times.size();++i){float q=std::sin(static_cast<float>(i)*kPi*.5f);v.push_back(rotate(rotate(b,{0,1,0},-q*p.wing_sweep_degrees),{1,0,0},-std::sin(static_cast<float>(i)*kPi*.5f+kPi*.5f)*p.wing_roll_degrees));}track(c,s,r.wing_right,times,v);}
 if(valid(s,r.leg_left)){auto b=base(r.leg_left);std::vector<animation::Transform>v;for(std::size_t i=0;i<times.size();++i){auto t=rotate(b,{1,0,0},p.left_leg_bend_degrees);t.translation.z+=p.left_leg_drop;v.push_back(t);}track(c,s,r.leg_left,times,v);}
 if(valid(s,r.leg_right)){auto b=base(r.leg_right);std::vector<animation::Transform>v;for(std::size_t i=0;i<times.size();++i){auto t=rotate(b,{1,0,0},p.right_leg_bend_degrees);t.translation.z+=p.right_leg_drop;v.push_back(t);}track(c,s,r.leg_right,times,v);}
 return c;
}
} // namespace auto_animation::motion
