#pragma once
#include <cstring>
#include <curl/curl.h>
#include <functional>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using json = nlohmann::json;

template <typename BufferType>
static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    size_t totalSize = size * nmemb;
    BufferType* buffer = static_cast<BufferType*>(userdata);

    if constexpr (std::is_same_v<BufferType, std::string>) {
        buffer->append(ptr, totalSize);
    } else {
        buffer->insert(buffer->end(), ptr, ptr + totalSize);
    }

    return totalSize;
}

class Curl {
  private:
    CURL* curl{};

  public:
    Curl(bool init = true) {
        if (init) {
            this->init();
        }
    };

    ~Curl() {
        cleanup();
    };

    void init() {
        curl_global_init(CURL_GLOBAL_DEFAULT);

        CURL* _curl{curl_easy_init()};

        if (!_curl) {
            curl = nullptr;
            return;
        }

        curl = _curl;
    };

    void cleanup() {
        if (curl) {
            curl_easy_cleanup(curl);
            curl = nullptr;
        }

        curl_global_cleanup();
    };

    std::optional<json> parseJSON(const std::string& json) {
        std::optional<nlohmann::json> parsed{};

        try {
            parsed = json::parse(json);
        } catch (json::parse_error& _ex) {
            parsed = std::nullopt;
        }

        return parsed;
    };

    template <typename BufferType>
    bool fetch(const std::string& url, const std::function<void(CURL*)> modifier = nullptr,
               BufferType* buffer = nullptr) {
        if (!curl)
            init();

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (buffer) {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback<BufferType>);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
        }

        if (modifier)
            modifier(curl);

        CURLcode res = curl_easy_perform(curl);
        return (res == CURLE_OK);
    };
};