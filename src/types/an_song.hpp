#pragma once

class ANYTSong;
class ANHostSong;

class ANSong {
public:
  ANSong(const std::string &name, const std::string &artist, const std::string &index, const int startOffsetMS)
      : m_name(name), m_artist(artist), m_index(index), m_startOffsetMS(startOffsetMS) {}
  std::string m_name;
  std::string m_artist;
  std::string m_index;
  int m_startOffsetMS;
  virtual ~ANSong() {}
  virtual std::string getHash() = 0;
  std::string getSource();
};

class ANYTSong : public ANSong {
public:
  ANYTSong(const std::string &name, const std::string &artist, const std::string &index, const int startOffsetMS,
           const std::string &yt_id)
      : ANSong(name, artist, index, startOffsetMS), m_ytId(yt_id) {}
  std::string m_ytId;
  std::string getHash() override;
};

class ANHostSong : public ANSong {
public:
  ANHostSong(const std::string &name, const std::string &artist, const std::string &index, const int startOffsetMS,
             const std::string &url)
      : ANSong(name, artist, index, startOffsetMS), m_url(url) {}
  std::string m_url;
  std::string getHash() override;
};
