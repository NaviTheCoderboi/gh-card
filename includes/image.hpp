#pragma once

#include "stb_image.h"
#include "stb_image_resize2.h"

#include <cstring>
#include <string>
#include <vector>

const char* asciiChars = "@%#*+=-:. ";
const int numChars = strlen(asciiChars);

class Image {
    std::vector<unsigned char>& buffer;

    void load() {
        img = stbi_load_from_memory(buffer.data(), buffer.size(), &width, &height, &channels, 3);
    };

  public:
    stbi_uc* img{};
    int height{};
    int width{};
    int channels{};

    Image(std::vector<unsigned char>& buff) : buffer{buff} {
        load();
    };

    ~Image() {
        if (img) {
            stbi_image_free(img);
        }
    };

    bool resize(int newWidth, int newHeight) {
        if (!img)
            return false;

        unsigned char* resizedImg{new unsigned char[newWidth * newHeight * channels]};

        auto result{stbir_resize_uint8_srgb(img, width, height, 0, resizedImg, newWidth, newHeight,
                                            0, static_cast<stbir_pixel_layout>(channels))};

        if (!result) {
            delete[] resizedImg;
            return false;
        }

        stbi_image_free(img);

        img = resizedImg;
        width = newWidth;
        height = newHeight;

        return true;
    };

    bool convertToGrayscale() {
        if (!img || channels < 3)
            return true;

        int imgSize{width * height};
        std::vector<unsigned char> grayImg(imgSize);

        for (int i{}; i < imgSize; ++i) {
            unsigned char r{img[i * channels + 0]};
            unsigned char g{img[i * channels + 1]};
            unsigned char b{img[i * channels + 2]};
            unsigned char gray{static_cast<unsigned char>(0.299f * r + 0.587f * g + 0.114f * b)};
            grayImg[i] = gray;
        }

        stbi_image_free(img);

        img = new unsigned char[imgSize];
        std::copy(grayImg.begin(), grayImg.end(), img);

        if (!img || grayImg.size() == 0)
            return false;

        channels = 1;

        return true;
    };

    std::vector<std::string> getASCIIImage() {
        std::vector<std::string> image{};

        for (int y{}; y < height; y++) {
            std::string line{};

            for (int x{}; x < width; ++x) {
                unsigned char pixel{img[y * width + x]};
                int charIndex{(pixel * (numChars - 1)) / 255};
                line.push_back(asciiChars[charIndex]);
            }

            image.push_back(line);
        }

        return image;
    };
};