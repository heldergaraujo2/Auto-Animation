#include "auto_animation/Logger.hpp"
#include "auto_animation/Version.hpp"
#include "auto_animation/viewer/Viewer.hpp"

#include <iostream>
#include <string_view>

int main(int argc, char** argv) {
    using namespace auto_animation;

    log(LogLevel::Info, "Starting Auto-Animation.");
    std::cout << project_name() << " v" << project_version() << '\n';

    viewer::Viewer viewer;
    if (!viewer.initialize()) {
        log(LogLevel::Error, "Viewer initialization failed.");
        return 1;
    }

    if (argc > 1 && std::string_view(argv[1]) == "--self-test") {
        viewer.run_for_frames(3);
        log(LogLevel::Info, "Viewer self-test completed.");
        return viewer.stats().frames == 3 ? 0 : 2;
    }

    log(LogLevel::Info, "Viewer controls: drag=orbit, wheel=zoom, Space=play/pause, W=wireframe, Esc=exit.");
    viewer.run();

    log(LogLevel::Info, "Viewer closed cleanly.");
    return 0;
}
