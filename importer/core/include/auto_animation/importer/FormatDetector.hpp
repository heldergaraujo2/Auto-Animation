#pragma once

#include <filesystem>
#include <string_view>

namespace auto_animation::importer {

enum class AssetFormat {
    Unknown,
    FBX,
    GLTF,
    GLB,
    OBJ,
    SMD,
    DAE,
    ThreeDS,
    STL,
    PLY,
    BMD
};

[[nodiscard]] AssetFormat detect_format(const std::filesystem::path& path) noexcept;
[[nodiscard]] std::string_view format_name(AssetFormat format) noexcept;

} // namespace auto_animation::importer
