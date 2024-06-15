#include "auto_nong_manager.hpp"
#include "Geode/cocos/platform/CCPlatformMacros.h"
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

ANSongDownloadStatus AutoNongManager::getSongDownloadStatus(const ANSong &song,
                                                            std::optional<int> songJukeboxId) {
  const auto songPath = AutoNongManager::get()->getSongPath(song, false);

  if (fs::exists(songPath)) {
    m_downloadTempStatus.erase(songPath);

    if (songJukeboxId.has_value() &&
        AutoNongManager::get()->isSongActiveInJB(song, songJukeboxId.value())) {
      return ANSongDownloadStatus::Active();
    }
    return ANSongDownloadStatus::Downloaded();
  }

  if (m_downloadTempStatus.contains(songPath)) {
    return m_downloadTempStatus[songPath];
  }

  // TODO: check if downloading or failed
  return ANSongDownloadStatus::NotDownloaded();
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

void AutoNongManager::createCobaltDownload(std::shared_ptr<ANYTSong> song, fs::path path,
                                           std::optional<int> songJukeboxId) {
  if (m_cobaltMetadataListeners.find(path) != m_cobaltMetadataListeners.end()) {
    m_cobaltMetadataListeners.erase(path);
  }
  const std::string apiUrl = "https://co.wuk.sh/api/json";
  const std::string videoId = song->m_ytId;

  std::unordered_map<std::string, std::string> parameters = {
      {"url", fmt::format("https://www.youtube.com/watch?v={}", videoId)},
      {"aFormat", "mp3"},
      {"isAudioOnly", "true"}};

  matjson::Value parameters_json = matjson::Object();
  for (auto const &[key, value] : parameters) {
    parameters_json[key] = value;
  }

  m_cobaltMetadataListeners.emplace(path, EventListener<ANCobaltMetadataTask>());

  ANCobaltMetadataTask metadataTask =
      web::WebRequest()
          .bodyJSON(parameters_json)
          .header("Accept", "application/json")
          .header("Content-Type", "application/json")
          .post(apiUrl)
          .map(
              [](web::WebResponse *response) -> ANCobaltMetadataTask::Value {
                if (response->ok() && response->string().isOk()) {
                  std::string error;
                  std::optional<matjson::Value> jsonObj =
                      matjson::parse(response->string().value(), error);

                  if (!jsonObj.has_value()) {
                    return Err(error);
                  }

                  return Ok(jsonObj.value());
                }
                return Err("Failed to fetch");
              },
              [](web::WebProgress *progress) -> ANCobaltMetadataTask::Progress {
                return progress->downloadProgress().value_or(0) / 100.f * 0.1f; // 0% to 10%
              });

  m_cobaltMetadataListeners[path].bind([this, song, path,
                                        songJukeboxId](ANCobaltMetadataTask::Event *event) {
    if (ANCobaltMetadataTask::Value *result = event->getValue()) {
      m_cobaltMetadataListeners.erase(path);

      if (result->isErr()) {
        setDownloadErrorForSong(*song, fmt::format("Cobalt metadata failed: {}", result->err()));
        return;
      }

      if (result->value()["status"] != "stream") {
        setDownloadErrorForSong(*song, "Cobalt metadata failed: Incorrect result");
        return;
      }

      std::string audio_url = result->value()["url"].as_string();

      ANDownloadSongTask task = web::WebRequest().get(audio_url).map(
          [](web::WebResponse *response) -> ANDownloadSongTask::Value {
            if (response->ok()) {
              return Ok(response->data());
            }
            return Err("Failed to fetch");
          },
          [](web::WebProgress *progress) -> ANDownloadSongTask::Progress {
            return progress->downloadProgress().value_or(0) / 100.f * 0.9f + 0.1f; // 10% to 100%
          });

      registerDownloadTask(task, song, path, songJukeboxId);
    } else if (float *progress = event->getProgress()) {
      setDownloadProgressForSong(*song, *progress);
    } else if (event->isCancelled()) {
      setDownloadErrorForSong(*song, "Cobalt metadata failed: Cancelled");
      m_cobaltMetadataListeners.erase(path);
    }
  });

  m_cobaltMetadataListeners[path].setFilter(metadataTask);
}

// TODO: handle android
void AutoNongManager::createYtDlpDownload(std::shared_ptr<ANYTSong> song, fs::path path,
                                          std::optional<int> songJukeboxId) {
  ANDownloadSongTask task = ANDownloadSongTask::runWithCallback(
      [this, song, path](auto postResult, auto postProgress, auto hasBeenCancelled) {
        const fs::path ytDlpPath = Mod::get()->getSettingValue<std::string>("yt-dlp-path");

        if (!fs::exists(ytDlpPath)) {
          postResult(Err("yt-dlp.exe not found at specified path"));
          return;
        }

        std::string command = fmt::format("\"{}\" -x --audio-format mp3 \"{}\" -o \"{}\"",
                                          ytDlpPath, song->m_ytId, path);

        int result = std::system(fmt::format("\"{}\"", command).c_str());

        log::info("command({}): {}", result, command);

        if (result != 0) {
          log::info("Failed to execute command({}): {}", result, command);
          postResult(Err("Failed to execute command"));
          return;
        }

        postResult(Ok(std::nullopt));
      });

  registerDownloadTask(task, song, path, songJukeboxId);
}

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
                                      return progress->downloadProgress().value_or(0) / 100.f;
                                    });
  registerDownloadTask(task, song, savePath, songJukeboxId);
}

void AutoNongManager::registerDownloadTask(ANDownloadSongTask task, std::shared_ptr<ANSong> song,
                                           fs::path path, std::optional<int> songJukeboxId) {
  if (m_downloadListeners.find(path) != m_downloadListeners.end()) {
    m_downloadListeners.erase(path);
  }

  m_downloadListeners.emplace(path, EventListener<ANDownloadSongTask>());

  m_downloadListeners[path].bind(
      [this, song, path, songJukeboxId](ANDownloadSongTask::Event *event) {
        if (ANDownloadSongTask::Value *result = event->getValue()) {
          m_downloadListeners.erase(path);
          if (result->isErr()) {
            setDownloadErrorForSong(*song, fmt::format("Song download failed: {}", result->err()));
            return;
          }

          if (result->value().has_value()) {
            std::ofstream file(path, std::ios::out | std::ios::binary);
            file.write(reinterpret_cast<const char *>(result->value().value().data()),
                       result->value().value().size());
            file.close();
          }

          if (songJukeboxId.has_value()) {
            AutoNongManager::get()->setSong(*song, songJukeboxId.value());
          }
          if (m_dropdownLayer.has_value()) {
            m_dropdownLayer.value()->updateCellsButtonsState();
          }
          setDownloadErrorForSong(*song, fmt::format("Downloaded {}", song->m_name));
        } else if (float *progress = event->getProgress()) {
          setDownloadProgressForSong(*song, *progress);
        } else if (event->isCancelled()) {
          setDownloadErrorForSong(*song, fmt::format("Cancelled downloading {}", song->m_name));
          m_downloadListeners.erase(path);
        }
      });

  m_downloadListeners[path].setFilter(task);
}

void AutoNongManager::setDownloadProgressForSong(const ANSong &song, float progress) {
  m_downloadTempStatus[getSongPath(song)] = ANSongDownloadStatus::Downloading(progress);

  if (m_dropdownLayer.has_value()) {
    m_dropdownLayer.value()->updateCellsButtonsState();
  }
}
void AutoNongManager::setDownloadErrorForSong(const ANSong &song, const std::string &error) {
  m_downloadTempStatus[getSongPath(song)] = ANSongDownloadStatus::Failed(error);

  if (m_dropdownLayer.has_value()) {
    m_dropdownLayer.value()->updateCellsButtonsState();
  }
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

    jukebox::deleteNong(jbSong, songJukeboxId, false);
    jukebox::addNong(jbSong, songJukeboxId);
    jukebox::setActiveNong(jbSong, songJukeboxId, getCustomSongWidget());
  } catch (const std::exception &e) {
    log::error("Failed to set song: {}", e.what());
    Notification::create("Failed to set song: {}", NotificationIcon::Error)->show();
  }
  if (m_dropdownLayer.has_value()) {
    m_dropdownLayer.value()->updateCellsButtonsState();
  }
}

void AutoNongManager::downloadSong(std::shared_ptr<ANSong> song, std::optional<int> songJukeboxId) {
  Notification::create("Downloading...", NotificationIcon::None)->show();
  const auto songPath = getSongPath(*song, true);

  if (fs::exists(songPath)) {
    fs::remove(songPath);
  }

  if (song->getSource() == "youtube") {
    if (Mod::get()->getSettingValue<bool>("use-cobalt")) {
      createCobaltDownload(std::dynamic_pointer_cast<ANYTSong>(song), songPath, songJukeboxId);
    } else if (Mod::get()->getSettingValue<bool>("use-yt-dlp")) {
      createYtDlpDownload(std::dynamic_pointer_cast<ANYTSong>(song), songPath, songJukeboxId);
    } else {
      setDownloadErrorForSong(*song, "No download method for YouTube");
    }
  } else if (song->getSource() == "host") {
    createHostDownload(std::dynamic_pointer_cast<ANHostSong>(song), songPath, songJukeboxId);
  } else {
    setDownloadErrorForSong(*song, "Unknown song source");
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
    if (ANDownloadIndexTask::Value *result = event->getValue()) {
      this->m_indexListeners.erase(index);
      if (result->isErr()) {
        Notification::create(fmt::format("Index download failed: {}", result->err()),
                             NotificationIcon::Error)
            ->show();
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
