#include "auto_animation/Version.hpp"
#include <iostream>
#include <string_view>
namespace {
int failures = 0;
void expect(bool condition, std::string_view message) {
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; ++failures; }
}
}
int main() {
    expect(auto_animation::project_name() == "Auto-Animation", "project name must be Auto-Animation");
    expect(auto_animation::project_version() == "0.1.0", "foundation version must be 0.1.0");
    return failures;
}
