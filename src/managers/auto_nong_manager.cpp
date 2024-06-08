#include "auto_nong_manager.hpp"
#include <matjson.hpp>

void AutoNongManager::registerDropdownLayer(ANDropdownLayer *layer) { m_dropdownLayer = layer; }
void AutoNongManager::unregisterDropdownLayer() { m_dropdownLayer = std::nullopt; }

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

void AutoNongManager::loadLocalIndex() {
  auto localIndex = Mod::get()->getSavedValue<std::vector<ANSong *>>("local-index");
  loadIndex(localIndex, "local");
}

void AutoNongManager::unloadAllIndexes() { m_songIDToNongs.clear(); }

void AutoNongManager::unloadIndex(const std::string &index) {
  for (auto &pair : m_songIDToNongs) {
    auto &songs = pair.second;
    songs.erase(std::remove_if(songs.begin(), songs.end(),
                               [index](const std::shared_ptr<ANSong> &song) {
                                 return song->m_index == index;
                               }),
                songs.end());
  }
  if (m_dropdownLayer.has_value()) {
    m_dropdownLayer.value()->updateCells();
  }
}

void AutoNongManager::removeSongFromLocalIndex(ANSong *song) {
  auto localIndex = Mod::get()->getSavedValue<matjson::Array>("local-index", {});
  localIndex.erase(std::remove_if(localIndex.begin(), localIndex.end(),
                                  [song](const matjson::Value &value) {
                                    return matjson::Serialize<ANSong *>::to_json(song) == value;
                                  }),
                   localIndex.end());
  Mod::get()->setSavedValue<matjson::Array>("local-index", localIndex);
  loadLocalIndex();
}

void AutoNongManager::addSongToLocalIndex(ANSong *song) {
  auto localIndex = Mod::get()->getSavedValue<matjson::Array>("local-index", {});
  localIndex.push_back(matjson::Serialize<ANSong *>::to_json(song));
  Mod::get()->setSavedValue<matjson::Array>("local-index", localIndex);
  loadLocalIndex();
}

fs::path AutoNongManager::getSongPath(const ANSong &song, bool create) {
  const auto baseDir = fs::path(Loader::get()->getLoadedMod("fleym.nongd")->getSaveDir()) / "nongs";

  if (create) {
    fs::create_directories(baseDir);
  }

  const std::string filename = fmt::format("autonong-{}.mp3", song.getHash());

  return baseDir / filename;
}

// ANDownloadTask AutoNongManager::createCobaltDownload(std::shared_ptr<ANSong> song) {}
//
// ANDownloadTask AutoNongManager::createYtDlpDownload(std::shared_ptr<ANSong> song) {}

void AutoNongManager::createHostDownload(std::shared_ptr<ANHostSong> song, fs::path savePath,
                                         std::optional<int> songJukeboxId) {
  ANDownloadSongTask task = web::WebRequest()
                                .get(song->m_url)
                                .map(
                                    [](web::WebResponse *response) -> ANDownloadSongTask::Value {
                                      if (response->ok()) {
                                        return Ok(response->data());
                                      }
                                      return Err("Failed to fetch");
                                    },
                                    [](web::WebProgress *progress) -> ANDownloadSongTask::Progress {
                                      return progress->downloadProgress().value_or(0);
                                    });
  registerDownloadTask(task, song, savePath, songJukeboxId);
}

void AutoNongManager::registerDownloadTask(ANDownloadSongTask task, std::shared_ptr<ANSong> song,
                                           fs::path path, std::optional<int> songJukeboxId) {
  if (m_downloadListeners.find(path) != m_downloadListeners.end()) {
    m_downloadListeners.erase(path);
  }

  m_downloadListeners.emplace(path, EventListener<ANDownloadSongTask>());

  m_downloadListeners[path].bind([this, song, path,
                                  songJukeboxId](ANDownloadSongTask::Event *event) {
    if (ANDownloadSongTask::Value *result = event->getValue()) {
      m_downloadListeners.erase(path);
      if (result->isErr()) {
        return;
      }

      std::ofstream file(path, std::ios::out | std::ios::binary);
      file.write(reinterpret_cast<const char *>(result->value().data()), result->value().size());
      file.close();

      if (songJukeboxId.has_value()) {
        AutoNongManager::get()->setSong(*song, songJukeboxId.value());
      }
      if (m_dropdownLayer.has_value()) {
        m_dropdownLayer.value()->updateCellsButtonsState();
      }
    } else if (float *progress = event->getProgress()) {

    } else if (event->isCancelled()) {
      m_downloadListeners.erase(path);
    }
  });
  m_downloadListeners[path].setFilter(task);
}

bool AutoNongManager::isSongActiveInJB(const ANSong &song, int songJukeboxId) {
  const auto jbSong = jukebox::getActiveNong(songJukeboxId);

  if (!jbSong) {
    return false;
  }

  const auto songPath = getSongPath(song, false);

  // TODO(Jukebox PR): startoffset
  return jbSong->songName == song.m_name && jbSong->authorName == song.m_artist &&
         jbSong->path == songPath;
}

void AutoNongManager::deleteSong(const ANSong &song, int songJukeboxId) {
  const auto songPath = getSongPath(song, false);

  const bool isActive = jukebox::getActiveNong(songJukeboxId)->path == songPath;

  jukebox::deleteNong(
      jukebox::SongInfo{
          .path = songPath, .songName = song.m_name, .authorName = song.m_artist,
          // .startOffset = m_anSong->m_startOffsetMS, // TODO(Jukebox PR): startOffset
      },
      songJukeboxId);

  if (fs::exists(songPath)) {
    fs::remove(songPath);
  }

  if (isActive) {
    const auto defaultSong = jukebox::getDefaultNong(songJukeboxId);
    if (defaultSong.has_value()) {
      jukebox::setActiveNong(defaultSong.value(), songJukeboxId, getCustomSongWidget());
    }
  }

  if (m_dropdownLayer.has_value()) {
    m_dropdownLayer.value()->updateCellsButtonsState();
  }
}

std::optional<Ref<CustomSongWidget>> AutoNongManager::getCustomSongWidget() {
  if (m_dropdownLayer.has_value()) {
    Ref<CustomSongWidget> customSongWidget = m_dropdownLayer.value()->m_parentWidget;
    if (customSongWidget) {
      return customSongWidget;
    }
  }
  return std::nullopt;
}

void AutoNongManager::unsetSong(int songJukeboxId) {
  const auto defaultSong = jukebox::getDefaultNong(songJukeboxId);
  if (defaultSong.has_value()) {
    jukebox::setActiveNong(defaultSong.value(), songJukeboxId, getCustomSongWidget());
    if (m_dropdownLayer.has_value()) {
      m_dropdownLayer.value()->updateCellsButtonsState();
    }
  }
}

void AutoNongManager::setSong(const ANSong &song, int songJukeboxId) {
  const auto songPath = getSongPath(song);

  try {
    if (m_dropdownLayer.has_value()) {
      m_dropdownLayer.value()->updateCellsButtonsState();
    }

    if (!fs::exists(songPath)) {
      Notification::create("File doesn't exist", NotificationIcon::Error)->show();
      return;
    }

    jukebox::SongInfo jbSong = {
        .path = songPath, .songName = song.m_name, .authorName = song.m_artist, .songUrl = "local",
        // .startOffset = m_anSong->m_startOffsetMS, // TODO(jukebox PR): startOffset
    };

    jukebox::deleteNong(jbSong, songJukeboxId); // TODO(jukebox PR): false
    jukebox::addNong(jbSong, songJukeboxId);
    jukebox::setActiveNong(jbSong, songJukeboxId, getCustomSongWidget());
  } catch (const std::exception &e) {
    log::error("Failed to set song: {}", e.what());
    Notification::create("Failed to set song", NotificationIcon::Error)->show();
  }
  if (m_dropdownLayer.has_value()) {
    m_dropdownLayer.value()->updateCellsButtonsState();
  }
}

void AutoNongManager::downloadSong(std::shared_ptr<ANSong> song, std::optional<int> songJukeboxId) {
  const auto songPath = getSongPath(*song, true);

  if (fs::exists(songPath)) {
    fs::remove(songPath);
  }

  if (song->getSource() == "youtube") {
    // TODO: youtube download methods
    // if (Mod::get()->getSettingValue<bool>("use-cobalt")) {
    //   downloadFromCobalt();
    // } else if (Mod::get()->getSettingValue<bool>("use-yt-dlp")) {
    //   downloadFromYtDlp();
    // } else {
    //   Notification::create("No download method for YouTube", NotificationIcon::Error)->show();
    // }
  } else if (song->getSource() == "host") {
    const ANHostSong *hostSong = static_cast<ANHostSong *>(song.get());
    std::shared_ptr<ANHostSong> convertedHostSong = std::dynamic_pointer_cast<ANHostSong>(song);
    createHostDownload(convertedHostSong, songPath, songJukeboxId);
  }
}

void AutoNongManager::loadIndexes() {
  std::vector<std::string> indexes =
      Mod::get()->getSettingValue<MultiStringSettingStruct>("indexes").m_strings;

  unloadAllIndexes();
  loadLocalIndex();

  for (std::string index : indexes) {
    if (index.size() < 5)
      continue;
    loadIndexFromURL(index);
  }
}

void AutoNongManager::loadIndexFromURL(const std::string &index) {
  if (m_indexListeners.find(index) != m_indexListeners.end()) {
    m_indexListeners.erase(index);
  }

  log::info("Loading Auto Nong index from url: {}", index);

  m_indexListeners.emplace(index, EventListener<ANDownloadIndexTask>());

  m_indexListeners[index].bind([this, index](ANDownloadIndexTask::Event *event) {
    log::info("event for index {}", index);
    if (ANDownloadIndexTask::Value *result = event->getValue()) {
      if (result->isErr()) {
        return;
      }
      this->loadIndex(result->value(), index);
    } else if (float *progress = event->getProgress()) {

    } else if (event->isCancelled()) {
      this->m_indexListeners.erase(index);
    }
  });

  const auto task = web::WebRequest().get(index).map(
      [](web::WebResponse *response) -> ANDownloadIndexTask::Value {
        if (response->ok()) {
          return Ok(response->data());
        }
        return Err("Failed to fetch");
      },
      [](web::WebProgress *progress) -> ANDownloadIndexTask::Progress {
        return progress->downloadProgress().value_or(0);
      });

  m_indexListeners[index].setFilter(task);
};

void AutoNongManager::loadIndex(ByteVector &indexBytes, const std::string &index) {
  bool isGzip = (indexBytes.size() > 2) && (indexBytes[0] == 0x1F) && (indexBytes[1] == 0x8B);

  std::string jsonString;
  if (isGzip) {
    jsonString = decompressGz(indexBytes, true);
  } else {
    jsonString = std::string(indexBytes.begin(), indexBytes.end());
  }

  std::string error;
  std::optional<matjson::Value> jsonObj = matjson::parse(jsonString, error);

  if (!jsonObj.has_value()) {
    log::warn("Failed to parse JSON: {}", error);
    Loader::get()->queueInMainThread([this, index]() { log::info("Loaded index: {}", index); });
    return;
  }

  auto vectors = matjson::Serialize<std::vector<ANSong *>>::from_json(jsonObj.value());

  this->loadIndex(vectors, index);
}

void AutoNongManager::loadIndex(const std::vector<ANSong *> &indexJson, const std::string &index) {
  unloadIndex(index);
  for (ANSong *song : indexJson) {
    for (const int songID : song->m_songIDs) {
      song->m_index = index;
      m_songIDToNongs[songID].push_back(std::shared_ptr<ANSong>(song));
    }
  }
  if (m_dropdownLayer.has_value()) {
    m_dropdownLayer.value()->updateCells();
  }
}

std::optional<std::string> AutoNongManager::generatePublishUrl(const ANSong &song, int songId) {
  const auto title = fmt::format("Add {} by {}", song.m_name, song.m_artist);
  const std::string extra =
      fmt::format("Created from Auto Nong.\nCreated for level: {}", getCurrentLevelID());

  const std::string extraEncoded = replaceAll(extra, "\n", "%0A");

  if (song.getSource() == "youtube") {
    auto ytSong = dynamic_cast<const ANYTSong *>(&song);

    return fmt::format("https://github.com/FlafyDev/auto-nong-indexes/issues/"
                       "new?template=add-nong-song.yml&title={}&song-name={}&artist-name={}&"
                       "youtube-link={}&song-id={}&extra={}",
                       title, song.m_name, song.m_artist, ytSong->m_ytId, songId, extraEncoded);
  }
  return std::nullopt;
}

void AutoNongManager::setCurrentLevelID(int levelID) { m_currentLevelID = levelID; }

int AutoNongManager::getCurrentLevelID() { return m_currentLevelID; }
