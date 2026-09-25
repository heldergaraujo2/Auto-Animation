#include "auto_animation/importer/FormatDetector.hpp"
#include "auto_animation/importer/ImporterRegistry.hpp"

#include <filesystem>
#include <iostream>
#include <string_view>

namespace {
int failures = 0;
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
    expect(registry.find_for(root / "missing.bmd") == nullptr, "BMD deliberately requires a future proprietary importer");

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

    const auto unsupported = registry.import(root / "unknown.xyz");
    expect(!unsupported, "unsupported extension is rejected");

    const auto missing = registry.import(root / "missing.obj");
    expect(!missing, "missing file is rejected");

    const auto format = auto_animation::importer::detect_format(root / "missing.fbx");
    expect(format == auto_animation::importer::AssetFormat::FBX, "format detector recognizes FBX");

    return failures;
}
