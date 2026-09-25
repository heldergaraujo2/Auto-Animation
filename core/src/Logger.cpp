#include "auto_animation/Logger.hpp"
#include <iostream>
namespace auto_animation {
void log(LogLevel level, std::string_view message) {
    const char* prefix = "[INFO]";
    switch (level) {
        case LogLevel::Info: prefix = "[INFO]"; break;
        case LogLevel::Warning: prefix = "[WARN]"; break;
        case LogLevel::Error: prefix = "[ERROR]"; break;
    }
    std::cout << prefix << ' ' << message << '\n';
}
}
