#include "auto_animation/deformation/Skinning.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
namespace auto_animation::deformation {
namespace {
using V=animation::Vec3;
V add(V a,V b){return {a.x+b.x,a.y+b.y,a.z+b.z};} V sub(V a,V b){return {a.x-b.x,a.y-b.y,a.z-b.z};} V mul(V a,float s){return {a.x*s,a.y*s,a.z*s};}
float dot(V a,V b){return a.x*b.x+a.y*b.y+a.z*b.z;} float len(V a){return std::sqrt(dot(a,a));}
V rotate(const animation::Quat&q,V v){animation::Quat p{v.x,v.y,v.z,0},qi{-q.x,-q.y,-q.z,q.w};auto m=[](const animation::Quat&a,const animation::Quat&b){return animation::Quat{a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};};auto r=m(m(q,p),qi);return {r.x,r.y,r.z};}
animation::Transform compose(const animation::Transform&a,const animation::Transform&b){return {add(a.translation,rotate(a.rotation,{b.translation.x*a.scale.x,b.translation.y*a.scale.y,b.translation.z*a.scale.z})),animation::normalize({a.rotation.w*b.rotation.x+a.rotation.x*b.rotation.w+a.rotation.y*b.rotation.z-a.rotation.z*b.rotation.y,a.rotation.w*b.rotation.y-a.rotation.x*b.rotation.z+a.rotation.y*b.rotation.w+a.rotation.z*b.rotation.x,a.rotation.w*b.rotation.z+a.rotation.x*b.rotation.y-a.rotation.y*b.rotation.x+a.rotation.z*b.rotation.w,a.rotation.w*b.rotation.w-a.rotation.x*b.rotation.x-a.rotation.y*b.rotation.y-a.rotation.z*b.rotation.z}),{a.scale.x*b.scale.x,a.scale.y*b.scale.y,a.scale.z*b.scale.z}};}
animation::Transform world(const rigging::Skeleton&s,int i){std::vector<int>c;for(int x=i;x>=0;x=s.bones[x].parent_index)c.push_back(x);animation::Transform r{{0,0,0},{0,0,0,1},{1,1,1}};for(auto it=c.rbegin();it!=c.rend();++it)r=compose(r,s.bones[*it].local_pose);return r;}
float segment(V p,V a,V b){V ab=sub(b,a);float d=dot(ab,ab);float t=d>1e-8f?std::clamp(dot(sub(p,a),ab)/d,0.f,1.f):0.f;return len(sub(p,add(a,mul(ab,t))));}
}
SkinningResult generate_heat_weights(const std::vector<animation::Vec3>&positions,const rigging::Skeleton&s,const SkinningOptions&o){
 SkinningResult r;r.indices.resize(positions.size());r.weights.resize(positions.size());if(s.bones.empty()){r.warnings.push_back("Skeleton is empty.");return r;}
 for(size_t vi=0;vi<positions.size();++vi){std::vector<std::pair<float,int>> scores;
  for(int bi=0;bi<(int)s.bones.size();++bi){auto bw=world(s,bi);float d=len(sub(positions[vi],bw.translation));for(int ch:s.children_of(bi)){auto cw=world(s,ch);d=std::min(d,segment(positions[vi],bw.translation,cw.translation));}d=std::max(d,1e-5f);scores.push_back({1.f/std::pow(d,std::max(.1f,o.falloff_power)),bi});}
  size_t n=std::min<size_t>(4,std::min(o.max_influences,scores.size()));std::partial_sort(scores.begin(),scores.begin()+n,scores.end());float sum=0;
  for(size_t k=0;k<n;++k){r.indices[vi][k]=scores[k].second;r.weights[vi][k]=scores[k].first;sum+=scores[k].first;}if(o.normalize_weights&&sum>0)for(size_t k=0;k<n;++k)r.weights[vi][k]/=sum;
 }return r;
}
animation::Vec3 apply_lbs(const SkinVertex&v,const rigging::Skeleton&s){V out{};float sum=0;for(int k=0;k<4;++k)if(v.bone_indices[k]>=0&&v.bone_indices[k]<(int)s.bones.size()&&v.bone_weights[k]>0){auto t=world(s,v.bone_indices[k]);out=add(out,mul(add(t.translation,rotate(t.rotation,{v.position.x*t.scale.x,v.position.y*t.scale.y,v.position.z*t.scale.z})),v.bone_weights[k]));sum+=v.bone_weights[k];}return sum>0?mul(out,1.f/sum):v.position;}
animation::Vec3 apply_dual_quaternion(const SkinVertex&v,const rigging::Skeleton&s){bool scaled=false;for(int k=0;k<4;++k)if(v.bone_indices[k]>=0&&v.bone_indices[k]<(int)s.bones.size()){auto t=world(s,v.bone_indices[k]);scaled|=std::fabs(t.scale.x-1)>1e-4||std::fabs(t.scale.y-1)>1e-4||std::fabs(t.scale.z-1)>1e-4;}if(scaled)return apply_lbs(v,s);animation::Quat q{0,0,0,0},ref{0,0,0,1};V tr{};float sum=0;for(int k=0;k<4;++k)if(v.bone_indices[k]>=0&&v.bone_indices[k]<(int)s.bones.size()&&v.bone_weights[k]>0){auto t=world(s,v.bone_indices[k]);auto x=t.rotation;if(ref.x*x.x+ref.y*x.y+ref.z*x.z+ref.w*x.w<0)x={-x.x,-x.y,-x.z,-x.w};q={q.x+x.x*v.bone_weights[k],q.y+x.y*v.bone_weights[k],q.z+x.z*v.bone_weights[k],q.w+x.w*v.bone_weights[k]};tr=add(tr,mul(t.translation,v.bone_weights[k]));sum+=v.bone_weights[k];}if(sum<=0)return v.position;q=animation::normalize(q);return add(rotate(q,v.position),mul(tr,1.f/sum));}
}