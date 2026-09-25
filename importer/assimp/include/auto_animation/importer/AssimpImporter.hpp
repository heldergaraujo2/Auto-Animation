#pragma once

#include "auto_animation/importer/Importer.hpp"

#include <string_view>

namespace auto_animation::importer {

class AssimpImporter final : public IImporter {
public:
    [[nodiscard]] std::string_view name() const noexcept override { return "Assimp"; }
    [[nodiscard]] bool supports_extension(std::string_view extension) const noexcept override;
    [[nodiscard]] ImportResult import_file(
        const std::filesystem::path& path,
        const ImportOptions& options = {}) const override;
};

} // namespace auto_animation::importer
