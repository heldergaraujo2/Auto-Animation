#include "auto_animation/rigging/IK.hpp"
#include <cassert>
int main(){using namespace auto_animation;rigging::Skeleton s;int a=s.add_bone("root",-1);int b=s.add_bone("mid",a,{{1,0,0},{0,0,0,1},{1,1,1}});int c=s.add_bone("end",b,{{1,0,0},{0,0,0,1},{1,1,1}});auto r=rigging::solve_fabrik(s,{{a,b,c}},{{1,1,0},30,.01f});assert(r.error<.02f);return 0;}