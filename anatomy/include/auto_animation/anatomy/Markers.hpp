#pragma once

#include "auto_animation/animation/Animation.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace auto_animation::anatomy {

enum class MarkerType : std::uint16_t {
    Root, Pelvis, Spine, Chest, Neck, Head,
    ShoulderLeft, ElbowLeft, WristLeft, HandLeft,
    ShoulderRight, ElbowRight, WristRight, HandRight,
    HipLeft, KneeLeft, AnkleLeft, FootLeft,
    HipRight, KneeRight, AnkleRight, FootRight,
    WingRootLeft, WingJointLeft, WingTipLeft,
    WingRootRight, WingJointRight, WingTipRight,
    TailRoot, TailSegment, TailTip,
    Jaw, Horn, Antenna, Fin, TentacleRoot, TentacleTip,
    Custom
};

enum class MarkerSpace : std::uint8_t { Object, Surface };

struct AnatomicalMarker {
    MarkerType type = MarkerType::Custom;
    std::string name;
    animation::Vec3 position{};
    animation::Vec3 normal{};
    MarkerSpace space = MarkerSpace::Object;
    std::int32_t bone_hint = -1;
    float confidence = 1.0f;
    bool required = false;
};

struct MarkerIssue {
    enum class Severity : std::uint8_t { Warning, Error };
    Severity severity = Severity::Error;
    std::string message;
};

struct MarkerValidationResult {
    std::vector<MarkerIssue> issues;
    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] bool has_warnings() const noexcept;
};

struct MarkerSet {
    std::string profile;
    std::vector<AnatomicalMarker> markers;

    bool add_or_replace(AnatomicalMarker marker);
    bool remove(MarkerType type, const std::string& name = {});
    [[nodiscard]] const AnatomicalMarker* find(MarkerType type, const std::string& name = {}) const noexcept;
    [[nodiscard]] AnatomicalMarker* find(MarkerType type, const std::string& name = {}) noexcept;
    [[nodiscard]] MarkerValidationResult validate() const;
    [[nodiscard]] std::string serialize() const;
    static bool deserialize(const std::string& text, MarkerSet& out, std::string& error);
};

[[nodiscard]] const char* marker_type_name(MarkerType) noexcept;
[[nodiscard]] bool marker_type_from_name(const std::string&, MarkerType&) noexcept;

} // namespace auto_animation::anatomy
