#pragma once
#include "auto_animation/importer/Importer.hpp"
namespace auto_animation::importer {
class BmdImporter final : public IImporter {
public:
    [[nodiscard]] std::string_view name() const noexcept override { return "MU Online BMD"; }
    [[nodiscard]] bool supports_extension(std::string_view extension) const noexcept override;
    [[nodiscard]] ImportResult import_file(const std::filesystem::path&, const ImportOptions& = {}) const override;
};
} // namespace auto_animation::importer
