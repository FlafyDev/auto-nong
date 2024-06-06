#pragma once

#include <matjson.hpp>
#include <string>
#include <vector>

template <> struct matjson::Serialize<std::vector<int>> {
  static bool is_json(matjson::Value const &value) { return value.is_array(); }

  static std::vector<int> from_json(matjson::Value const &value) {
    std::vector<int> vec;
    if (!is_json(value)) {
      return vec;
    }
    auto array = value.as_array();
    for (auto const &elem : array) {
      vec.push_back(elem.as_int());
    }
    return vec;
  }

  static matjson::Value to_json(std::vector<int> const &vec) {
    auto array = matjson::Array();
    for (auto const &elem : vec) {
      array.push_back(matjson::Value(elem));
    }
    return array;
  }
};

template <> struct matjson::Serialize<std::vector<std::string>> {
  static bool is_json(matjson::Value const &value) { return value.is_array(); }

  static std::vector<std::string> from_json(matjson::Value const &value) {
    std::vector<std::string> vec;
    if (!is_json(value)) {
      return vec;
    }
    auto array = value.as_array();
    for (auto const &elem : array) {
      vec.push_back(elem.as_string());
    }
    return vec;
  }

  static matjson::Value to_json(std::vector<std::string> const &vec) {
    auto array = matjson::Array();
    for (auto const &elem : vec) {
      array.push_back(matjson::Value(elem));
    }
    return array;
  }
};
