#include "auto_animation/Logger.hpp"
#include "auto_animation/Version.hpp"
#include <iostream>
int main() {
    auto_animation::log(auto_animation::LogLevel::Info, "Foundation initialized successfully.");
    std::cout << auto_animation::project_name() << " v" << auto_animation::project_version() << '\n';
    std::cout << "Next milestone: 3D viewer foundation." << '\n';
    return 0;
}
