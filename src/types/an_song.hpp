#pragma once

#include "../includes/geode.hpp"
#include "serializable_vector.hpp"
#include <matjson.hpp>
#include <memory>

class ANYTSong;
class ANHostSong;

class ANSong {
public:
  ANSong(const std::string &name, const std::string &artist, const std::string &index,
         const std::vector<int> &songIDs, const int startOffsetMS)
      : m_name(name), m_artist(artist), m_index(index), m_songIDs(songIDs),
        m_startOffsetMS(startOffsetMS) {}
  std::string m_name;
  std::string m_artist;
  std::string m_index;
  int m_startOffsetMS;
  std::vector<int> m_songIDs;
  virtual ~ANSong() = default;
  virtual std::string getHash() const = 0;
  virtual std::string getSource() const = 0;
};

class ANYTSong : public ANSong {
public:
  ANYTSong(const std::string &name, const std::string &artist, const std::string &index,
           const std::vector<int> &songIDs, const int startOffsetMS, const std::string &yt_id)
      : ANSong(name, artist, index, songIDs, startOffsetMS), m_ytId(yt_id) {}
  std::string m_ytId;
  std::string getHash() const override;
  std::string getSource() const override;
};

class ANHostSong : public ANSong {
public:
  ANHostSong(const std::string &name, const std::string &artist, const std::string &index,
             const std::vector<int> &songIDs, const int startOffsetMS, const std::string &url)
      : ANSong(name, artist, index, songIDs, startOffsetMS), m_url(url) {}
  std::string m_url;
  std::string getHash() const override;
  std::string getSource() const override;
};

template <> struct matjson::Serialize<ANSong *> {
  static bool is_json(matjson::Value const &value) {
    return value.is_object() && value.contains("source");
  }

  static ANSong *from_json(matjson::Value const &value) {
    const std::string name = value["name"].as_string();
    const std::string artist = value.contains("artist") ? value["artist"].as_string() : "";
    const std::string source = value["source"].as_string();
    const std::vector<int> songIDs =
        matjson::Serialize<std::vector<int>>::from_json(value["songs"]);
    const int startOffsetMS = value.contains("startOffset") ? value["startOffset"].as_int() : 0;

    if (source == "youtube") {
      const std::string ytId = value["yt-id"].as_string();
      return new ANYTSong(name, artist, "", songIDs, startOffsetMS, ytId);
    } else if (source == "host") {
      const std::string url = value["url"].as_string();
      return new ANHostSong(name, artist, "", songIDs, startOffsetMS, url);
    } else {
      return nullptr;
    }
  }

  static matjson::Value to_json(const ANSong *song) {
    matjson::Object obj;
    obj["name"] = song->m_name;
    obj["artist"] = song->m_artist;
    obj["index"] = song->m_index;
    obj["startOffset"] = song->m_startOffsetMS;
    obj["songs"] = matjson::Serialize<std::vector<int>>::to_json(song->m_songIDs);
    obj["source"] = song->getSource();

    if (auto ytSong = dynamic_cast<const ANYTSong *>(song)) {
      obj["yt-id"] = ytSong->m_ytId;
    } else if (auto hostSong = dynamic_cast<const ANHostSong *>(song)) {
      obj["url"] = hostSong->m_url;
    }

    return obj;
  }
};

template <> struct matjson::Serialize<std::vector<ANSong *>> {
  static bool is_json(matjson::Value const &value) { return value.is_array(); }

  static std::vector<ANSong *> from_json(matjson::Value const &value) {
    std::vector<ANSong *> vec;
    if (!is_json(value)) {
      return vec;
    }
    auto array = value.as_array();
    for (const auto &elem : array) {
      vec.push_back(matjson::Serialize<ANSong *>::from_json(elem));
    }
    return vec;
  }

  static matjson::Value to_json(std::vector<ANSong> &vec) {
    auto array = matjson::Array();
    for (auto &elem : vec) {
      array.push_back(matjson::Serialize<ANSong *>::to_json(&elem));
    }
    return array;
  }
};
