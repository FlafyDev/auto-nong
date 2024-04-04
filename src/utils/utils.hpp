#pragma once

#include <string>
#include <vector>

std::vector<std::string> splitString(const std::string &str, char delimiter);

std::string urlToFilename(const std::string &url);

std::string getSubstringAfterSlash(const std::string &input);

std::string decompressGz(std::vector<uint8_t> &compressedData);
