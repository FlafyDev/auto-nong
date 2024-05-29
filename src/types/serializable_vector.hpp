#pragma once

#include "../includes/geode.hpp"
#include <matjson.hpp>
#include <vector>

template <> struct matjson::Serialize<Vec<int>> {
  static bool is_json(matjson::Value const &value) { return value.is_array(); }

  static Vec<int> from_json(matjson::Value const &value) {
    Vec<int> vec;
    auto array = value.as_array();
    for (auto const &elem : array) {
      vec.push_back(elem.as_int());
    }
    return vec;
  }

  static matjson::Value to_json(Vec<int> const &vec) {
    auto array = matjson::Array();
    for (auto const &elem : vec) {
      array.push_back(elem);
    }
    return array;
  }
};

template <> struct matjson::Serialize<Vec<String>> {
  static bool is_json(matjson::Value const &value) { return value.is_array(); }

  static Vec<String> from_json(matjson::Value const &value) {
    Vec<String> vec;
    auto array = value.as_array();
    for (auto const &elem : array) {
      vec.push_back(elem.as_string());
    }
    return vec;
  }

  static matjson::Value to_json(Vec<String> const &vec) {
    auto array = matjson::Array();
    for (auto const &elem : vec) {
      array.push_back(elem);
    }
    return array;
  }
};
