#include "an_song.hpp"
#include "../utils/utils.hpp"

std::string ANYTSong::getHash() const { return hashValues(m_ytId); }

std::string ANYTSong::getSource() const { return "youtube"; }

std::string ANHostSong::getHash() const { return hashValues(m_url); }

std::string ANHostSong::getSource() const { return "host"; }
