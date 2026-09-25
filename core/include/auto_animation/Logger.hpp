#pragma once
#include <string_view>
namespace auto_animation {
enum class LogLevel { Info, Warning, Error };
void log(LogLevel level, std::string_view message);
}
