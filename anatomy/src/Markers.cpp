#include "auto_animation/anatomy/Markers.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace auto_animation::anatomy {
namespace {
bool finite(animation::Vec3 v) noexcept {
    return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
}
bool finite(float v) noexcept { return std::isfinite(v); }

struct MarkerName { MarkerType type; const char* name; };
constexpr MarkerName kNames[] = {
    {MarkerType::Root,"Root"},{MarkerType::Pelvis,"Pelvis"},{MarkerType::Spine,"Spine"},
    {MarkerType::Chest,"Chest"},{MarkerType::Neck,"Neck"},{MarkerType::Head,"Head"},
    {MarkerType::ShoulderLeft,"ShoulderLeft"},{MarkerType::ElbowLeft,"ElbowLeft"},
    {MarkerType::WristLeft,"WristLeft"},{MarkerType::HandLeft,"HandLeft"},
    {MarkerType::ShoulderRight,"ShoulderRight"},{MarkerType::ElbowRight,"ElbowRight"},
    {MarkerType::WristRight,"WristRight"},{MarkerType::HandRight,"HandRight"},
    {MarkerType::HipLeft,"HipLeft"},{MarkerType::KneeLeft,"KneeLeft"},
    {MarkerType::AnkleLeft,"AnkleLeft"},{MarkerType::FootLeft,"FootLeft"},
    {MarkerType::HipRight,"HipRight"},{MarkerType::KneeRight,"KneeRight"},
    {MarkerType::AnkleRight,"AnkleRight"},{MarkerType::FootRight,"FootRight"},
    {MarkerType::WingRootLeft,"WingRootLeft"},{MarkerType::WingJointLeft,"WingJointLeft"},
    {MarkerType::WingTipLeft,"WingTipLeft"},{MarkerType::WingRootRight,"WingRootRight"},
    {MarkerType::WingJointRight,"WingJointRight"},{MarkerType::WingTipRight,"WingTipRight"},
    {MarkerType::TailRoot,"TailRoot"},{MarkerType::TailSegment,"TailSegment"},
    {MarkerType::TailTip,"TailTip"},{MarkerType::Jaw,"Jaw"},{MarkerType::Horn,"Horn"},
    {MarkerType::Antenna,"Antenna"},{MarkerType::Fin,"Fin"},
    {MarkerType::TentacleRoot,"TentacleRoot"},{MarkerType::TentacleTip,"TentacleTip"},
    {MarkerType::Custom,"Custom"}
};
} 

const char* marker_type_name(MarkerType type) noexcept {
    for (const auto& item : kNames) if (item.type == type) return item.name;
    return "Custom";
}

bool marker_type_from_name(const std::string& name, MarkerType& out) noexcept {
    for (const auto& item : kNames) if (name == item.name) { out = item.type; return true; }
    return false;
}

bool MarkerSet::add_or_replace(AnatomicalMarker marker) {
    if (marker.name.empty() && marker.type == MarkerType::Custom) return false;
    for (auto& existing : markers) {
        if (existing.type == marker.type && existing.name == marker.name) {
            existing = std::move(marker);
            return true;
        }
    }
    markers.push_back(std::move(marker));
    return true;
}

bool MarkerSet::remove(MarkerType type, const std::string& name) {
    for (auto it = markers.begin(); it != markers.end(); ++it) {
        if (it->type == type && (name.empty() || it->name == name)) {
            markers.erase(it);
            return true;
        }
    }
    return false;
}

const AnatomicalMarker* MarkerSet::find(MarkerType type, const std::string& name) const noexcept {
    for (const auto& marker : markers)
        if (marker.type == type && (name.empty() || marker.name == name)) return &marker;
    return nullptr;
}
AnatomicalMarker* MarkerSet::find(MarkerType type, const std::string& name) noexcept {
    for (auto& marker : markers)
        if (marker.type == type && (name.empty() || marker.name == name)) return &marker;
    return nullptr;
}

MarkerValidationResult MarkerSet::validate() const {
    MarkerValidationResult result;
    std::vector<std::string> keys;
    for (const auto& marker : markers) {
        const std::string key = std::to_string(static_cast<int>(marker.type)) + ":" + marker.name;
        for (const auto& existing : keys)
            if (existing == key) result.issues.push_back({MarkerIssue::Severity::Error, "Duplicate marker: " + key});
        keys.push_back(key);
        if (!finite(marker.position) || !finite(marker.normal))
            result.issues.push_back({MarkerIssue::Severity::Error, "Marker has non-finite coordinates: " + key});
        if (!finite(marker.confidence) || marker.confidence < 0.0f || marker.confidence > 1.0f)
            result.issues.push_back({MarkerIssue::Severity::Error, "Marker confidence must be in [0,1]: " + key});
        if (marker.required && marker.type == MarkerType::Custom && marker.name.empty())
            result.issues.push_back({MarkerIssue::Severity::Error, "Required custom marker needs a name."});
        if (marker.bone_hint < -1)
            result.issues.push_back({MarkerIssue::Severity::Error, "Marker bone hint is invalid: " + key});
    }
    if (markers.empty())
        result.issues.push_back({MarkerIssue::Severity::Warning, "Marker set is empty."});
    if (profile.empty())
        result.issues.push_back({MarkerIssue::Severity::Warning, "Marker profile is unnamed."});
    return result;
}

std::string MarkerSet::serialize() const {
    std::ostringstream out;
    out << "AUTO_ANIMATION_MARKERS 1\n";
    out << "profile " << std::quoted(profile) << "\n";
    out << std::setprecision(9);
    for (const auto& m : markers) {
        out << "marker " << marker_type_name(m.type) << " " << std::quoted(m.name) << " "
            << static_cast<int>(m.space) << " " << m.position.x << " " << m.position.y << " " << m.position.z << " "
            << m.normal.x << " " << m.normal.y << " " << m.normal.z << " " << m.bone_hint << " "
            << m.confidence << " " << (m.required ? 1 : 0) << "\n";
    }
    return out.str();
}

bool MarkerSet::deserialize(const std::string& text, MarkerSet& out, std::string& error) {
    std::istringstream in(text);
    std::string header, version;
    if (!(in >> header >> version) || header != "AUTO_ANIMATION_MARKERS" || version != "1") {
        error = "Unsupported marker serialization header/version.";
        return false;
    }
    MarkerSet parsed;
    std::string kind;
    while (in >> kind) {
        if (kind == "profile") {
            if (!(in >> std::quoted(parsed.profile))) { error = "Invalid profile line."; return false; }
            continue;
        }
        if (kind != "marker") { error = "Unknown marker record: " + kind; return false; }
        std::string type_name, name;
        int space = 0, required = 0;
        AnatomicalMarker m;
        if (!(in >> type_name >> std::quoted(name) >> space
              >> m.position.x >> m.position.y >> m.position.z
              >> m.normal.x >> m.normal.y >> m.normal.z
              >> m.bone_hint >> m.confidence >> required)) {
            error = "Invalid marker record.";
            return false;
        }
        if (!marker_type_from_name(type_name, m.type)) { error = "Unknown marker type: " + type_name; return false; }
        if (space < 0 || space > 1 || (required != 0 && required != 1)) { error = "Invalid marker flags."; return false; }
        m.name = std::move(name);
        m.space = static_cast<MarkerSpace>(space);
        m.required = required != 0;
        if (!parsed.add_or_replace(std::move(m))) { error = "Could not add marker."; return false; }
    }
    const auto validation = parsed.validate();
    if (!validation.valid()) { error = validation.issues.front().message; return false; }
    out = std::move(parsed);
    return true;
}

bool MarkerValidationResult::valid() const noexcept {
    for (const auto& issue : issues) if (issue.severity == MarkerIssue::Severity::Error) return false;
    return true;
}
bool MarkerValidationResult::has_warnings() const noexcept {
    for (const auto& issue : issues) if (issue.severity == MarkerIssue::Severity::Warning) return true;
    return false;
}

} // namespace auto_animation::anatomy
