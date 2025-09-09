#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "color.hpp"
#include "http.hpp"
#include "image.hpp"
#include "utils.hpp"

#include <curl/curl.h>
#include <format>
#include <iostream>
#include <string>
#include <vector>

namespace Constants {
constexpr size_t CARD_IMG_WIDTH = 50;
constexpr size_t CARD_IMG_HEIGHT = 20;
constexpr size_t CARD_PADDING = 1;
constexpr size_t CARD_CENTER_SPACING = 2;
} // namespace Constants

struct CardCalc {
    const std::size_t& fullWidth;
    const std::size_t& totalLines;
    const std::size_t& leftWidth;
    const std::size_t& rightWidth;
};

void printCard(const std::vector<std::string>& asciiImg,
               const std::vector<std::string>& userDetails, const CardCalc calc) {
    std::cout << Color::golden << "┌" << Utils::repeat("─", calc.fullWidth - 2) << "┐"
              << Color::reset << "\n";
    for (int i = 1; i <= Constants::CARD_PADDING; i++) {
        std::cout << Color::golden << "│" << Utils::repeat(" ", calc.fullWidth - 2) << "│"
                  << Color::reset << "\n";
    }

    for (size_t i = 0; i < calc.totalLines; i++) {
        std::string leftStr = (i < asciiImg.size()) ? asciiImg[i] : "";
        std::string rightStr =
            (i < userDetails.size()) ? Utils::truncateStr(userDetails[i], calc.rightWidth) : "";

        std::cout << std::format("{}{}{}", Color::golden, "│", Color::reset)
                  << Utils::repeat(" ", Constants::CARD_PADDING)
                  << (leftStr + Utils::repeat(" ", calc.leftWidth - leftStr.length()))
                  << Utils::repeat(" ", Constants::CARD_CENTER_SPACING)
                  << (rightStr +
                      Utils::repeat(" ", calc.rightWidth - Utils::visibleLength(rightStr)))
                  << Utils::repeat(" ", Constants::CARD_PADDING)
                  << std::format("{}{}{}", Color::golden, "│", Color::reset) << "\n";
    }

    for (int i = 1; i <= Constants::CARD_PADDING; i++) {
        std::cout << Color::golden << "│" << Utils::repeat(" ", calc.fullWidth - 2) << "│"
                  << Color::reset << "\n";
    }
    std::cout << Color::golden << "└" << Utils::repeat("─", calc.fullWidth - 2) << "┘"
              << Color::reset << "\n";
}

int main() {
    const std::size_t width{Utils::getTerminalWidth()};

    Curl curl{};

    std::string username{};
    std::cout << Color::green << "Enter the github username:" << Color::reset << "\n"
              << Color::magenta;
    std::cin >> username;
    std::cout << Color::reset << "\n";

    std::string userUrl{"https://api.github.com/users/" + username};
    std::string userBuffer{};

    auto resp = curl.fetch<std::string>(
        userUrl, [](CURL* curl) { curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0"); },
        &userBuffer);
    if (resp == false) {
        std::cout << Color::red << "User not found" << Color::reset << "\n";
        return 1;
    }

    auto _user = curl.parseJSON(userBuffer);
    if (!_user || (*_user)["status"] == "404") {
        std::cout << Color::red << "User not found" << Color::reset << "\n";
        return 1;
    }

    auto& user = *_user;

    std::vector<unsigned char> imgBuffer{};
    std::string imgUrl{user["avatar_url"]};
    auto response = curl.fetch<std::vector<unsigned char>>(imgUrl, nullptr, &imgBuffer);

    if (!response) {
        std::cout << Color::red << "User avatar not found" << Color::reset << "\n";
        return 1;
    }

    const std::vector<std::string> userDetails{
        std::format("{}ID{}: {}", Color::cyan, Color::reset, user["id"].get<unsigned int>()),
        std::format("{}Username{}: {}", Color::cyan, Color::reset,
                    user["name"].is_null()
                        ? std::format("{}[No username]{}", Color::red, Color::reset)
                        : user["name"].get<std::string>()),
        std::format("{}Login{}: {}", Color::cyan, Color::reset, user["login"].get<std::string>()),
        std::format("{}Email{}: {}", Color::cyan, Color::reset,
                    user["email"].is_null()
                        ? std::format("{}[Private email]{}", Color::red, Color::reset)
                        : user["email"].get<std::string>()),
        std::format("{}Company{}: {}", Color::cyan, Color::reset,
                    user["company"].is_null()
                        ? std::format("{}[No company]{}", Color::red, Color::reset)
                        : user["company"].get<std::string>()),
        std::format("{}Blog{}: {}", Color::cyan, Color::reset,
                    user["blog"].is_null()
                        ? std::format("{}[No blog site]{}", Color::red, Color::reset)
                        : std::format("{}{}{}", Color::blue, user["blog"].get<std::string>(),
                                      Color::reset)),
        std::format("{}Location{}: {}", Color::cyan, Color::reset,
                    user["location"].is_null()
                        ? std::format("{}[No location provided]{}", Color::red, Color::reset)
                        : user["location"].get<std::string>()),
        std::format("{}Bio{}: {}", Color::cyan, Color::reset,
                    user["bio"].is_null() ? std::format("{}[No bio]{}", Color::red, Color::reset)
                                          : user["bio"].get<std::string>()),
        std::format("{}Followers{}: {}", Color::cyan, Color::reset,
                    user["followers"].get<unsigned int>()),
        std::format("{}Following{}: {}", Color::cyan, Color::reset,
                    user["following"].get<unsigned int>()),
        std::format("{}Public repositories{}: {}", Color::cyan, Color::reset,
                    user["public_repos"].get<unsigned int>()),
        std::format("{}Public gists{}: {}", Color::cyan, Color::reset,
                    user["public_gists"].get<unsigned int>()),
        std::format(
            "{}Profile url{}: {}", Color::cyan, Color::reset,
            std::format("{}{}{}", Color::blue, user["html_url"].get<std::string>(), Color::reset)),
    };

    Image img{imgBuffer};
    img.convertToGrayscale();
    img.resize(50, 20);

    const auto asciiImg = img.getASCIIImage();

    const size_t totalLines = std::max(asciiImg.size(), userDetails.size());
    const std::size_t leftWidth = asciiImg[0].length();
    const std::size_t rightWidth = width - (2 * Constants::CARD_PADDING) -
                                   Constants::CARD_CENTER_SPACING - leftWidth -
                                   (2 * Constants::CARD_PADDING);

    const CardCalc calc{width, totalLines, leftWidth, rightWidth};

    printCard(asciiImg, userDetails, calc);

    imgBuffer.clear();
    userBuffer.clear();

    return 0;
}