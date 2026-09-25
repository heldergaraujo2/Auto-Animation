#include "auto_animation/animation/Animation.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace auto_animation::animation {
namespace {
constexpr float kEpsilon=1.0e-6f;
template<typename T,typename F>
T sample_curve(const Curve<T>& c,double t,const T& fallback,F&& f){
    if(c.keys.empty()) return fallback;
    if(t<=c.keys.front().time_seconds) return c.keys.front().value;
    if(t>=c.keys.back().time_seconds) return c.keys.back().value;
    const auto it=std::upper_bound(c.keys.begin(),c.keys.end(),t,[](double v,const Key<T>& k){return v<k.time_seconds;});
    const auto& b=*it; const auto& a=*(it-1); const double span=b.time_seconds-a.time_seconds;
    if(span<=0.0) return b.value;
    const float alpha=static_cast<float>((t-a.time_seconds)/span);
    if(a.interpolation==Interpolation::Step) return a.value;
    return f(a.value,b.value,alpha,a.interpolation);
}
bool finite(float v) noexcept{return std::isfinite(v);}
bool finite(double v) noexcept{return std::isfinite(v);}
bool finite(const Vec3& v) noexcept{return finite(v.x)&&finite(v.y)&&finite(v.z);}
bool finite(const Quat& q) noexcept{return finite(q.x)&&finite(q.y)&&finite(q.z)&&finite(q.w);}
}
Vec3 lerp(const Vec3&a,const Vec3&b,float t) noexcept{return {a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t};}
Quat normalize(const Quat&q) noexcept{const float n=q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;if(n<=kEpsilon)return {};const float i=1.0f/std::sqrt(n);return {q.x*i,q.y*i,q.z*i,q.w*i};}
Quat nlerp(const Quat&a,const Quat&b,float t) noexcept{Quat q=b;float d=a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;if(d<0.0f)q={-b.x,-b.y,-b.z,-b.w};return normalize({a.x+(q.x-a.x)*t,a.y+(q.y-a.y)*t,a.z+(q.z-a.z)*t,a.w+(q.w-a.w)*t});}
Quat slerp(const Quat&a,const Quat&b,float t) noexcept{Quat qa=normalize(a),qb=normalize(b);float d=qa.x*qb.x+qa.y*qb.y+qa.z*qb.z+qa.w*qb.w;if(d<0.0f){qb={-qb.x,-qb.y,-qb.z,-qb.w};d=-d;}if(d>0.9995f)return nlerp(qa,qb,t);d=std::clamp(d,-1.0f,1.0f);const float theta=std::acos(d),s=std::sin(theta);if(std::abs(s)<=kEpsilon)return qa;const float aw=std::sin((1.0f-t)*theta)/s,bw=std::sin(t*theta)/s;return normalize({qa.x*aw+qb.x*bw,qa.y*aw+qb.y*bw,qa.z*aw+qb.z*bw,qa.w*aw+qb.w*bw});}
Transform interpolate(const Transform&a,const Transform&b,float t) noexcept{return {lerp(a.translation,b.translation,t),slerp(a.rotation,b.rotation,t),lerp(a.scale,b.scale,t)};}
Vec3 sample(const Curve<Vec3>&c,double t,const Vec3&f){return sample_curve(c,t,f,[](const Vec3&a,const Vec3&b,float x,Interpolation){return lerp(a,b,x);});}
Quat sample(const Curve<Quat>&c,double t,const Quat&f){return sample_curve(c,t,f,[](const Quat&a,const Quat&b,float x,Interpolation i){return i==Interpolation::Spherical?slerp(a,b,x):nlerp(a,b,x);});}
AnimationSample sample_clip(const AnimationClip&clip,double t,const Pose&bind){
    AnimationSample out{t,bind.local_transforms};
    if(clip.duration_seconds>0.0){if(clip.looping){t=std::fmod(t,clip.duration_seconds);if(t<0.0)t+=clip.duration_seconds;}else t=std::clamp(t,0.0,clip.duration_seconds);}
    for(const auto& track:clip.tracks){if(track.bone_index<0)continue;const auto i=static_cast<std::size_t>(track.bone_index);if(i>=out.local_transforms.size())continue;auto& x=out.local_transforms[i];x.translation=sample(track.translation,t,x.translation);x.rotation=sample(track.rotation,t,x.rotation);x.scale=sample(track.scale,t,x.scale);}
    out.time_seconds=t;return out;
}
AnimationValidationResult validate(const AnimationClip&clip){
    AnimationValidationResult r;
    if(clip.name.empty())r.issues.push_back({AnimationValidationIssue::Severity::Warning,"Animation clip has no name."});
    if(!finite(clip.duration_seconds)||clip.duration_seconds<0.0)r.issues.push_back({AnimationValidationIssue::Severity::Error,"Animation duration must be finite and non-negative."});
    if(!finite(clip.sample_rate)||clip.sample_rate<0.0)r.issues.push_back({AnimationValidationIssue::Severity::Error,"Animation sample rate must be finite and non-negative."});
    for(const auto& t:clip.tracks){
        if(t.bone_name.empty()&&t.bone_index<0)r.issues.push_back({AnimationValidationIssue::Severity::Error,"Track has neither a bone name nor a bone index."});
        const auto check=[&](const auto& c,const char* label){double prev=-std::numeric_limits<double>::infinity();for(const auto& k:c.keys){if(!finite(k.time_seconds)||k.time_seconds<0.0||k.time_seconds>clip.duration_seconds+1e-9)r.issues.push_back({AnimationValidationIssue::Severity::Error,std::string(label)+" contains an invalid key time."});if(k.time_seconds<prev)r.issues.push_back({AnimationValidationIssue::Severity::Error,std::string(label)+" key times are not sorted."});prev=k.time_seconds;}};
        check(t.translation,"Translation curve");check(t.rotation,"Rotation curve");check(t.scale,"Scale curve");
        for(const auto& k:t.translation.keys)if(!finite(k.value))r.issues.push_back({AnimationValidationIssue::Severity::Error,"Translation curve contains non-finite values."});
        for(const auto& k:t.rotation.keys){if(!finite(k.value))r.issues.push_back({AnimationValidationIssue::Severity::Error,"Rotation curve contains non-finite values."});const float n=std::sqrt(k.value.x*k.value.x+k.value.y*k.value.y+k.value.z*k.value.z+k.value.w*k.value.w);if(n<=kEpsilon)r.issues.push_back({AnimationValidationIssue::Severity::Error,"Rotation curve contains a zero-length quaternion."});}
        for(const auto& k:t.scale.keys)if(!finite(k.value))r.issues.push_back({AnimationValidationIssue::Severity::Error,"Scale curve contains non-finite values."});
    }
    return r;
}
bool AnimationValidationResult::valid()const noexcept{return std::none_of(issues.begin(),issues.end(),[](const auto&i){return i.severity==AnimationValidationIssue::Severity::Error;});}
bool AnimationValidationResult::has_warnings()const noexcept{return std::any_of(issues.begin(),issues.end(),[](const auto&i){return i.severity==AnimationValidationIssue::Severity::Warning;});}
} // namespace auto_animation::animation
