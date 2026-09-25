#include "auto_animation/importer/AssimpImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <functional>
#include <limits>
#include <unordered_map>

namespace auto_animation::importer {

namespace {
std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::array<float, 16> to_matrix(const aiMatrix4x4& m) {
    return {m.a1,m.a2,m.a3,m.a4,m.b1,m.b2,m.b3,m.b4,m.c1,m.c2,m.c3,m.c4,m.d1,m.d2,m.d3,m.d4};
}

std::array<float, 4> color(const aiColor4D& c) {
    return {c.r,c.g,c.b,c.a};
}

void append_node_bones(
    const aiNode* node,
    std::int32_t parent,
    const std::unordered_map<std::string, std::int32_t>& lookup,
    std::vector<Bone>& bones) {
    auto it = lookup.find(node->mName.C_Str());
    const std::int32_t current = it == lookup.end() ? parent : it->second;
    if (it != lookup.end()) {
        bones[current].parent_index = parent;
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        append_node_bones(node->mChildren[i], current, lookup, bones);
    }
}

} // namespace

bool AssimpImporter::supports_extension(std::string_view extension) const noexcept {
    std::string ext(extension);
    ext = lower(ext);
    return ext == ".fbx" || ext == ".gltf" || ext == ".glb" || ext == ".obj" ||
           ext == ".smd" || ext == ".dae" || ext == ".3ds" || ext == ".stl" ||
           ext == ".ply";
}

ImportResult AssimpImporter::import_file(
    const std::filesystem::path& path,
    const ImportOptions& options) const {
    if (!std::filesystem::exists(path)) {
        return {{}, "File does not exist: " + path.string()};
    }

    Assimp::Importer importer;
    unsigned int flags = aiProcess_JoinIdenticalVertices;
    if (options.triangulate) flags |= aiProcess_Triangulate;
    if (options.generate_normals) flags |= aiProcess_GenSmoothNormals;
    if (options.flip_uvs) flags |= aiProcess_FlipUVs;

    const aiScene* scene = importer.ReadFile(path.string(), flags);
    if (scene == nullptr || scene->mRootNode == nullptr) {
        return {{}, std::string("Assimp import failed: ") + importer.GetErrorString()};
    }

    Asset asset;
    asset.source_path = path.string();
    asset.source_format = lower(path.extension().string());

    asset.materials.reserve(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        Material material;
        aiString name;
        scene->mMaterials[i]->Get(AI_MATKEY_NAME, name);
        material.name = name.C_Str();
        aiColor4D diffuse(1,1,1,1);
        if (scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS) {
            material.base_color = color(diffuse);
        }
        aiString texture;
        if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texture) == AI_SUCCESS) {
            material.diffuse_texture = texture.C_Str();
        }
        if (scene->mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &texture) == AI_SUCCESS) {
            material.normal_texture = texture.C_Str();
        }
        asset.materials.push_back(std::move(material));
    }

    std::unordered_map<std::string, std::int32_t> bone_lookup;
    if (options.load_skeleton) {
        Skeleton skeleton;
        skeleton.name = path.stem().string();
        for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
            const aiMesh* mesh = scene->mMeshes[m];
            for (unsigned int b = 0; b < mesh->mNumBones; ++b) {
                const aiBone* bone = mesh->mBones[b];
                const std::string name = bone->mName.C_Str();
                if (bone_lookup.contains(name)) continue;
                Bone out;
                out.name = name;
                out.bind_transform = to_matrix(bone->mOffsetMatrix);
                bone_lookup.emplace(name, static_cast<std::int32_t>(skeleton.bones.size()));
                skeleton.bones.push_back(out);
            }
        }
        if (!skeleton.bones.empty()) {
            append_node_bones(scene->mRootNode, -1, bone_lookup, skeleton.bones);
            asset.skeletons.push_back(std::move(skeleton));
        }
    }

    asset.meshes.reserve(scene->mNumMeshes);
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh* mesh = scene->mMeshes[m];
        Mesh out;
        out.name = mesh->mName.C_Str();
        out.material_index = mesh->mMaterialIndex;

        out.vertices.resize(mesh->mNumVertices);
        for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
            auto& dst = out.vertices[v];
            dst.position = {mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z};
            if (mesh->HasNormals()) {
                dst.normal = {mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z};
            }
            if (mesh->HasTextureCoords(0)) {
                dst.uv = {mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y};
            }
        }

        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            const aiFace& face = mesh->mFaces[f];
            out.indices.insert(out.indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
        }

        if (options.load_skeleton && !bone_lookup.empty()) {
            std::vector<unsigned int> counts(out.vertices.size(), 0);
            for (unsigned int b = 0; b < mesh->mNumBones; ++b) {
                const aiBone* bone = mesh->mBones[b];
                const auto index_it = bone_lookup.find(bone->mName.C_Str());
                if (index_it == bone_lookup.end()) continue;
                const auto bone_index = static_cast<std::uint32_t>(index_it->second);
                for (unsigned int w = 0; w < bone->mNumWeights; ++w) {
                    const auto vertex = bone->mWeights[w].mVertexId;
                    if (vertex >= out.vertices.size()) continue;
                    const auto slot = counts[vertex]++;
                    if (slot < 4) {
                        out.vertices[vertex].bone_indices[slot] = bone_index;
                        out.vertices[vertex].bone_weights[slot] = bone->mWeights[w].mWeight;
                    }
                }
            }
        }

        asset.meshes.push_back(std::move(out));
    }

    if (options.load_animations) {
        asset.animations.reserve(scene->mNumAnimations);
        for (unsigned int a = 0; a < scene->mNumAnimations; ++a) {
            const aiAnimation* animation = scene->mAnimations[a];
            AnimationClip clip;
            clip.name = animation->mName.C_Str();
            clip.duration_seconds = animation->mTicksPerSecond > 0.0
                ? animation->mDuration / animation->mTicksPerSecond
                : animation->mDuration;
            clip.ticks_per_second = animation->mTicksPerSecond;

            for (unsigned int c = 0; c < animation->mNumChannels; ++c) {
                const aiNodeAnim* channel = animation->mChannels[c];
                AnimationTrack track;
                track.bone_name = channel->mNodeName.C_Str();
                const unsigned int count = std::max(
                    channel->mNumPositionKeys,
                    std::max(channel->mNumRotationKeys, channel->mNumScalingKeys));
                track.keyframes.reserve(count);
                for (unsigned int k = 0; k < count; ++k) {
                    Keyframe key;
                    if (k < channel->mNumPositionKeys) {
                        const auto& p = channel->mPositionKeys[k];
                        key.time_seconds = animation->mTicksPerSecond > 0.0 ? p.mTime / animation->mTicksPerSecond : p.mTime;
                        key.translation = {p.mValue.x,p.mValue.y,p.mValue.z};
                    }
                    if (k < channel->mNumRotationKeys) {
                        const auto& r = channel->mRotationKeys[k];
                        key.time_seconds = animation->mTicksPerSecond > 0.0 ? r.mTime / animation->mTicksPerSecond : r.mTime;
                        key.rotation = {r.mValue.x,r.mValue.y,r.mValue.z,r.mValue.w};
                    }
                    if (k < channel->mNumScalingKeys) {
                        const auto& s = channel->mScalingKeys[k];
                        key.time_seconds = animation->mTicksPerSecond > 0.0 ? s.mTime / animation->mTicksPerSecond : s.mTime;
                        key.scale = {s.mValue.x,s.mValue.y,s.mValue.z};
                    }
                    track.keyframes.push_back(key);
                }
                clip.tracks.push_back(std::move(track));
            }
            asset.animations.push_back(std::move(clip));
        }
    }

    return {std::move(asset), {}};
}

} // namespace auto_animation::importer
