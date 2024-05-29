#pragma once

#include "../includes/geode.hpp"
#include <string>
#include <vector>

Vec<std::string> splitString(str str, char delimiter);
std::string urlToFilename(str url);
std::string getSubstringAfterSlash(str input);
std::string decompressGz(Vec<uint8_t> &compressedData);
