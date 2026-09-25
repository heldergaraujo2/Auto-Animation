cmake_minimum_required(VERSION 3.20)
project(AutoAnimation VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(AUTO_ANIMATION_BUILD_TESTS "Build automated tests" ON)
option(AUTO_ANIMATION_BUILD_VIEWER "Build the 3D viewer application" ON)
option(AUTO_ANIMATION_FETCH_SDL2 "Fetch SDL2 when a system package is unavailable" ON)

add_library(auto_animation_core
    core/src/Version.cpp
    core/src/Logger.cpp
)
target_include_directories(auto_animation_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/core/include)
target_compile_features(auto_animation_core PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(auto_animation_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(auto_animation_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(AUTO_ANIMATION_BUILD_VIEWER)
    find_package(OpenGL REQUIRED)
    find_package(SDL2 CONFIG QUIET)

    if(NOT TARGET SDL2::SDL2 AND AUTO_ANIMATION_FETCH_SDL2)
        include(FetchContent)
        FetchContent_Declare(
            SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-2.32.8
            GIT_SHALLOW TRUE
        )
        set(SDL_TESTS OFF CACHE BOOL "" FORCE)
        set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(SDL2)
    endif()

    if(NOT TARGET SDL2::SDL2)
        message(FATAL_ERROR "SDL2 is required for AUTO_ANIMATION_BUILD_VIEWER=ON. Install SDL2 or configure with AUTO_ANIMATION_BUILD_VIEWER=OFF.")
    endif()

    add_library(auto_animation_viewer
        viewer/src/Viewer.cpp
    )
    target_include_directories(auto_animation_viewer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/viewer/include)
    target_link_libraries(auto_animation_viewer PUBLIC auto_animation_core SDL2::SDL2 OpenGL::GL)
    target_compile_features(auto_animation_viewer PUBLIC cxx_std_20)

    if(MSVC)
        target_compile_options(auto_animation_viewer PRIVATE /W4 /permissive-)
    else()
        target_compile_options(auto_animation_viewer PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_executable(auto-animation app/src/main.cpp)
    target_link_libraries(auto-animation PRIVATE auto_animation_viewer)
else()
    add_executable(auto-animation app/src/main.cpp)
    target_link_libraries(auto-animation PRIVATE auto_animation_core)
endif()

if(AUTO_ANIMATION_BUILD_TESTS)
    enable_testing()
    add_executable(auto-animation-tests tests/unit/test_foundation.cpp)
    target_link_libraries(auto-animation-tests PRIVATE auto_animation_core)
    add_test(NAME foundation COMMAND auto-animation-tests)

    if(AUTO_ANIMATION_BUILD_VIEWER)
        add_executable(auto-animation-viewer-tests tests/unit/test_viewer.cpp)
        target_link_libraries(auto-animation-viewer-tests PRIVATE auto_animation_viewer)
        add_test(NAME viewer COMMAND auto-animation-viewer-tests)
    endif()
endif()
