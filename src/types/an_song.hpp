#pragma once 

#include <iostream>

class ANYTSong;
class ANHostSong;

class ANSong {
public:
    ANSong(const std::string& name, const std::string& artist, const std::string& index) : name(name), artist(artist), index(index) {}
    std::string name;
    std::string artist;
    std::string index;
    virtual ~ANSong() {}
    std::string getSource();
};

class ANYTSong : public ANSong {
public:
    ANYTSong(const std::string& name, const std::string& artist, const std::string& index, const std::string& yt_id) : ANSong(name, artist, index), yt_id(yt_id) {}
    std::string yt_id;
};

class ANHostSong : public ANSong {
public:
    ANHostSong(const std::string& name, const std::string& artist, const std::string& index, const std::string& url) : ANSong(name, artist, index), url(url) {}
    std::string url;
};

