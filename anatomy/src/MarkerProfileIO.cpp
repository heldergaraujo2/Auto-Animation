#include "auto_animation/anatomy/MarkerProfileIO.hpp"

#include <fstream>
#include <sstream>

namespace auto_animation::anatomy {

bool save_marker_profile(const MarkerSet& markers,
                         const std::string& path,
                         std::string& error) {
    if (path.empty()) {
        error = "Marker profile path is empty.";
        return false;
    }
    const auto validation = markers.validate();
    if (!validation.valid()) {
        error = validation.issues.empty() ? "Marker profile is invalid." : validation.issues.front().message;
        return false;
    }
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) {
        error = "Could not open marker profile for writing: " + path;
        return false;
    }
    file << markers.serialize();
    if (!file.good()) {
        error = "Could not write marker profile: " + path;
        return false;
    }
    return true;
}

bool load_marker_profile(const std::string& path,
                         MarkerSet& out,
                         std::string& error) {
    if (path.empty()) {
        error = "Marker profile path is empty.";
        return false;
    }
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        error = "Could not open marker profile: " + path;
        return false;
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    if (!file.good() && !file.eof()) {
        error = "Could not read marker profile: " + path;
        return false;
    }
    return MarkerSet::deserialize(buffer.str(), out, error);
}

} // namespace auto_animation::anatomy
