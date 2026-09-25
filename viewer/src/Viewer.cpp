#include "auto_animation/viewer/Viewer.hpp"
#include "auto_animation/Logger.hpp"

#include <SDL.h>
#include <SDL_opengl.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <utility>

namespace auto_animation::viewer {

namespace {
constexpr float kPi = 3.14159265358979323846f;

void set_perspective(float fov_degrees, float aspect, float near_plane, float far_plane) {
    const float radians = fov_degrees * kPi / 180.0f;
    const float top = std::tan(radians * 0.5f) * near_plane;
    const float right = top * aspect;
    glFrustum(-right, right, -top, top, near_plane, far_plane);
}

void draw_cube() {
    static constexpr float v[8][3] = {
        {-0.5f,-0.5f,-0.5f},{ 0.5f,-0.5f,-0.5f},{ 0.5f, 0.5f,-0.5f},{-0.5f, 0.5f,-0.5f},
        {-0.5f,-0.5f, 0.5f},{ 0.5f,-0.5f, 0.5f},{ 0.5f, 0.5f, 0.5f},{-0.5f, 0.5f, 0.5f}
    };
    static constexpr unsigned int f[6][4] = {
        {0,1,2,3},{4,7,6,5},{0,4,5,1},{3,2,6,7},{1,5,6,2},{0,3,7,4}
    };
    glBegin(GL_QUADS);
    for (const auto& face : f) {
        for (unsigned int index : face) {
            glVertex3fv(v[index]);
        }
    }
    glEnd();
}

void draw_grid(float extent, float step) {
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    for (float x = -extent; x <= extent; x += step) {
        glVertex3f(x, 0.0f, -extent);
        glVertex3f(x, 0.0f, extent);
    }
    for (float z = -extent; z <= extent; z += step) {
        glVertex3f(-extent, 0.0f, z);
        glVertex3f(extent, 0.0f, z);
    }
    glEnd();
}

void draw_axes(float length) {
    glBegin(GL_LINES);
    glColor3f(0.85f, 0.15f, 0.15f);
    glVertex3f(0.0f, 0.002f, 0.0f);
    glVertex3f(length, 0.002f, 0.0f);
    glColor3f(0.15f, 0.85f, 0.20f);
    glVertex3f(0.0f, 0.002f, 0.0f);
    glVertex3f(0.0f, length, 0.0f);
    glColor3f(0.20f, 0.45f, 0.95f);
    glVertex3f(0.0f, 0.002f, 0.0f);
    glVertex3f(0.0f, 0.002f, length);
    glEnd();
}

void draw_demo_asset(double time_seconds) {
    const float bob = static_cast<float>(std::sin(time_seconds * 2.0) * 0.12);
    const float wing = static_cast<float>(std::sin(time_seconds * 5.0) * 18.0);

    glPushMatrix();
    glTranslatef(0.0f, 2.4f + bob, 0.0f);

    glPushMatrix();
    glScalef(0.65f, 1.15f, 0.38f);
    draw_cube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 1.35f, 0.0f);
    glScalef(0.48f, 0.48f, 0.48f);
    draw_cube();
    glPopMatrix();

    for (int side : {-1, 1}) {
        glPushMatrix();
        glTranslatef(static_cast<float>(side) * 0.72f, 0.45f, 0.0f);
        glRotatef(static_cast<float>(side) * wing, 0.0f, 0.0f, 1.0f);
        glTranslatef(static_cast<float>(side) * 0.9f, 0.0f, 0.0f);
        glScalef(1.8f, 0.08f, 0.45f);
        draw_cube();
        glPopMatrix();
    }

    for (int side : {-1, 1}) {
        glPushMatrix();
        glTranslatef(static_cast<float>(side) * 0.30f, -1.25f, 0.0f);
        glScalef(0.22f, 0.9f, 0.22f);
        draw_cube();
        glPopMatrix();
    }
    glPopMatrix();
}

} // namespace

struct Viewer::Impl {
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
    bool running = false;
    bool wireframe = false;
    float yaw = 35.0f;
    float pitch = 18.0f;
    float distance = 8.0f;
    bool dragging = false;
    int last_mouse_x = 0;
    int last_mouse_y = 0;
    bool timeline_playing = true;
    double timeline_seconds = 0.0;
};

Viewer::Viewer(ViewerConfig config)
    : impl_(new Impl{}), config_(std::move(config)) {}

Viewer::~Viewer() {
    if (impl_ != nullptr) {
        if (impl_->context != nullptr) {
            SDL_GL_DeleteContext(impl_->context);
        }
        if (impl_->window != nullptr) {
            SDL_DestroyWindow(impl_->window);
        }
        SDL_Quit();
        delete impl_;
    }
}

bool Viewer::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        log(LogLevel::Error, SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    impl_->window = SDL_CreateWindow(
        config_.title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config_.width,
        config_.height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (impl_->window == nullptr) {
        log(LogLevel::Error, SDL_GetError());
        SDL_Quit();
        return false;
    }

    impl_->context = SDL_GL_CreateContext(impl_->window);
    if (impl_->context == nullptr) {
        log(LogLevel::Error, SDL_GetError());
        SDL_DestroyWindow(impl_->window);
        impl_->window = nullptr;
        SDL_Quit();
        return false;
    }

    SDL_GL_SetSwapInterval(config_.vsync ? 1 : 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.035f, 0.045f, 0.060f, 1.0f);

    log(LogLevel::Info, "3D viewer initialized.");
    return true;
}

bool Viewer::initialized() const noexcept {
    return impl_ != nullptr && impl_->window != nullptr && impl_->context != nullptr;
}

void Viewer::request_close() {
    if (impl_ != nullptr) {
        impl_->running = false;
    }
}

const ViewerStats& Viewer::stats() const noexcept {
    return stats_;
}

void Viewer::run() {
    if (!initialized()) {
        return;
    }

    impl_->running = true;
    auto previous = std::chrono::steady_clock::now();

    while (impl_->running) {
        const auto now = std::chrono::steady_clock::now();
        stats_.delta_seconds = std::chrono::duration<double>(now - previous).count();
        previous = now;
        stats_.delta_seconds = std::clamp(stats_.delta_seconds, 0.0, 0.25);
        ++stats_.frames;
        stats_.fps = stats_.delta_seconds > 0.0 ? 1.0 / stats_.delta_seconds : 0.0;

        SDL_Event event{};
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                impl_->running = false;
            } else if (event.type == SDL_WINDOWEVENT &&
                       event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                config_.width = std::max(1, event.window.data1);
                config_.height = std::max(1, event.window.data2);
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    impl_->running = false;
                } else if (event.key.keysym.sym == SDLK_SPACE) {
                    impl_->timeline_playing = !impl_->timeline_playing;
                } else if (event.key.keysym.sym == SDLK_w) {
                    impl_->wireframe = !impl_->wireframe;
                } else if (event.key.keysym.sym == SDLK_LEFT) {
                    impl_->timeline_seconds = std::max(0.0, impl_->timeline_seconds - 1.0 / 30.0);
                } else if (event.key.keysym.sym == SDLK_RIGHT) {
                    impl_->timeline_seconds += 1.0 / 30.0;
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN &&
                       event.button.button == SDL_BUTTON_LEFT) {
                impl_->dragging = true;
                impl_->last_mouse_x = event.button.x;
                impl_->last_mouse_y = event.button.y;
            } else if (event.type == SDL_MOUSEBUTTONUP &&
                       event.button.button == SDL_BUTTON_LEFT) {
                impl_->dragging = false;
            } else if (event.type == SDL_MOUSEMOTION && impl_->dragging) {
                const int dx = event.motion.x - impl_->last_mouse_x;
                const int dy = event.motion.y - impl_->last_mouse_y;
                impl_->yaw += static_cast<float>(dx) * 0.35f;
                impl_->pitch = std::clamp(impl_->pitch - static_cast<float>(dy) * 0.25f, -80.0f, 80.0f);
                impl_->last_mouse_x = event.motion.x;
                impl_->last_mouse_y = event.motion.y;
            } else if (event.type == SDL_MOUSEWHEEL) {
                impl_->distance = std::clamp(
                    impl_->distance - static_cast<float>(event.wheel.y) * 0.6f,
                    2.0f,
                    30.0f);
            }
        }

        if (impl_->timeline_playing) {
            impl_->timeline_seconds += stats_.delta_seconds;
            if (impl_->timeline_seconds >= 4.0) {
                impl_->timeline_seconds = std::fmod(impl_->timeline_seconds, 4.0);
            }
        }

        glViewport(0, 0, config_.width, config_.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        set_perspective(55.0f,
                        static_cast<float>(config_.width) / static_cast<float>(std::max(1, config_.height)),
                        0.1f,
                        100.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, -1.5f, -impl_->distance);
        glRotatef(impl_->pitch, 1.0f, 0.0f, 0.0f);
        glRotatef(impl_->yaw, 0.0f, 1.0f, 0.0f);

        glPolygonMode(GL_FRONT_AND_BACK, impl_->wireframe ? GL_LINE : GL_FILL);

        glColor3f(0.17f, 0.20f, 0.24f);
        draw_grid(10.0f, 1.0f);
        draw_axes(2.0f);

        glColor3f(0.72f, 0.78f, 0.86f);
        draw_demo_asset(impl_->timeline_seconds);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        SDL_GL_SwapWindow(impl_->window);

        if (stats_.frames % 30 == 0) {
            char title[256];
            std::snprintf(title, sizeof(title),
                          "%s | %.1f FPS | %s | Space: Play/Pause | W: Wireframe",
                          config_.title.c_str(), stats_.fps,
                          impl_->wireframe ? "Wireframe" : "Solid");
            SDL_SetWindowTitle(impl_->window, title);
        }
    }
}

} // namespace auto_animation::viewer
