#include "auto_animation/importer/FormatDetector.hpp"
#include "auto_animation/importer/ImporterRegistry.hpp"

#include <filesystem>
#include <fstream>
#include <vector>
#include <cstdint>
#include <array>
#include <cstring>

#include <iostream>
#include <string_view>

namespace {
int failures = 0;
void put_u8(std::vector<std::uint8_t>& b, std::uint8_t v){b.push_back(v);}
void put_s16(std::vector<std::uint8_t>& b, std::int16_t v){b.push_back(static_cast<std::uint8_t>(v));b.push_back(static_cast<std::uint8_t>(v>>8));}
void put_u16(std::vector<std::uint8_t>& b, std::uint16_t v){b.push_back(static_cast<std::uint8_t>(v));b.push_back(static_cast<std::uint8_t>(v>>8));}
void put_f32(std::vector<std::uint8_t>& b, float v){std::uint32_t u=0;std::memcpy(&u,&v,4);for(int i=0;i<4;++i)b.push_back(static_cast<std::uint8_t>(u>>(i*8)));}
void fixed(std::vector<std::uint8_t>& b,std::string_view s,std::size_t n){for(std::size_t i=0;i<n;++i)b.push_back(i<s.size()?static_cast<std::uint8_t>(s[i]):0);}
std::filesystem::path make_test_bmd(){
    std::vector<std::uint8_t> b={'B','M','D',11};
    fixed(b,"TestBMD",32); put_u16(b,1);put_u16(b,2);put_u16(b,1);
    put_s16(b,3);put_s16(b,3);put_s16(b,3);put_s16(b,1);put_s16(b,0);
    for(auto p:std::vector<std::array<float,3>>{{{0,0,0}},{{1,0,0}},{{0,1,0}}}){put_s16(b,0);put_s16(b,0);put_f32(b,p[0]);put_f32(b,p[1]);put_f32(b,p[2]);}
    for(int i=0;i<3;++i){put_s16(b,0);put_s16(b,0);put_f32(b,0);put_f32(b,0);put_f32(b,1);put_s16(b,0);put_s16(b,0);}
    for(auto uv:std::vector<std::array<float,2>>{{{0,0}},{{1,0}},{{0,1}}}){put_f32(b,uv[0]);put_f32(b,uv[1]);}
    std::array<std::int16_t,4> vi{0,1,2,0};
    std::array<std::int16_t,4> ni{0,0,0,0};
    std::array<std::int16_t,4> ti{0,1,2,0};
    put_u8(b,3);put_u8(b,0);for(auto v:vi)put_s16(b,v);for(auto v:ni)put_s16(b,v);for(auto v:ti)put_s16(b,v);for(int i=0;i<38;++i)put_u8(b,0);
    fixed(b,"texture.tga",32);
    put_s16(b,2);put_u8(b,0);
    for(std::string_view name:{"Root","Child"}){put_u8(b,0);fixed(b,name,32);put_s16(b,name=="Root"?-1:0);for(int k=0;k<2;++k){put_f32(b,0);put_f32(b,0);put_f32(b,0);}for(int k=0;k<2;++k){put_f32(b,0);put_f32(b,0);put_f32(b,0);}}
    const auto path=std::filesystem::temp_directory_path()/"auto_animation_test.bmd";
    std::ofstream out(path,std::ios::binary);out.write(reinterpret_cast<const char*>(b.data()),static_cast<std::streamsize>(b.size()));return path;
}
void expect(bool value, std::string_view message) {
    if (!value) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: test_importer <fixtures-dir>\n";
        return 2;
    }

    const auto root = std::filesystem::path(argv[1]);
    const auto registry = auto_animation::importer::create_default_registry();

    expect(registry.find_for(root / "cube.obj") != nullptr, "OBJ importer registered");
    expect(registry.find_for(root / "triangle.gltf") != nullptr, "GLTF importer registered");
    expect(registry.find_for(root / "triangle.smd") != nullptr, "SMD importer registered");
    expect(registry.find_for(root / "missing.fbx") != nullptr, "FBX importer registered");
    expect(registry.find_for(root / "missing.dae") != nullptr, "DAE importer registered");
    expect(registry.find_for(root / "missing.3ds") != nullptr, "3DS importer registered");
    expect(registry.find_for(root / "triangle.stl") != nullptr, "STL importer registered");
    expect(registry.find_for(root / "triangle.ply") != nullptr, "PLY importer registered");
    expect(registry.find_for(root / "missing.bmd") != nullptr, "BMD importer registered");

    const auto obj = registry.import(root / "cube.obj");
    expect(static_cast<bool>(obj), "OBJ import succeeds");
    expect(obj.asset.meshes.size() == 1, "OBJ produces one mesh");
    expect(obj.asset.meshes[0].indices.size() == 6, "OBJ produces two triangles");

    const auto fbx = registry.import(root / "cubes_nonames.fbx");
    if (!fbx) std::cerr << "FBX ERROR: " << fbx.error << "\\n";
    expect(static_cast<bool>(fbx), "FBX import succeeds");
    expect(!fbx.asset.meshes.empty(), "FBX produces geometry");

    const auto gltf = registry.import(root / "triangle.gltf");
    if (!gltf) std::cerr << "GLTF ERROR: " << gltf.error << "\\n";
    expect(static_cast<bool>(gltf), "GLTF import succeeds");
    expect(!gltf.asset.meshes.empty(), "GLTF produces geometry");

    const auto smd = registry.import(root / "triangle.smd");
    if (!smd) std::cerr << "SMD ERROR: " << smd.error << "\\n";
    expect(static_cast<bool>(smd), "SMD import succeeds");
    expect(!smd.asset.meshes.empty(), "SMD produces geometry");

    const auto rigged = registry.import(root / "rigged.smd");
    if (!rigged) std::cerr << "RIGGED SMD ERROR: " << rigged.error << "\\n";
    expect(static_cast<bool>(rigged), "rigged SMD import succeeds");
    expect(rigged.asset.has_skeleton(), "rigged SMD produces skeleton");
    if (rigged.asset.has_skeleton()) {
        expect(rigged.asset.skeletons[0].find_bone("root") >= 0, "imported skeleton contains root");
        expect(rigged.asset.skeletons[0].find_bone("arm") >= 0, "imported skeleton contains child bone");
        expect(rigged.asset.skeletons[0].validate().valid(), "imported skeleton validates");
    }

    const auto stl = registry.import(root / "triangle.stl");
    expect(static_cast<bool>(stl), "STL import succeeds");

    const auto ply = registry.import(root / "triangle.ply");
    expect(static_cast<bool>(ply), "PLY import succeeds");

    const auto bmd_path = make_test_bmd();
    const auto bmd = registry.import(bmd_path);
    expect(static_cast<bool>(bmd), "native unencrypted BMD import succeeds");
    expect(bmd.asset.has_geometry(), "BMD produces geometry");
    expect(bmd.asset.has_skeleton(), "BMD produces skeleton");
    expect(bmd.asset.has_animation(), "BMD produces animation");
    if (bmd.asset.has_skeleton()) expect(bmd.asset.skeletons[0].find_bone("Child") == 1, "BMD hierarchy preserves child bone");
    std::error_code remove_error; std::filesystem::remove(bmd_path, remove_error);

    const auto unsupported = registry.import(root / "unknown.xyz");
    expect(!unsupported, "unsupported extension is rejected");

    const auto missing = registry.import(root / "missing.obj");
    expect(!missing, "missing file is rejected");

    const auto format = auto_animation::importer::detect_format(root / "missing.fbx");
    expect(format == auto_animation::importer::AssetFormat::FBX, "format detector recognizes FBX");

    return failures;
}
