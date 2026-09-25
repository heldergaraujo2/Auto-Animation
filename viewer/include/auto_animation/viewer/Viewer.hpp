#pragma once

#include <cstdint>

namespace auto_animation::rigging { struct Skeleton; }
#include <string>

namespace auto_animation::viewer {

struct ViewerConfig {
    int width = 1280;
    int height = 720;
    std::string title = "Auto-Animation — 3D Viewer";
    bool vsync = true;
};

struct ViewerStats {
    double delta_seconds = 0.0;
    double fps = 0.0;
    std::uint64_t frames = 0;
};

class Viewer {
public:
    explicit Viewer(ViewerConfig config = {});
    ~Viewer();

    Viewer(const Viewer&) = delete;
    Viewer& operator=(const Viewer&) = delete;

    bool initialize();
    void run();
    void run_for_frames(std::uint64_t frame_count);
    void request_close();
    void set_skeleton(const rigging::Skeleton* skeleton);

    [[nodiscard]] bool initialized() const noexcept;
    [[nodiscard]] const ViewerStats& stats() const noexcept;

private:
    struct Impl;
    Impl* impl_;
    ViewerConfig config_;
    ViewerStats stats_;
};

} // namespace auto_animation::viewer
