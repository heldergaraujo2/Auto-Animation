#pragma once

#include <array>
#include "auto_animation/animation/Animation.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace auto_animation::importer {

struct Vec2 { float x = 0.0f; float y = 0.0f; };
struct Vec3 { float x = 0.0f; float y = 0.0f; float z = 0.0f; };

struct Vertex {
    Vec3 position{};
    Vec3 normal{};
    Vec2 uv{};
    std::array<std::uint32_t, 4> bone_indices{};
    std::array<float, 4> bone_weights{};
};

struct Material {
    std::string name;
    std::array<float, 4> base_color{1.0f, 1.0f, 1.0f, 1.0f};
    std::string diffuse_texture;
    std::string normal_texture;
};

struct Mesh {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
    std::uint32_t material_index = 0;
};

struct Bone {
    std::string name;
    std::int32_t parent_index = -1;
    std::array<float, 16> bind_transform{};
};

struct Skeleton {
    std::string name;
    std::vector<Bone> bones;
};

using AnimationClip = animation::AnimationClip;
using AnimationTrack = animation::AnimationTrack;

struct Asset {
    std::string source_path;
    std::string source_format;
    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    std::vector<Skeleton> skeletons;
    std::vector<AnimationClip> animations;

    [[nodiscard]] bool has_geometry() const noexcept { return !meshes.empty(); }
    [[nodiscard]] bool has_skeleton() const noexcept { return !skeletons.empty(); }
    [[nodiscard]] bool has_animation() const noexcept { return !animations.empty(); }
};

} // namespace auto_animation::importer
