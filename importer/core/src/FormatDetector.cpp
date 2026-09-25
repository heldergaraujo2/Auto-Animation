#include "auto_animation/importer/FormatDetector.hpp"

#include <algorithm>
#include <cctype>

namespace auto_animation::importer {

AssetFormat detect_format(const std::filesystem::path& path) noexcept {
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (ext == ".fbx") return AssetFormat::FBX;
    if (ext == ".gltf") return AssetFormat::GLTF;
    if (ext == ".glb") return AssetFormat::GLB;
    if (ext == ".obj") return AssetFormat::OBJ;
    if (ext == ".smd") return AssetFormat::SMD;
    if (ext == ".dae") return AssetFormat::DAE;
    if (ext == ".3ds") return AssetFormat::ThreeDS;
    if (ext == ".stl") return AssetFormat::STL;
    if (ext == ".ply") return AssetFormat::PLY;
    if (ext == ".bmd") return AssetFormat::BMD;
    return AssetFormat::Unknown;
}

std::string_view format_name(AssetFormat format) noexcept {
    switch (format) {
        case AssetFormat::FBX: return "FBX";
        case AssetFormat::GLTF: return "GLTF";
        case AssetFormat::GLB: return "GLB";
        case AssetFormat::OBJ: return "OBJ";
        case AssetFormat::SMD: return "SMD";
        case AssetFormat::DAE: return "DAE";
        case AssetFormat::ThreeDS: return "3DS";
        case AssetFormat::STL: return "STL";
        case AssetFormat::PLY: return "PLY";
        case AssetFormat::BMD: return "BMD";
        default: return "Unknown";
    }
}

} // namespace auto_animation::importer
