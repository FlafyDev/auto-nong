#include "auto_nong_manager.hpp"
#include <matjson.hpp>

std::vector<std::shared_ptr<ANSong>> AutoNongManager::getNongsFromSongID(int songID) {
  if (m_songIDToNongs.find(songID) == m_songIDToNongs.end()) {
    return {};
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

void AutoNongManager::reloadLocalIndex() {
  for (auto &pair : m_songIDToNongs) {
    auto &songs = pair.second;
    songs.erase(std::remove_if(
                    songs.begin(), songs.end(),
                    [](const std::shared_ptr<ANSong> &song) { return song->m_index == "local"; }),
                songs.end());
  }

  auto localIndex = Mod::get()->getSavedValue<std::vector<ANSong *>>("local-index");

  log::info("Reloading local index with {} songs", localIndex.size());
  loadIndex(localIndex, "local");
}

void AutoNongManager::removeSongFromLocalIndex(ANSong *song) {
  auto localIndex = Mod::get()->getSavedValue<matjson::Array>("local-index", {});
  localIndex.erase(std::remove_if(localIndex.begin(), localIndex.end(),
                                  [song](const matjson::Value &value) {
                                    return matjson::Serialize<ANSong *>::to_json(song) == value;
                                  }),
                   localIndex.end());
  Mod::get()->setSavedValue<matjson::Array>("local-index", localIndex);
  reloadLocalIndex();
}

void AutoNongManager::addSongToLocalIndex(ANSong *song) {
  auto localIndex = Mod::get()->getSavedValue<matjson::Array>("local-index", {});
  localIndex.push_back(matjson::Serialize<ANSong *>::to_json(song));
  Mod::get()->setSavedValue<matjson::Array>("local-index", localIndex);
  reloadLocalIndex();
}

void AutoNongManager::loadIndexesSchedule(float) { loadIndexes(); }

void AutoNongManager::loadIndexes() {
  std::vector<std::string> indexes =
      Mod::get()->getSettingValue<MultiStringSettingStruct>("indexes").m_strings;

  reloadLocalIndex();

  for (std::string index : indexes) {
    if (index.size() < 5)
      continue;
    log::info("Loading index: {}", index);
    geode::utils::web::AsyncWebRequest()
        .fetch(index)
        .bytes()
        .then([this, index](const geode::ByteVector &r) {
          std::vector<uint8_t> rVec = r;
          bool isGzip = (rVec.size() > 2) && (rVec[0] == 0x1F) && (rVec[1] == 0x8B);

          std::string jsonString;
          if (isGzip) {
            jsonString = decompressGz(rVec, true);
          } else {
            jsonString = std::string(rVec.begin(), rVec.end());
          }

          std::string error;
          std::optional<matjson::Value> jsonObj = matjson::parse(jsonString, error);

          if (!jsonObj.has_value()) {
            log::warn("Failed to parse JSON: {}", error);
            Loader::get()->queueInMainThread(
                [this, index]() { log::info("Loaded index: {}", index); });
            return;
          }

          auto vectors = matjson::Serialize<std::vector<ANSong *>>::from_json(jsonObj.value());

          this->loadIndex(vectors, index);
        })
        .send();
  }
}

void AutoNongManager::loadIndex(const std::vector<ANSong *> &indexJson, const std::string &index) {
  for (ANSong *song : indexJson) {
    for (const int songID : song->m_songIDs) {
      song->m_index = index;
      m_songIDToNongs[songID].push_back(std::shared_ptr<ANSong>(song));
    }
  }
  Loader::get()->queueInMainThread([this, index]() { log::info("Loaded index: {}", index); });
}

void AutoNongManager::setCurrentLevelID(int levelID) { m_currentLevelID = levelID; }

int AutoNongManager::getCurrentLevelID() { return m_currentLevelID; }
