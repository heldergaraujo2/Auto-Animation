#include "auto_animation/animation/Animation.hpp"

#include <cmath>
#include <iostream>
#include <string_view>

namespace {
int failures=0;
void expect(bool v,std::string_view m){if(!v){std::cerr<<"FAIL: "<<m<<'\n';++failures;}}
bool near(float a,float b,float e=1e-4f){return std::abs(a-b)<=e;}
}

int main(){
    using namespace auto_animation::animation;

    Curve<Vec3> position;
    position.keys={{0.0,{0,0,0},Interpolation::Linear},{1.0,{10,0,0},Interpolation::Linear}};
    auto p=sample(position,0.5,{99,99,99});
    expect(near(p.x,5.0f),"vector curve interpolates independently");

    Curve<Quat> rotation;
    rotation.keys={{0.0,{0,0,0,1},Interpolation::Spherical},{1.0,{0,0,1,0},Interpolation::Spherical}};
    auto q=sample(rotation,0.5,{});
    const float qlen=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
    expect(near(qlen,1.0f),"quaternion sampling remains normalized");

    AnimationClip clip;
    clip.name="IndependentChannels";
    clip.duration_seconds=2.0;
    clip.sample_rate=30.0;
    clip.looping=true;
    AnimationTrack track;
    track.bone_name="root";
    track.bone_index=0;
    track.translation.keys={{0.0,{0,0,0},Interpolation::Linear},{2.0,{2,0,0},Interpolation::Linear}};
    track.rotation.keys={{0.5,{0,0,0,1},Interpolation::Spherical},{1.5,{0,0,1,0},Interpolation::Spherical}};
    track.scale.keys={{1.0,{1,1,1},Interpolation::Linear},{2.0,{2,2,2},Interpolation::Linear}};
    clip.tracks.push_back(track);

    Pose bind;
    bind.local_transforms.resize(1);
    auto s=sample_clip(clip,2.5,bind);
    expect(near(static_cast<float>(s.time_seconds),0.5f),"looping wraps time");
    expect(near(s.local_transforms[0].translation.x,0.5f),"translation channel keeps its own timing");
    expect(near(s.local_transforms[0].scale.x,1.0f),"scale channel keeps its own timing");

    auto valid=validate(clip);
    expect(valid.valid(),"valid clip passes validation");

    AnimationClip bad=clip;
    bad.duration_seconds=-1.0;
    bad.tracks[0].translation.keys[1].time_seconds=3.0;
    expect(!validate(bad).valid(),"invalid duration/key time is rejected");

    AnimationClip unsorted=clip;
    unsorted.tracks[0].translation.keys[1].time_seconds=-0.5;
    expect(!validate(unsorted).valid(),"negative key time is rejected");

    AnimationClip empty;
    expect(validate(empty).has_warnings(),"unnamed clip produces a warning");

    return failures;
}
