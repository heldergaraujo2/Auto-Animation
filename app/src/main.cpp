#include "auto_animation/Logger.hpp"
#include "auto_animation/Version.hpp"
#include "auto_animation/viewer/Viewer.hpp"

#include <iostream>

int main() {
    using namespace auto_animation;

    log(LogLevel::Info, "Starting Auto-Animation.");
    std::cout << project_name() << " v" << project_version() << '\n';

    viewer::Viewer viewer;
    if (!viewer.initialize()) {
        log(LogLevel::Error, "Viewer initialization failed.");
        return 1;
    }

    log(LogLevel::Info, "Viewer controls: drag=orbit, wheel=zoom, Space=play/pause, W=wireframe, Esc=exit.");
    viewer.run();

    log(LogLevel::Info, "Viewer closed cleanly.");
    return 0;
}
