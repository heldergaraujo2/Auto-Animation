#include "auto_animation/anatomy/MarkerProfileIO.hpp"

#include <cstdio>
#include <iostream>
#include <string>

namespace {
int failures = 0;
void expect(bool value, const char* message) {
    if (!value) { std::cerr << "FAIL: " << message << '\n'; ++failures; }
}
}

int main() {
    using namespace auto_animation::anatomy;
    const std::string path = "auto_animation_marker_profile_test.autoanim";

    MarkerSet original;
    original.profile = "WingedHumanoid";
    original.add_or_replace({MarkerType::Pelvis, "Pelvis", {0, 1, 0}, {0, 1, 0}, MarkerSpace::Object, -1, 1, true});
    original.add_or_replace({MarkerType::WingTipLeft, "WingTip_L", {-2, 3, 0}, {1, 0, 0}, MarkerSpace::Surface, -1, 0.8f, false});

    std::string error;
    expect(save_marker_profile(original, path, error), "profile saves");
    expect(error.empty(), "save error empty");

    MarkerSet loaded;
    expect(load_marker_profile(path, loaded, error), "profile loads");
    expect(loaded.profile == original.profile, "profile name persists");
    expect(loaded.markers.size() == original.markers.size(), "marker count persists");
    expect(loaded.find(MarkerType::WingTipLeft, "WingTip_L") != nullptr, "marker identity persists");
    expect(loaded.find(MarkerType::WingTipLeft, "WingTip_L")->position.x == -2.0f, "marker position persists");

    std::remove(path.c_str());

    MarkerSet invalid;
    invalid.markers.push_back({MarkerType::Head, "", {0,0,0}, {0,1,0}, MarkerSpace::Object, -1, 2.0f, false});
    expect(!save_marker_profile(invalid, path, error), "invalid profile is rejected before save");
    std::remove(path.c_str());

    MarkerSet missing;
    expect(!load_marker_profile("does-not-exist.autoanim", missing, error), "missing profile fails cleanly");
    return failures;
}
