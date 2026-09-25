#pragma once

#include "auto_animation/importer/Importer.hpp"

#include <memory>
#include <string>
#include <vector>

namespace auto_animation::importer {

class ImporterRegistry {
public:
    void register_importer(std::shared_ptr<const IImporter> importer);

    [[nodiscard]] const IImporter* find_for(const std::filesystem::path& path) const noexcept;

    [[nodiscard]] ImportResult import(
        const std::filesystem::path& path,
        const ImportOptions& options = {}) const;

    [[nodiscard]] std::vector<std::string> supported_formats() const;

private:
    std::vector<std::shared_ptr<const IImporter>> importers_;
};

[[nodiscard]] ImporterRegistry create_default_registry();

} // namespace auto_animation::importer
