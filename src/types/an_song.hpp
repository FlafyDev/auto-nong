#pragma once 

#include <iostream>

class ANYTSong;
class ANHostSong;

class ANSong {
public:
    ANSong(const std::string& name, const std::string& artist, const std::string& index) : m_name(name), m_artist(artist), m_index(index) {}
    std::string m_name;
    std::string m_artist;
    std::string m_index;
    virtual ~ANSong() {}
    std::string getSource();
};

class ANYTSong : public ANSong {
public:
    ANYTSong(const std::string& name, const std::string& artist, const std::string& index, const std::string& yt_id) : ANSong(name, artist, index), m_ytId(yt_id) {}
    std::string m_ytId;
};

class ANHostSong : public ANSong {
public:
    ANHostSong(const std::string& name, const std::string& artist, const std::string& index, const std::string& url) : ANSong(name, artist, index), m_url(url) {}
    std::string m_url;
};

