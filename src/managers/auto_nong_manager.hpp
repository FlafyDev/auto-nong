#pragma once

#include "../includes/geode.hpp"
#include "../types/an_song.hpp"
#include "../ui/an_clear_popup_blacklist.hpp"
#include "../ui/multi_string_setting.hpp"
#include "../ui/an_song_cell.hpp"
#include "../utils/utils.hpp"
#include "Geode/loader/Event.hpp"

using namespace geode::prelude;

class ANYTSong;
class ANHostSong;

using ANDownloadSongTask = Task<Result<ByteVector, std::string>, float>;
using ANDownloadIndexTask = Task<Result<ByteVector, std::string>, float>;

class AutoNongManager : public CCNode {
protected:
  inline static AutoNongManager *m_instance = nullptr;
  int m_currentLevelID = 0;
  std::map<fs::path, EventListener<ANDownloadSongTask>>
      m_downloadListeners;
  std::map<std::string, EventListener<ANDownloadIndexTask>> m_indexListeners;
  std::map<int, std::vector<std::shared_ptr<ANSong>>> m_songIDToNongs;
  std::optional<ANDropdownLayer*> m_dropdownLayer = std::nullopt;

  void loadIndexFromURL(const std::string &index);
  void loadIndex(const std::vector<ANSong *> &indexJson, const std::string &index);
  void loadIndex(ByteVector &indexJson, const std::string &index);
  void unloadIndex(const std::string& index);
  void unloadAllIndexes();
  void loadLocalIndex();

  std::optional<Ref<CustomSongWidget>> getCustomSongWidget();

  void registerDownloadTask(ANDownloadSongTask task, std::shared_ptr<ANSong> song, fs::path path, std::optional<int> songJukeboxId);
  void createHostDownload(std::shared_ptr<ANHostSong> song, fs::path savePath, std::optional<int> songJukeboxId);
public:
  std::vector<std::shared_ptr<ANSong>> getNongsFromSongID(int songID);
  bool anySongExists(std::set<int> songIDs);

  void registerDropdownLayer(ANDropdownLayer *layer);
  void unregisterDropdownLayer();

  void loadIndexes();
  void removeSongFromLocalIndex(ANSong *song);
  void addSongToLocalIndex(ANSong *song);

  int getCurrentLevelID();
  void setCurrentLevelID(int levelID);

  fs::path getSongPath(const ANSong &song, bool create = false);

  bool isSongActiveInJB(const ANSong &song, int songJukeboxId);
  void deleteSong(const ANSong& song, int songJukeboxId);
  void setSong(const ANSong &song, int songJukeboxId);
  void unsetSong(int songJukeboxId);
  void downloadSong(std::shared_ptr<ANSong> song, std::optional<int> songJukeboxId);
  
  std::optional<std::string> generatePublishUrl(const ANSong &song, int songId);

  static AutoNongManager *get() {
    if (m_instance == nullptr) {
      m_instance = new AutoNongManager;
      m_instance->retain();
    }

    return m_instance;
  }
};
