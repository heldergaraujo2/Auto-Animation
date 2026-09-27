#include "auto_animation/Logger.hpp"
#include "auto_animation/Version.hpp"
#include "auto_animation/viewer/Viewer.hpp"
#include "auto_animation/anatomy/Markers.hpp"
#include "auto_animation/anatomy/MarkerProfileIO.hpp"

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

    anatomy::MarkerSet marker_set;
    if (argc > 1 && std::string_view(argv[1]) == "--marker-editor") {
        marker_set.profile = "Humanoid";
        marker_set.add_or_replace({anatomy::MarkerType::Root, "Root", {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, anatomy::MarkerSpace::Object, -1, 1.0f, true});
        marker_set.add_or_replace({anatomy::MarkerType::Pelvis, "Pelvis", {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, anatomy::MarkerSpace::Object, -1, 1.0f, true});
        marker_set.add_or_replace({anatomy::MarkerType::Chest, "Chest", {0.0f, 2.4f, 0.0f}, {0.0f, 1.0f, 0.0f}, anatomy::MarkerSpace::Object, -1, 1.0f, true});
        marker_set.add_or_replace({anatomy::MarkerType::Head, "Head", {0.0f, 3.8f, 0.0f}, {0.0f, 1.0f, 0.0f}, anatomy::MarkerSpace::Object, -1, 1.0f, true});
        marker_set.add_or_replace({anatomy::MarkerType::ShoulderLeft, "Shoulder_L", {-0.7f, 2.8f, 0.0f}, {0.0f, 1.0f, 0.0f}, anatomy::MarkerSpace::Object, -1, 1.0f, false});
        marker_set.add_or_replace({anatomy::MarkerType::ShoulderRight, "Shoulder_R", {0.7f, 2.8f, 0.0f}, {0.0f, 1.0f, 0.0f}, anatomy::MarkerSpace::Object, -1, 1.0f, false});
        viewer.set_editable_markers(&marker_set);
        log(LogLevel::Info, "Marker editor: F=edit mode, Shift+click=add, click-drag=move, Delete=remove, M=mirror, A=mirror all, [ / ]=change type.");
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
