#pragma once

#include "auto_animation/anatomy/Markers.hpp"

#include <cstddef>
#include <string>

namespace auto_animation::anatomy {

class MarkerEditor {
public:
    explicit MarkerEditor(MarkerSet* markers = nullptr) noexcept;

    void attach(MarkerSet* markers) noexcept;
    [[nodiscard]] MarkerSet* markers() noexcept;
    [[nodiscard]] const MarkerSet* markers() const noexcept;

    bool select(MarkerType type, const std::string& name = {});
    void clear_selection() noexcept;
    [[nodiscard]] std::size_t selected_index() const noexcept;
    [[nodiscard]] bool has_selection() const noexcept;
    [[nodiscard]] AnatomicalMarker* selected() noexcept;
    [[nodiscard]] const AnatomicalMarker* selected() const noexcept;

    bool create(MarkerType type, const animation::Vec3& position,
                const animation::Vec3& normal = {0.0f, 1.0f, 0.0f},
                std::string name = {}, bool required = false);
    bool remove_selected();
    bool move_selected(const animation::Vec3& position);
    bool nudge_selected(const animation::Vec3& delta);
    bool set_normal_selected(const animation::Vec3& normal);
    bool set_confidence_selected(float confidence);
    bool set_required_selected(bool required);

    bool mirror_selected();
    bool mirror_all();

    [[nodiscard]] MarkerType active_type() const noexcept;
    void set_active_type(MarkerType type) noexcept;
    [[nodiscard]] std::string active_name() const;
    void set_active_name(std::string name);

    [[nodiscard]] MarkerValidationResult validate() const;

private:
    MarkerSet* markers_ = nullptr;
    std::size_t selected_index_ = static_cast<std::size_t>(-1);
    MarkerType active_type_ = MarkerType::Head;
    std::string active_name_;

    void normalize_selection() noexcept;
};

} // namespace auto_animation::anatomy
