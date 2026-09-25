#include "auto_animation/viewer/Viewer.hpp"

#include <iostream>
#include <string_view>

namespace {
int failures = 0;

void expect(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}
}

int main() {
    auto_animation::viewer::Viewer viewer({
        640,
        480,
        "Auto-Animation Viewer Test",
        false
    });

    expect(!viewer.initialized(), "viewer must start uninitialized");
    expect(viewer.stats().frames == 0, "initial frame count must be zero");
    expect(viewer.stats().fps == 0.0, "initial FPS must be zero");
    viewer.request_close();
    return failures;
}
