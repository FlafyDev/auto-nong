#include "auto_nong_manager.hpp"

std::vector<std::shared_ptr<ANSong>> AutoNongManager::getNongsFromSongID(int songID) {
  if (m_songIDToNongs.find(songID) == m_songIDToNongs.end()) {
    return std::vector<std::shared_ptr<ANSong>>{};
  }
  return m_songIDToNongs[songID];
}

bool AutoNongManager::anySongExists(std::set<int> songIDs) {
  for (int songID : songIDs) {
    if (m_songIDToNongs.find(songID) != m_songIDToNongs.end())
      return true;
  }

  return false;
}

void AutoNongManager::loadIndexesSchedule(float) { loadIndexes(); }

void AutoNongManager::loadIndexes() {
  std::vector<std::string> indexes =
      Mod::get()->getSettingValue<MultiStringSettingStruct>("indexes").m_strings;

  m_songIDToNongs = {};

  for (std::string index : indexes) {
    if (index.size() < 5)
      continue;
    geode::utils::web::AsyncWebRequest().fetch(index).bytes().then(
        [this, index](const geode::ByteVector &r) {
          std::vector<uint8_t> rVec = r;
          std::string jsonString = decompressGz(rVec);

          matjson::Value jsonObj = matjson::parse(jsonString);

          for (const auto &songData : jsonObj.as_array()) {
            std::string name = songData["name"].as_string();
            std::string artist = songData.contains("artist") ? songData["artist"].as_string() : "";
            std::string source = songData["source"].as_string();

            std::shared_ptr<ANSong> song;
            if (source == "youtube") {
              std::string yt_id = songData["yt-id"].as_string();
              song = std::make_shared<ANYTSong>(name, artist, index, yt_id);
            } else if (source == "host") {
              std::string url = songData["url"].as_string();
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
        });
  }
}

void AutoNongManager::setCurrentLevelID(int levelID) { m_currentLevelID = levelID; }

int AutoNongManager::getCurrentLevelID() { return m_currentLevelID; }
