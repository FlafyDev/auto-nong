#include "an_song.hpp"

std::string ANSong::getSource() {
  if (typeid(*this) == typeid(ANYTSong)) {
    return "youtube";
  } else if (typeid(*this) == typeid(ANHostSong)) {
    return "host";
  } else {
    return "unknown";
  }
}
