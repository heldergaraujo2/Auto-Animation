cmake_minimum_required(VERSION 3.20)
project(AutoAnimation VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(AUTO_ANIMATION_BUILD_TESTS "Build automated tests" ON)

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

add_executable(auto-animation app/src/main.cpp)
target_link_libraries(auto-animation PRIVATE auto_animation_core)

if(AUTO_ANIMATION_BUILD_TESTS)
    enable_testing()
    add_executable(auto-animation-tests tests/unit/test_foundation.cpp)
    target_link_libraries(auto-animation-tests PRIVATE auto_animation_core)
    add_test(NAME foundation COMMAND auto-animation-tests)
endif()
