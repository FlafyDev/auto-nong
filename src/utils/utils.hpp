#pragma once

#include <string>
#include <vector>

std::vector<std::string> splitString(const std::string &str, char delimiter);

std::string urlToFilename(const std::string &url);

std::string getSubstringAfterSlash(const std::string &input);

std::string decompressGz(std::vector<uint8_t> &compressedData, bool isJson = false);

template<typename T, typename... Rest>
std::string hashValues(const T& value, const Rest&... rest) {
    std::hash<T> hasher;
    std::size_t combined_hash = hasher(value);
    (void)std::initializer_list<std::size_t>{
        ((combined_hash ^= std::hash<Rest>()(rest)),
        (combined_hash ^= static_cast<std::size_t>(0x9e3779b9)),
        (combined_hash ^= (combined_hash << 6)),
        (combined_hash ^= (combined_hash >> 2)),
        static_cast<std::size_t>(0))...
    };
    
    std::ostringstream oss;
    oss << std::hex << std::setw(sizeof(std::size_t) * 2) << std::setfill('0') << combined_hash;
    return oss.str();
}
