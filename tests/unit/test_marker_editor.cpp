#include "auto_animation/anatomy/MarkerEditor.hpp"

#include <cmath>
#include <iostream>
#include <string_view>

namespace {
int failures = 0;
void expect(bool value, std::string_view message) {
    if (!value) { std::cerr << "FAIL: " << message << '\n'; ++failures; }
}
}

int main() {
    using namespace auto_animation::anatomy;
    using auto_animation::animation::Vec3;

    MarkerSet markers;
    markers.profile = "Humanoid";
    MarkerEditor editor(&markers);

    expect(editor.create(MarkerType::Head, {0, 2, 0}), "create marker");
    expect(editor.has_selection(), "new marker selected");
    expect(editor.nudge_selected({1, -0.5f, 2}), "nudge selected");
    expect(std::abs(editor.selected()->position.x - 1.0f) < 1e-5f, "nudge x");
    expect(editor.set_normal_selected({0, 0, 2}), "set normal");
    expect(std::abs(editor.selected()->normal.z - 1.0f) < 1e-5f, "normal normalized");
    expect(editor.set_confidence_selected(2.0f), "confidence clamps");
    expect(editor.selected()->confidence == 1.0f, "confidence clamp upper");
    expect(editor.set_required_selected(true), "required flag");
    expect(editor.mirror_selected(), "mirror selected");
    expect(std::abs(editor.selected()->position.x + 1.0f) < 1e-5f, "mirror x");
    expect(editor.select(MarkerType::Head), "select by type");
    expect(editor.remove_selected(), "remove selected");
    expect(markers.markers.empty(), "marker removed");

    expect(editor.create(MarkerType::FootLeft, {-1, 0, 0}), "left foot");
    expect(editor.create(MarkerType::FootRight, {1, 0, 0}), "right foot");
    expect(editor.mirror_all(), "mirror all");
    expect(markers.markers[0].position.x > 0.0f, "left marker mirrored");
    expect(markers.markers[1].position.x < 0.0f, "right marker mirrored");

    expect(editor.validate().valid(), "edited marker set validates");
    return failures;
}
