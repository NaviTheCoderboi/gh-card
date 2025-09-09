#pragma once
#include "color.hpp"

#include <nlohmann/json.hpp>
#include <regex>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace Utils {
std::size_t getTerminalWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return 0;
    }
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        return 0;
    }
    return w.ws_col;
#endif
}

size_t visibleLength(const std::string& str) {
    static const std::regex ansi_escape(R"(\x1B\[[0-9;]*[A-Za-z])");
    std::string clean_str = std::regex_replace(str, ansi_escape, "");
    return clean_str.length();
}

std::string truncateStr(const std::string& input, size_t width) {
    const std::string ellipsis = "...";
    if (width == 0)
        return "";
    if (width < ellipsis.size()) {
        return ellipsis.substr(0, width);
    }
    if (input.size() <= width) {
        return input;
    }

    return input.substr(0, width - ellipsis.size()) + ellipsis;
}

std::string repeat(const std::string& input, std::size_t num) {
    std::string ret;
    ret.reserve(input.size() * num);
    while (num--)
        ret += input;
    return ret;
}

template <typename T>
std::string formatField(std::string& label, const nlohmann::json& value, std::string& defaultMsg,
                        int maxWidth = -1) {
    std::string result = std::format("{}{}{}: ", Color::cyan, label, Color::reset);

    if (value.is_null()) {
        result += std::format("{}[{}]{}", Color::red, defaultMsg, Color::reset);
    } else {
        if constexpr (std::is_same_v<T, std::string>) {
            std::string val = value.get<std::string>();
            if (maxWidth > 0 && val.length() > static_cast<size_t>(maxWidth)) {
                result += val.substr(0, maxWidth - 3) + "...";
            } else {
                result += val;
            }
        } else {
            result += std::to_string(value.get<T>());
        }
    }

    return result;
}

std::string formatUrl(std::string_view url) {
    return std::format("{}{}{}", Color::blue, url, Color::reset);
}
} // namespace Utils