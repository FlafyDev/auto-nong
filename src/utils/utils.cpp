#include "utils.hpp"

#include "../includes/geode.hpp"
#include <matjson.hpp>
#include <sstream>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

std::vector<std::string> splitString(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

std::string urlToFilename(const std::string &url) {
  std::string filename;
  filename.reserve(url.size()); // reserve memory for efficiency

  // Define a whitelist of allowed characters
  const std::string whitelist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.";

  // Convert the URL to the filename, keeping only characters in the whitelist
  for (char ch : url) {
    if (whitelist.find(ch) != std::string::npos) {
      filename.push_back(ch);
    } else {
      // Replace invalid characters with underscores
      filename.push_back('_');
    }
  }

  return filename;
}

std::string getSubstringAfterSlash(const std::string &input) {
  size_t pos = input.find_last_of('/');
  if (pos != std::string::npos && pos != input.length() - 1) {
    return input.substr(pos + 1);
  } else {
    return input;
  }
}

std::string decompressGz(std::vector<uint8_t> &compressedData, bool isJson) {
  uint8_t *inflatedData = nullptr;
  ZipUtils::ccInflateMemory(compressedData.data(), compressedData.size(), &inflatedData);

  std::string inflatedString;
  const int maxRetries = 100;
  int retries = 0;

  std::string jsonError;
  while (inflatedString.empty() && retries < maxRetries) {
    inflatedString = std::string(reinterpret_cast<char *>(inflatedData));
    if (!inflatedString.empty() &&
        (!isJson || matjson::parse(inflatedString, jsonError).has_value())) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    retries++;
  }

  free(inflatedData);
  return inflatedString;
}
