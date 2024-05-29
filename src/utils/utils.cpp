#include "utils.hpp"
#include "../includes/geode.hpp"
#include <sstream>

Vec<String> splitString(const String &str, char delimiter) {
  Vec<String> tokens;
  std::stringstream ss(str);
  String token;
  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

String urlToFilename(const String &url) {
  String filename;
  filename.reserve(url.size()); // reserve memory for efficiency

  // Define a whitelist of allowed characters
  const String whitelist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.";

  // Convert the URL to the filename, keeping only characters in the whitelist
  for (char ch : url) {
    if (whitelist.find(ch) != String::npos) {
      filename.push_back(ch);
    } else {
      // Replace invalid characters with underscores
      filename.push_back('_');
    }
  }

  return filename;
}

String getSubstringAfterSlash(const String &input) {
  size_t pos = input.find_last_of('/');
  if (pos != String::npos && pos != input.length() - 1) {
    return input.substr(pos + 1);
  } else {
    return input;
  }
}

String decompressGz(Vec<uint8_t> &compressedData) {
  uint8_t *inflatedData;
  ZipUtils::ccInflateMemory(compressedData.data(), compressedData.size(), &inflatedData);
  String inflatedString(reinterpret_cast<char *>(inflatedData));
  free(inflatedData);
  return inflatedString;
}
