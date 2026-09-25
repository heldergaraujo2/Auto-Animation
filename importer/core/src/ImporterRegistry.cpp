#include "auto_animation/importer/ImporterRegistry.hpp"
#include "auto_animation/importer/FormatDetector.hpp"

#include <algorithm>

namespace auto_animation::importer {

void ImporterRegistry::register_importer(std::shared_ptr<const IImporter> importer) {
    if (importer != nullptr) {
        importers_.push_back(std::move(importer));
    }
}

const IImporter* ImporterRegistry::find_for(const std::filesystem::path& path) const noexcept {
    const std::string extension = path.extension().string();
    for (const auto& importer : importers_) {
        if (importer->supports_extension(extension)) {
            return importer.get();
        }
    }
    return nullptr;
}

ImportResult ImporterRegistry::import(
    const std::filesystem::path& path,
    const ImportOptions& options) const {
    const auto* importer = find_for(path);
    if (importer == nullptr) {
        return {{}, "No importer registered for extension: " + path.extension().string()};
    }
    return importer->import_file(path, options);
}

std::vector<std::string> ImporterRegistry::supported_formats() const {
    std::vector<std::string> result;
    for (const auto& importer : importers_) {
        result.emplace_back(importer->name());
    }
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

} // namespace auto_animation::importer
