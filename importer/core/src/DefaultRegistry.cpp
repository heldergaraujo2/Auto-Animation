#include "auto_animation/importer/ImporterRegistry.hpp"
#include "auto_animation/importer/AssimpImporter.hpp"

namespace auto_animation::importer {

ImporterRegistry create_default_registry() {
    ImporterRegistry registry;
    registry.register_importer(std::make_shared<AssimpImporter>());
    return registry;
}

} // namespace auto_animation::importer
