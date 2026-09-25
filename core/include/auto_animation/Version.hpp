#pragma once
#include <string_view>
namespace auto_animation {
[[nodiscard]] constexpr std::string_view project_name() noexcept { return "Auto-Animation"; }
[[nodiscard]] constexpr std::string_view project_version() noexcept { return "0.1.0"; }
}
