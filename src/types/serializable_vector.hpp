#pragma once

#include <matjson.hpp>

#include <vector>

template<>
struct matjson::Serialize<std::vector<int>> {
    static bool is_json(matjson:: Value const& value) {
        return value.is_array();
    }

    static std::vector<int> from_json(matjson::Value const& value) {
        std::vector<int> vec;
        auto array = value.as_array();
        for (auto const& elem : array) {
            vec.push_back(elem.as_int());
        }
        return vec;
    }

    static matjson::Value to_json(std::vector<int> const& vec) {
        auto array = matjson::Array();
        for (auto const& elem : vec) {
            array.push_back(elem);
        }
        return array;
    }
};
