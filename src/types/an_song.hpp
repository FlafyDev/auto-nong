#pragma once

#include "../includes/geode.hpp"

class ANYTSong;
class ANHostSong;

class ANSong {
public:
  ANSong(str name, str artist, str index) : m_name(name), m_artist(artist), m_index(index) {}
  String m_name;
  String m_artist;
  String m_index;
  virtual ~ANSong() {}
  String getSource();
};

class ANYTSong : public ANSong {
public:
  ANYTSong(str name, str artist, str index, str yt_id)
      : ANSong(name, artist, index), m_ytId(yt_id) {}
  String m_ytId;
};

class ANHostSong : public ANSong {
public:
  ANHostSong(str name, str artist, str index, str url) : ANSong(name, artist, index), m_url(url) {}
  String m_url;
};
