#pragma once

#include "auto_animation/importer/Asset.hpp"

#include <filesystem>
#include <string>
#include <string_view>

namespace auto_animation::importer {

struct ImportOptions {
    bool load_materials = true;
    bool load_skeleton = true;
    bool load_animations = true;
    bool triangulate = true;
    bool generate_normals = true;
    bool flip_uvs = false;
};

struct ImportResult {
    Asset asset;
    std::string error;

    [[nodiscard]] explicit operator bool() const noexcept {
        return error.empty() && asset.has_geometry();
    }
};

class IImporter {
public:
    virtual ~IImporter() = default;
    [[nodiscard]] virtual std::string_view name() const noexcept = 0;
    [[nodiscard]] virtual bool supports_extension(std::string_view extension) const noexcept = 0;
    [[nodiscard]] virtual ImportResult import_file(
        const std::filesystem::path& path,
        const ImportOptions& options = {}) const = 0;
};

} // namespace auto_animation::importer
