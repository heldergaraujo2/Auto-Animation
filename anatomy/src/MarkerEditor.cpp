#include "auto_animation/anatomy/MarkerEditor.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

namespace auto_animation::anatomy {

namespace {
constexpr std::size_t kNoSelection = static_cast<std::size_t>(-1);

animation::Vec3 normalize_or_default(animation::Vec3 v) {
    const float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (!std::isfinite(length) || length <= 1e-6f) return {0.0f, 1.0f, 0.0f};
    return {v.x / length, v.y / length, v.z / length};
}
}

MarkerEditor::MarkerEditor(MarkerSet* markers) noexcept : markers_(markers) {}

void MarkerEditor::attach(MarkerSet* markers) noexcept {
    markers_ = markers;
    selected_index_ = kNoSelection;
}

MarkerSet* MarkerEditor::markers() noexcept { return markers_; }
const MarkerSet* MarkerEditor::markers() const noexcept { return markers_; }

void MarkerEditor::normalize_selection() noexcept {
    if (markers_ == nullptr || selected_index_ >= markers_->markers.size())
        selected_index_ = kNoSelection;
}

bool MarkerEditor::select(MarkerType type, const std::string& name) {
    if (markers_ == nullptr) return false;
    for (std::size_t i = 0; i < markers_->markers.size(); ++i) {
        const auto& m = markers_->markers[i];
        if (m.type == type && (name.empty() || m.name == name)) {
            selected_index_ = i;
            active_type_ = type;
            active_name_ = m.name;
            return true;
        }
    }
    return false;
}

void MarkerEditor::clear_selection() noexcept { selected_index_ = kNoSelection; }

std::size_t MarkerEditor::selected_index() const noexcept {
    return selected_index_;
}

bool MarkerEditor::has_selection() const noexcept {
    return markers_ != nullptr && selected_index_ < markers_->markers.size();
}

AnatomicalMarker* MarkerEditor::selected() noexcept {
    normalize_selection();
    return has_selection() ? &markers_->markers[selected_index_] : nullptr;
}

const AnatomicalMarker* MarkerEditor::selected() const noexcept {
    return has_selection() ? &markers_->markers[selected_index_] : nullptr;
}

bool MarkerEditor::create(MarkerType type, const animation::Vec3& position,
                          const animation::Vec3& normal, std::string name, bool required) {
    if (markers_ == nullptr) return false;
    AnatomicalMarker marker;
    marker.type = type;
    marker.name = name.empty() ? marker_type_name(type) : std::move(name);
    marker.position = position;
    marker.normal = normalize_or_default(normal);
    marker.space = MarkerSpace::Object;
    marker.confidence = 1.0f;
    marker.required = required;
    const std::string lookup_name = marker.name;
    if (!markers_->add_or_replace(std::move(marker))) return false;
    const auto* created = markers_->find(type, lookup_name);
    if (created == nullptr) return false;
    active_type_ = type;
    active_name_ = created->name;
    for (std::size_t i = 0; i < markers_->markers.size(); ++i) {
        if (&markers_->markers[i] == created) {
            selected_index_ = i;
            break;
        }
    }
    return true;
}

bool MarkerEditor::remove_selected() {
    if (!has_selection()) return false;
    const auto type = markers_->markers[selected_index_].type;
    const auto name = markers_->markers[selected_index_].name;
    const bool removed = markers_->remove(type, name);
    if (removed) {
        selected_index_ = kNoSelection;
    }
    return removed;
}

bool MarkerEditor::move_selected(const animation::Vec3& position) {
    auto* marker = selected();
    if (marker == nullptr) return false;
    marker->position = position;
    return true;
}

bool MarkerEditor::nudge_selected(const animation::Vec3& delta) {
    auto* marker = selected();
    if (marker == nullptr) return false;
    marker->position.x += delta.x;
    marker->position.y += delta.y;
    marker->position.z += delta.z;
    return true;
}

bool MarkerEditor::set_normal_selected(const animation::Vec3& normal) {
    auto* marker = selected();
    if (marker == nullptr) return false;
    marker->normal = normalize_or_default(normal);
    return true;
}

bool MarkerEditor::set_confidence_selected(float confidence) {
    auto* marker = selected();
    if (marker == nullptr || !std::isfinite(confidence)) return false;
    marker->confidence = std::clamp(confidence, 0.0f, 1.0f);
    return true;
}

bool MarkerEditor::set_required_selected(bool required) {
    auto* marker = selected();
    if (marker == nullptr) return false;
    marker->required = required;
    return true;
}

bool MarkerEditor::mirror_selected() {
    auto* marker = selected();
    if (marker == nullptr) return false;
    marker->position.x = -marker->position.x;
    marker->normal.x = -marker->normal.x;
    return true;
}

bool MarkerEditor::mirror_all() {
    if (markers_ == nullptr) return false;
    for (auto& marker : markers_->markers) {
        marker.position.x = -marker.position.x;
        marker.normal.x = -marker.normal.x;
    }
    return true;
}

MarkerType MarkerEditor::active_type() const noexcept { return active_type_; }
void MarkerEditor::set_active_type(MarkerType type) noexcept { active_type_ = type; }

std::string MarkerEditor::active_name() const { return active_name_; }
void MarkerEditor::set_active_name(std::string name) { active_name_ = std::move(name); }

MarkerValidationResult MarkerEditor::validate() const {
    return markers_ != nullptr ? markers_->validate() : MarkerValidationResult{};
}

} // namespace auto_animation::anatomy
