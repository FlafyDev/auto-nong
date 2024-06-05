#include "an_song.hpp"
#include "../utils/utils.hpp"

std::string ANSong::getSource() {
  if (typeid(*this) == typeid(ANYTSong)) {
    return "youtube";
  } else if (typeid(*this) == typeid(ANHostSong)) {
    return "host";
  } else {
    return "unknown";
  }
}

std::string ANYTSong::getHash() {
  return hashValues(m_name, m_artist, m_index, m_startOffsetMS, m_ytId);
}

std::string ANHostSong::getHash() {
  return hashValues(m_name, m_artist, m_index, m_startOffsetMS, m_url);
}
