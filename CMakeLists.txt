cmake_minimum_required(VERSION 3.20)
project(AutoAnimation VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(AUTO_ANIMATION_BUILD_TESTS "Build automated tests" ON)
option(AUTO_ANIMATION_BUILD_VIEWER "Build the 3D viewer application" ON)
option(AUTO_ANIMATION_BUILD_IMPORTERS "Build asset importers" ON)
option(AUTO_ANIMATION_FETCH_SDL2 "Fetch SDL2 when a system package is unavailable" ON)
option(AUTO_ANIMATION_FETCH_ASSIMP "Fetch Assimp when a system package is unavailable" ON)

add_library(auto_animation_core
    core/src/Version.cpp
    core/src/Logger.cpp
)
add_library(auto_animation_animation
    animation/src/Animation.cpp
)
add_library(auto_animation_rigging
    rigging/src/Skeleton.cpp
)
target_include_directories(auto_animation_rigging PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/rigging/include
)
target_link_libraries(auto_animation_rigging PUBLIC auto_animation_animation)
target_compile_features(auto_animation_rigging PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(auto_animation_rigging PRIVATE /W4 /permissive-)
else()
    target_compile_options(auto_animation_rigging PRIVATE -Wall -Wextra -Wpedantic)
endif()
target_include_directories(auto_animation_animation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/animation/include
)
target_link_libraries(auto_animation_animation PUBLIC auto_animation_core)
target_compile_features(auto_animation_animation PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(auto_animation_animation PRIVATE /W4 /permissive-)
else()
    target_compile_options(auto_animation_animation PRIVATE -Wall -Wextra -Wpedantic)
endif()
target_include_directories(auto_animation_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/core/include)
target_compile_features(auto_animation_core PUBLIC cxx_std_20)
set_target_properties(auto_animation_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    target_compile_options(auto_animation_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(auto_animation_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(AUTO_ANIMATION_BUILD_IMPORTERS)
    if(AUTO_ANIMATION_FETCH_ASSIMP)
        include(FetchContent)
        set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
        set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
        set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
        set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
        FetchContent_Declare(
            assimp
            GIT_REPOSITORY https://github.com/assimp/assimp.git
            GIT_TAG v5.4.3
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(assimp)
    else()
        find_package(assimp CONFIG REQUIRED)
    endif()

    add_library(auto_animation_importer
        importer/core/src/FormatDetector.cpp
        importer/core/src/ImporterRegistry.cpp
        importer/core/src/DefaultRegistry.cpp
        importer/assimp/src/AssimpImporter.cpp
    )
    target_include_directories(auto_animation_importer PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/importer/core/include
        ${CMAKE_CURRENT_SOURCE_DIR}/importer/assimp/include
    )
    if(TARGET assimp::assimp)
        target_link_libraries(auto_animation_importer PUBLIC auto_animation_core auto_animation_animation auto_animation_rigging assimp::assimp)
    else()
        target_link_libraries(auto_animation_importer PUBLIC auto_animation_core auto_animation_animation auto_animation_rigging assimp)
    endif()
    target_compile_features(auto_animation_importer PUBLIC cxx_std_20)

    if(MSVC)
        target_compile_options(auto_animation_importer PRIVATE /W4 /permissive-)
    else()
        target_compile_options(auto_animation_importer PRIVATE -Wall -Wextra -Wpedantic)
    endif()
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
    target_link_libraries(auto_animation_viewer PUBLIC auto_animation_core auto_animation_rigging SDL2::SDL2 OpenGL::GL)
    target_compile_features(auto_animation_viewer PUBLIC cxx_std_20)

    if(MSVC)
        target_compile_options(auto_animation_viewer PRIVATE /W4 /permissive-)
    else()
        target_compile_options(auto_animation_viewer PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_executable(auto-animation app/src/main.cpp)
    target_link_libraries(auto-animation PRIVATE auto_animation_viewer)
    if(AUTO_ANIMATION_BUILD_IMPORTERS)
        target_link_libraries(auto-animation PRIVATE auto_animation_importer)
    endif()
else()
    add_executable(auto-animation app/src/main.cpp)
    target_link_libraries(auto-animation PRIVATE auto_animation_core)
endif()

if(AUTO_ANIMATION_BUILD_TESTS)
    enable_testing()
    add_executable(auto-animation-tests tests/unit/test_foundation.cpp)
    target_link_libraries(auto-animation-tests PRIVATE auto_animation_core)
    add_test(NAME foundation COMMAND auto-animation-tests)

    add_executable(auto-animation-animation-tests tests/unit/test_animation.cpp)
    target_link_libraries(auto-animation-animation-tests PRIVATE auto_animation_animation)
    add_test(NAME animation COMMAND auto-animation-animation-tests)

    add_executable(auto-animation-rigging-tests tests/unit/test_skeleton.cpp)
    target_link_libraries(auto-animation-rigging-tests PRIVATE auto_animation_rigging)
    add_test(NAME rigging COMMAND auto-animation-rigging-tests)

    if(AUTO_ANIMATION_BUILD_VIEWER)
        add_executable(auto-animation-viewer-tests tests/unit/test_viewer.cpp)
        target_link_libraries(auto-animation-viewer-tests PRIVATE auto_animation_viewer)
        add_test(NAME viewer COMMAND auto-animation-viewer-tests)
    endif()

    if(AUTO_ANIMATION_BUILD_IMPORTERS)
        add_executable(auto-animation-importer-tests tests/unit/test_importer.cpp)
        target_link_libraries(auto-animation-importer-tests PRIVATE auto_animation_importer)
        add_test(NAME importer COMMAND auto-animation-importer-tests ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures)
    endif()
endif()
