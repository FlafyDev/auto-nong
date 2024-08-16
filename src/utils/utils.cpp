#include "utils.hpp"

#include "../includes/geode.hpp"
#include <iomanip>
#include <iostream>
#include <matjson.hpp>
#include <regex>
#include <sstream>
#include <string>

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

// "dQw4w9WgXcQ"
// "https://youtu.be/dQw4w9WgXcQ?si=KpUDKu1_37VmDUVK"
// "https://www.youtube.com/watch?v=dQw4w9WgXcQ"
// "https://youtu.be/dQw4w9WgXcQ?si=t63BpVooXd1p-IEp"
// "https://youtu.be/dQw4w9WgXcQ"
// "https://www.youtube.com/watch?v=dQw4w9WgXcQ&list=RDGMEMQ1dJ7wXfLlqCjwV0xfSNbA&start_radio=1&rv=dQw4w9WgXcQ"
// "https://youtu.be/dQw4w9WgXcQ?si=ghJwHs21HYnOHdUY"
// "https://www.youtube.com/watch?v=dQw4w9WgXcQ&list=PLw9f36HuOEuC1wPukfmrvC8baB3hB0EbR"
std::optional<std::string> youtubeLinkToID(const std::string &link) {
  if (link.size() == 11) {
    return link;
  }

  std::regex youtube_regex("(?:youtube\\.com\\/.*[?&]v=|youtu\\.be\\/)([a-zA-Z0-9_-]{11})");
  std::smatch match;

  if (std::regex_search(link, match, youtube_regex) && match.size() > 1) {
    return match.str(1);
  }

  return std::nullopt;
}

std::string replaceAll(std::string str, const std::string &from, const std::string &to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
  }
  return str;
}
