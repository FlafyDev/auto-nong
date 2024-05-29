#include "auto_nong_manager.hpp"
#include "../includes/geode.hpp"

Vec<Shared<ANSong>> AutoNongManager::getNongsFromSongID(int songID) {
  if (m_songIDToNongs.find(songID) == m_songIDToNongs.end()) {
    return Vec<Shared<ANSong>>{};
  }
  return m_songIDToNongs[songID];
}

bool AutoNongManager::anySongExists(Set<int> songIDs) {
  for (int songID : songIDs) {
    if (m_songIDToNongs.find(songID) != m_songIDToNongs.end())
      return true;
  }

  return false;
}

void AutoNongManager::loadIndexesSchedule(float) { loadIndexes(); }

void AutoNongManager::loadIndexes() {
  Vec<String> indexes = Mod::get()->getSettingValue<MultiStringSettingStruct>("indexes").m_strings;

  m_songIDToNongs = {};

  for (String index : indexes) {
    if (index.size() < 5)
      continue;
    log::info("Loading index: {}", index);
    geode::utils::web::AsyncWebRequest()
        .fetch(index)
        .bytes()
        .then([this, index](const geode::ByteVector &r) {
          Vec<uint8_t> rVec = r;
          bool isGzip = (rVec.size() > 2) && (rVec[0] == 0x1F) && (rVec[1] == 0x8B);

          String jsonString;
          if (isGzip) {
            jsonString = decompressGz(rVec);
          } else {
            jsonString = String(rVec.begin(), rVec.end());
          }

          matjson::Value jsonObj = matjson::parse(jsonString);

          for (const auto &songData : jsonObj.as_array()) {
            String name = songData["name"].as_string();
            String artist = songData.contains("artist") ? songData["artist"].as_string() : "";
            String source = songData["source"].as_string();

            Shared<ANSong> song;
            if (source == "youtube") {
              String yt_id = songData["yt-id"].as_string();
              song = std::make_shared<ANYTSong>(name, artist, index, yt_id);
            } else if (source == "host") {
              String url = songData["url"].as_string();
              song = std::make_shared<ANHostSong>(name, artist, index, url);
            } else {
              log::warn("Unsupported source: {}", source);
              continue;
            }

            if (songData.contains("songs")) {
              for (matjson::Value songID : songData["songs"].as_array()) {
                m_songIDToNongs[songID.as_int()].push_back(song);
              }
            }
          }
          Loader::get()->queueInMainThread(
              [this, index]() { log::info("Loaded index: {}", index); });
        })
        .send();
  }
}

void AutoNongManager::setCurrentLevelID(int levelID) { m_currentLevelID = levelID; }

int AutoNongManager::getCurrentLevelID() { return m_currentLevelID; }
