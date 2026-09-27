#include "auto_animation/animation/MotionDatabase.hpp"
#include <cassert>
int main(){using namespace auto_animation::animation;AnimationClip c;c.duration_seconds=1;c.tracks.push_back({"root",0,{{0,{0,0,0}},{1,{1,0,0}}},{},{}});MotionDatabase d;d.add_clip(c,10);assert(!d.features().empty());assert(d.query(d.features().front()).distance==0);return 0;}