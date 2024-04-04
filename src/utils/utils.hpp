#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

std::vector<std::string> splitString(const std::string& str, char delimiter);

std::string urlToFilename(const std::string& url);

std::string getSubstringAfterSlash(const std::string& input);
