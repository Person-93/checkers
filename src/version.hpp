#pragma once

#include <string_view>
#include <sstream>

struct version {
    static const unsigned long long major;
    static const unsigned long long minor;
    static const unsigned long long patch;
    static const std::string_view   tweak;

    static std::string_view shortVersion() {
        static std::string version = []() -> std::string {
            std::ostringstream ss;
            ss << major << '.' << minor << '.' << patch;
            return ss.str();
        }();
        return version;
    }

    static std::string_view longVersion() {
        static std::string version = []() -> std::string {
            std::ostringstream ss;
            ss << shortVersion() << tweak;
            return ss.str();
        }();
        return version;
    }
};
