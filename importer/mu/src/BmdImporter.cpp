#include "auto_animation/importer/BmdImporter.hpp"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <cmath>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

namespace auto_animation::importer {
namespace {
class Reader {
public:
    explicit Reader(const std::vector<std::uint8_t>& bytes) : bytes_(bytes) {}
    bool read_u8(std::uint8_t& v) { if (!need(1)) return false; v=bytes_[off_++]; return true; }
    bool read_s16(std::int16_t& v) { if (!need(2)) return false; v=static_cast<std::int16_t>(bytes_[off_]|(bytes_[off_+1]<<8)); off_+=2; return true; }
    bool read_u16(std::uint16_t& v) { if (!need(2)) return false; v=static_cast<std::uint16_t>(bytes_[off_]|(bytes_[off_+1]<<8)); off_+=2; return true; }
    bool read_f32(float& v) { if (!need(4)) return false; std::uint32_t u=bytes_[off_]|(bytes_[off_+1]<<8)|(bytes_[off_+2]<<16)|(bytes_[off_+3]<<24); off_+=4; std::memcpy(&v,&u,4); return true; }
    bool read_bytes(std::size_t n,std::vector<std::uint8_t>& out){if(!need(n))return false;out.insert(out.end(),bytes_.begin()+static_cast<std::ptrdiff_t>(off_),bytes_.begin()+static_cast<std::ptrdiff_t>(off_+n));off_+=n;return true;}
    bool skip(std::size_t n){if(!need(n))return false;off_+=n;return true;}
    std::size_t offset() const noexcept { return off_; }
    bool string_fixed(std::size_t n,std::string& out){if(!need(n))return false;auto first=bytes_.begin()+static_cast<std::ptrdiff_t>(off_);auto last=first+static_cast<std::ptrdiff_t>(n);auto zero=std::find(first,last,std::uint8_t{0});out.assign(first,zero);off_+=n;return true;}
private:
    bool need(std::size_t n) const noexcept{return n<=bytes_.size()-off_;}
    const std::vector<std::uint8_t>& bytes_; std::size_t off_=0;
};

struct Vec3 { float x=0,y=0,z=0; };
struct Key { Vec3 position{}; Vec3 rotation{}; };
struct BmdBone { std::string name; std::int16_t parent=-1; bool dummy=false; std::vector<std::vector<Key>> actions; };

constexpr std::size_t kMaxMeshes=4096,kMaxBones=4096,kMaxActions=4096,kMaxElements=1000000,kMaxKeys=100000;
bool finite(Vec3 v){return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}
animation::Quat euler_quat(Vec3 e) {
    const float hx=e.x*0.5f,hy=e.y*0.5f,hz=e.z*0.5f;
    const float sx=std::sin(hx),cx=std::cos(hx),sy=std::sin(hy),cy=std::cos(hy),sz=std::sin(hz),cz=std::cos(hz);
    return animation::normalize({sx*cy*cz-cx*sy*sz,cx*sy*cz+sx*cy*sz,cx*cy*sz-sx*sy*cz,cx*cy*cz+sx*sy*sz});
}
std::string lower(std::string v){std::transform(v.begin(),v.end(),v.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});return v;}
bool count_ok(std::uint32_t n,std::size_t max){return n<=max;}
}
bool BmdImporter::supports_extension(std::string_view extension) const noexcept {
    std::string e(extension); e=lower(e); return e==".bmd";
}
ImportResult BmdImporter::import_file(const std::filesystem::path& path,const ImportOptions& options) const {
    std::ifstream file(path,std::ios::binary);
    if(!file)return {{}, "File does not exist or cannot be opened: "+path.string()};
    std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(file)),{});
    if(bytes.size()<4)return {{}, "BMD file is too small."};
    if(bytes[0]!='B'||bytes[1]!='M'||bytes[2]!='D')return {{}, "Invalid BMD header."};
    const std::uint8_t version=bytes[3];
    if(version==12||version==15)return {{}, "Encrypted BMD versions 12/15 are recognized but require the optional MU cryptography adapter."};
    Reader r(bytes);
    std::uint8_t header[4]{};for(auto&v:header)if(!r.read_u8(v))return{{},"Truncated BMD header."};
    std::string name;std::uint16_t mesh_count=0,bone_count=0,action_count=0;
    if(!r.string_fixed(32,name)||!r.read_u16(mesh_count)||!r.read_u16(bone_count)||!r.read_u16(action_count))return{{},"Truncated BMD header fields."};
    if(!count_ok(mesh_count,kMaxMeshes)||!count_ok(bone_count,kMaxBones)||!count_ok(action_count,kMaxActions))return{{},"BMD count exceeds safety limit."};

    Asset asset;asset.source_path=path.string();asset.source_format=".bmd";
    std::vector<std::int16_t> mesh_nodes;
    for(std::uint32_t mi=0;mi<mesh_count;++mi){
        std::int16_t nv=0,nn=0,nt=0,ntri=0,texture=0;
        if(!r.read_s16(nv)||!r.read_s16(nn)||!r.read_s16(nt)||!r.read_s16(ntri)||!r.read_s16(texture))return{{},"Truncated BMD mesh header."};
        if(nv<0||nn<0||nt<0||ntri<0||!count_ok(static_cast<std::uint32_t>(nv),kMaxElements)||!count_ok(static_cast<std::uint32_t>(nn),kMaxElements)||!count_ok(static_cast<std::uint32_t>(nt),kMaxElements)||!count_ok(static_cast<std::uint32_t>(ntri),kMaxElements))return{{},"Invalid BMD mesh counts."};
        struct V{std::int16_t node;Vec3 p;};struct N{std::int16_t node;Vec3 n;std::int16_t bind;};struct UV{float u,v;};
        std::vector<V> vs(static_cast<std::size_t>(nv));std::vector<N> ns(static_cast<std::size_t>(nn));std::vector<UV> uvs(static_cast<std::size_t>(nt));
        for(auto&v:vs){std::int16_t pad;if(!r.read_s16(v.node)||!r.read_s16(pad)||!r.read_f32(v.p.x)||!r.read_f32(v.p.y)||!r.read_f32(v.p.z)||!finite(v.p))return{{},"Invalid BMD vertex data."};}
        for(auto&n:ns){std::int16_t pad;if(!r.read_s16(n.node)||!r.read_s16(pad)||!r.read_f32(n.n.x)||!r.read_f32(n.n.y)||!r.read_f32(n.n.z)||!r.read_s16(n.bind)||!r.read_s16(pad)||!finite(n.n))return{{},"Invalid BMD normal data."};}
        for(auto&uv:uvs)if(!r.read_f32(uv.u)||!r.read_f32(uv.v)||!std::isfinite(uv.u)||!std::isfinite(uv.v))return{{},"Invalid BMD UV data."};
        struct Tri{std::uint8_t polygon;std::array<std::int16_t,4> v,n,t;};
        std::vector<Tri> tris;tris.reserve(static_cast<std::size_t>(ntri));
        for(std::int32_t ti=0;ti<ntri;++ti){std::vector<std::uint8_t> raw;if(!r.read_bytes(64,raw))return{{},"Truncated BMD triangle data."};Tri tri{};tri.polygon=raw[0];if(tri.polygon!=3&&tri.polygon!=4)return{{},"Invalid BMD polygon size."};for(int i=0;i<4;++i){auto at=[&](std::size_t o){return static_cast<std::int16_t>(static_cast<std::uint16_t>(raw[o])|(static_cast<std::uint16_t>(raw[o+1])<<8));};tri.v[i]=at(2+i*2);tri.n[i]=at(10+i*2);tri.t[i]=at(18+i*2);}tris.push_back(tri);}
        std::string texture_path;if(!r.string_fixed(32,texture_path))return{{},"Truncated BMD texture path."};
        if(options.load_materials){Material m;m.name=texture_path;m.diffuse_texture=texture_path;asset.materials.push_back(std::move(m));}
        Mesh mesh;mesh.name=name+"_mesh_"+std::to_string(mi);mesh.material_index=options.load_materials?static_cast<std::uint32_t>(asset.materials.size()-1):0;
        auto emit=[&](const Tri&tri,int i){if(i<0||i>=4||tri.v[i]<0||static_cast<std::size_t>(tri.v[i])>=vs.size()||tri.n[i]<0||static_cast<std::size_t>(tri.n[i])>=ns.size()||tri.t[i]<0||static_cast<std::size_t>(tri.t[i])>=uvs.size())return;Vertex v;v.position={vs[tri.v[i]].p.x,vs[tri.v[i]].p.y,vs[tri.v[i]].p.z};v.normal={ns[tri.n[i]].n.x,ns[tri.n[i]].n.y,ns[tri.n[i]].n.z};v.uv={uvs[tri.t[i]].u,uvs[tri.t[i]].v};if(vs[tri.v[i]].node>=0&&static_cast<std::uint16_t>(vs[tri.v[i]].node)<bone_count){v.bone_indices[0]=static_cast<std::uint32_t>(vs[tri.v[i]].node);v.bone_weights[0]=1.0f;}mesh.vertices.push_back(v);mesh.indices.push_back(static_cast<std::uint32_t>(mesh.vertices.size()-1));};
        for(const auto&tri:tris){emit(tri,0);emit(tri,2);emit(tri,1);if(tri.polygon==4){emit(tri,0);emit(tri,2);emit(tri,3);}}
        asset.meshes.push_back(std::move(mesh));
    }
    std::vector<std::uint16_t> action_keys(action_count);
    std::vector<bool> action_locked(action_count,false);
    for(std::uint32_t ai=0;ai<action_count;++ai){std::int16_t keys=0;std::uint8_t locked=0;if(!r.read_s16(keys)||!r.read_u8(locked)||keys<0||!count_ok(static_cast<std::uint32_t>(keys),kMaxKeys))return{{},"Invalid BMD action count."};action_keys[ai]=static_cast<std::uint16_t>(keys);action_locked[ai]=locked!=0;if(action_locked[ai]&&!r.skip(static_cast<std::size_t>(keys)*12))return{{},"Truncated BMD locked action positions."};}
    std::vector<BmdBone> bones(bone_count);
    for(std::uint32_t bi=0;bi<bone_count;++bi){
        std::uint8_t dummy=0;if(!r.read_u8(dummy))return{{},"Truncated BMD bone header."};bones[bi].dummy=dummy!=0;
        if(bones[bi].dummy){bones[bi].name="dummy_"+std::to_string(bi);bones[bi].parent=-1;bones[bi].actions.resize(action_count);continue;}
        if(!r.string_fixed(32,bones[bi].name)||!r.read_s16(bones[bi].parent)||bones[bi].parent< -1||bones[bi].parent>=static_cast<std::int16_t>(bone_count))return{{},"Invalid BMD bone hierarchy."};
        bones[bi].actions.resize(action_count);
        for(std::uint32_t ai=0;ai<action_count;++ai){const std::size_t keys=action_keys[ai];bones[bi].actions[ai].resize(keys);for(auto&k:bones[bi].actions[ai])if(!r.read_f32(k.position.x)||!r.read_f32(k.position.y)||!r.read_f32(k.position.z)||!finite(k.position))return{{},"Truncated BMD bone positions."};for(auto&k:bones[bi].actions[ai])if(!r.read_f32(k.rotation.x)||!r.read_f32(k.rotation.y)||!r.read_f32(k.rotation.z)||!finite(k.rotation))return{{},"Truncated BMD bone rotations."};}
    }
    if(options.load_skeleton&&bone_count>0){Skeleton s;s.name=name;for(std::uint32_t bi=0;bi<bone_count;++bi){Bone b;b.name=bones[bi].name;b.parent_index=bones[bi].parent;if(!bones[bi].actions.empty()&&!bones[bi].actions[0].empty()){b.bind_local.translation={bones[bi].actions[0][0].position.x,bones[bi].actions[0][0].position.y,bones[bi].actions[0][0].position.z};b.bind_local.rotation=euler_quat(bones[bi].actions[0][0].rotation);b.local_pose=b.bind_local;}s.bones.push_back(std::move(b));}asset.skeletons.push_back(std::move(s));}
    if(options.load_animations){for(std::uint32_t ai=0;ai<action_count;++ai){if(action_keys[ai]<2)continue;AnimationClip clip;clip.name="action_"+std::to_string(ai);clip.sample_rate=24.0;clip.duration_seconds=(action_keys[ai]-1)/24.0;clip.looping=false;for(std::uint32_t bi=0;bi<bone_count;++bi){if(bones[bi].dummy||bones[bi].actions[ai].empty())continue;AnimationTrack track;track.bone_name=bones[bi].name;track.bone_index=static_cast<std::int32_t>(bi);for(std::size_t k=0;k<bones[bi].actions[ai].size();++k){const double t=static_cast<double>(k)/24.0;const auto&key=bones[bi].actions[ai][k];track.translation.keys.push_back({t,{key.position.x,key.position.y,key.position.z},animation::Interpolation::Linear});track.rotation.keys.push_back({t,euler_quat(key.rotation),animation::Interpolation::Spherical});}clip.tracks.push_back(std::move(track));}asset.animations.push_back(std::move(clip));}}
    return {std::move(asset),{}};
}
} // namespace auto_animation::importer
