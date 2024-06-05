#include "an_song_cell.hpp"
#include "../types/an_song.hpp"
#include "../utils/utils.hpp"
#include "fleym.nongd/include/jukebox.hpp"
#include "list_cell.hpp"

bool ANSongCell::init(int songId, int songJukeboxId, ANSong *song, ANDropdownLayer *parentPopup,
                      CCSize const &size, bool isRobtopSong) {
  if (!JBListCell::init(size))
    return false;

  m_songId = songId;
  m_songJukeboxId = songJukeboxId;
  m_anSong = song;
  m_parentPopup = parentPopup;
  m_isRobtopSong = isRobtopSong;
  m_customSongWidget = parentPopup->m_parentWidget;

  CCSprite *spr;
  CCMenu *menu;

  auto height = this->getContentSize().height;

  // Download button
  spr = CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png");
  spr->setScale(0.8f);
  m_downloadButton =
      CCMenuItemSpriteExtra::create(spr, this, menu_selector(ANSongCell::onDownload));
  m_downloadButton->setAnchorPoint(ccp(0.5f, 0.5f));
  m_downloadButton->setID("set-button");
  menu = CCMenu::create();
  menu->addChild(m_downloadButton);
  menu->setAnchorPoint(ccp(0, 0));
  menu->setPosition(ccp(295.f, height / 2));
  menu->setID("button-menu1");
  this->addChild(menu);

  // Set button
  m_setToggle =
      CCMenuItemToggler::createWithStandardSprites(this, menu_selector(ANSongCell::onSetSong), .6f);
  m_setToggle->setPositionX(-10.f);
  menu = CCMenu::create();
  menu->setAnchorPoint(ccp(0, 0));
  menu->setPosition(ccp(272.f, height / 2));
  menu->setID("button-menu2");
  menu->addChild(m_setToggle);
  this->addChild(menu);

  // Delete button
  spr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
  spr->setScale(0.74f);
  m_trashButton = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ANSongCell::onDeleteSong));
  menu = CCMenu::create();
  menu->setAnchorPoint(ccp(0, 0));
  menu->setPosition(ccp(295.f, height / 2));
  menu->setID("button-menu2");
  menu->addChild(m_trashButton);
  this->addChild(menu);

  setButtonsState();

  m_songInfoLayer = CCLayer::create();
  m_songNameLabel = CCLabelBMFont::create(m_anSong->m_name.c_str(), "bigFont.fnt");
  m_songNameLabel->limitLabelWidth(220.f, 0.7f, 0.1f);
  m_songNameLabel->setID("song-name");

  m_artistNameLabel = CCLabelBMFont::create(m_anSong->m_artist.c_str(), "goldFont.fnt");
  m_artistNameLabel->limitLabelWidth(120.f, 0.7f, 0.1f);
  m_artistNameLabel->setID("artist-name");

  m_sourceLabel = CCLabelBMFont::create(
      fmt::format("{} : {}", getSubstringAfterSlash(m_anSong->m_index), m_anSong->getSource())
          .c_str(),
      "goldFont.fnt");
  m_sourceLabel->limitLabelWidth(120.f, 0.7f, 0.1f);
  m_sourceLabel->setID("artist-name");

  m_songInfoLayer->addChild(m_sourceLabel);
  m_songInfoLayer->addChild(m_artistNameLabel);
  m_songInfoLayer->addChild(m_songNameLabel);
  m_songInfoLayer->setID("song-info");
  auto layout = ColumnLayout::create();
  layout->setAutoScale(false);
  layout->setAxisAlignment(AxisAlignment::Center);
  layout->setCrossAxisLineAlignment(AxisAlignment::Start);
  m_songInfoLayer->setContentSize(ccp(240.f, height - 20.f));
  m_songInfoLayer->setAnchorPoint(ccp(0.f, 0.f));
  m_songInfoLayer->setPosition(ccp(12.f, 1.5f));
  m_songInfoLayer->setLayout(layout);

  this->addChild(m_songInfoLayer);
  return true;
}

void ANSongCell::onSetSong(CCObject *target) {
  if (m_setToggle->isToggled() &&
      jukebox::getActiveNong(m_songJukeboxId)->path == getFileDownloadPath(false)) {
    const auto defaultSong = jukebox::getDefaultNong(m_songJukeboxId);
    if (defaultSong.has_value()) {
      jukebox::setActiveNong(defaultSong.value(), m_songJukeboxId, m_customSongWidget);
      if (m_parentPopup) {
        m_parentPopup->updateCellsButtonsState();
      }
    }
  } else {
    setSong();
  }
  // Cancel the toggling of the button that happens after the callback.
  m_setToggle->toggle(!m_setToggle->isToggled());
}

void ANSongCell::setButtonsState() {
  bool downloaded = fs::exists(getFileDownloadPath(false));
  m_trashButton->setVisible(downloaded);
  m_downloadButton->setVisible(!downloaded);
  m_setToggle->setVisible(downloaded);
  m_setToggle->toggle(jukebox::getActiveNong(m_songJukeboxId)->path == getFileDownloadPath(false));
}

void ANSongCell::onDeleteSong(CCObject *target) {
  const fs::path downloadPath = getFileDownloadPath(false);
  const bool isActive = jukebox::getActiveNong(m_songJukeboxId)->path == getFileDownloadPath(false);

  jukebox::deleteNong(
      jukebox::SongInfo{
          .path = downloadPath,
      },
      m_songId);

  if (fs::exists(downloadPath)) {
    fs::remove(downloadPath);
  }

  if (isActive) {
    const auto defaultSong = jukebox::getDefaultNong(m_songJukeboxId);
    if (defaultSong.has_value()) {
      jukebox::setActiveNong(defaultSong.value(), m_songJukeboxId, m_customSongWidget);
    }
  }

  setButtonsState();
}

void ANSongCell::setSong() {
  try {
    if (m_parentPopup) {
      m_parentPopup->updateCellsButtonsState();
    }
    fs::path downloadPath = getFileDownloadPath(false);

    if (!fs::exists(downloadPath)) {
      Notification::create("File doesn't exist", NotificationIcon::Error)->show();
      return;
    }

    jukebox::SongInfo song = {
        .path = downloadPath,
        .songName = m_anSong->m_name,
        .authorName = m_anSong->m_artist,
        .songUrl = "local",
        .startOffset = 0,
    };

    jukebox::addNong(song, m_songJukeboxId);
    jukebox::setActiveNong(song, m_songJukeboxId, m_customSongWidget);
  } catch (const std::exception &e) {
    log::error("Failed to set song: {}", e.what());
    Notification::create("Failed to set song", NotificationIcon::Error)->show();
  }
  if (m_parentPopup) {
    m_parentPopup->updateCellsButtonsState();
  }
}

fs::path ANSongCell::getFileDownloadPath(bool create) {
  std::string baseDir = Mod::get()->getSaveDir().string();

  if (typeid(*m_anSong) == typeid(ANYTSong)) {
    const ANYTSong *ytSong = static_cast<ANYTSong *>(m_anSong);
    const std::string videoId = ytSong->m_ytId;
    if (create) {
      fs::create_directories(fmt::format("{}/youtube", baseDir));
    }
    return fmt::format("{}/youtube/{}.mp3", baseDir, videoId);
  }
  if (typeid(*m_anSong) == typeid(ANHostSong)) {
    const ANHostSong *hostSong = static_cast<ANHostSong *>(m_anSong);
    if (create) {
      fs::create_directories(fmt::format("{}/host", baseDir));
    }
    return fmt::format("{}/host/{}.mp3", baseDir, urlToFilename(hostSong->m_url));
  }
  return "";
}

void ANSongCell::downloadFromYtDlp() {
  m_currentlyDownloading = true;
  const ANYTSong *ytSong = static_cast<ANYTSong *>(m_anSong);
  const fs::path ytDlpPath = Mod::get()->getSettingValue<std::string>("yt-dlp-path");
  const std::string videoId = ytSong->m_ytId;
  const fs::path downloadPath = getFileDownloadPath(true);

  if (!fs::exists(ytDlpPath)) {
    log::info("yt-dlp.exe not found at specified path: {}", ytDlpPath);
    Notification::create("yt-dlp.exe not found at specified path", NotificationIcon::Error)->show();
    return;
  }

  std::string ytDlpCommand = fmt::format("\"{}\" -x --audio-format mp3 \"{}\" -o \"{}\"", ytDlpPath,
                                         videoId, downloadPath);

  auto notif = Notification::create("Downloading using yt dlp", NotificationIcon::Loading, 0);
  notif->setTime(NOTIFICATION_DEFAULT_TIME);
  notif->show();

  std::thread commandThread([this, ytSong, ytDlpCommand]() {
    std::string command = ytDlpCommand;

    int result = std::system(fmt::format("\"{}\"", command).c_str());

    log::info("command({}): {}", result, command);

    m_currentlyDownloading = false;

    if (result != 0) {
      log::info("Failed to execute command({}): {}", result, command);
      Loader::get()->queueInMainThread([]() {
        Notification::create("Failed to download from YouTube", NotificationIcon::Error)->show();
      });
      return;
    }

    log::info("Successfully executed command: {}", command);

    Loader::get()->queueInMainThread([this]() {
      Notification::create("Downloaded", NotificationIcon::Success)->show();
      setSong();
    });
  });

  commandThread.detach();
}

void ANSongCell::downloadFromCobalt() {
  m_currentlyDownloading = true;
  const std::string apiUrl = "https://co.wuk.sh/api/json";
  const ANYTSong *ytSong = static_cast<ANYTSong *>(m_anSong);
  const std::string videoId = ytSong->m_ytId;

  std::unordered_map<std::string, std::string> parameters = {
      {"url", fmt::format("https://www.youtube.com/watch?v={}", videoId)},
      {"aFormat", "mp3"},
      {"isAudioOnly", "true"}};

  matjson::Value parameters_json = matjson::Object();
  for (auto const &[key, value] : parameters) {
    parameters_json[key] = value;
  }
  std::string body = parameters_json.dump(matjson::NO_INDENTATION);

  auto notif = Notification::create("Downloading using Cobalt Tools", NotificationIcon::Loading, 0);
  notif->setTime(NOTIFICATION_DEFAULT_TIME);
  notif->show();

  geode::utils::web::AsyncWebRequest()
      .header("Accept", "application/json")
      .header("Content-Type", "application/json")
      .bodyRaw(body)
      .post(apiUrl)
      .text()
      .then([this](const std::string &r) {
        matjson::Value data = matjson::parse(r);

        if (data["status"] == "stream") {
          std::string audio_url = data["url"].as_string();

          geode::utils::web::AsyncWebRequest()
              .get(audio_url)
              .bytes()
              .then([this](const geode::ByteVector &audio_data) {
                std::ofstream file(getFileDownloadPath(true), std::ios::out | std::ios::binary);
                file.write(reinterpret_cast<const char *>(audio_data.data()), audio_data.size());
                file.close();

                log::info("Downloaded the song successfully!");

                Loader::get()->queueInMainThread([this]() {
                  Notification::create("Downloaded", NotificationIcon::Success)->show();
                  setSong();
                });
                m_currentlyDownloading = false;
              })
              .expect([this](auto _) { m_currentlyDownloading = false; });
        } else {
          log::warn("Failed to download the song: {}", r);
          m_currentlyDownloading = false;
        }
      })
      .expect([this](auto _) { m_currentlyDownloading = false; });
}

void ANSongCell::onDownload(CCObject *target) {
  if (m_currentlyDownloading) {
    FLAlertLayer::create(
        "Currently downloading",
        "Please wait for the current download to finish before downloading the same song again.",
        "OK")
        ->show();
    return;
  }

  if (fs::exists(getFileDownloadPath(false))) {
    createQuickPopup("Song already downloaded", "Are you sure you want to redownload the song?",
                     "No", "Yes", [this, target](auto, bool btn2) {
                       if (!btn2) {
                         return;
                       }
                       fs::remove(getFileDownloadPath(false));
                       onDownload(target);
                     });
    return;
  }

  if (typeid(*m_anSong) == typeid(ANYTSong)) {
    if (Mod::get()->getSettingValue<bool>("use-cobalt")) {
      downloadFromCobalt();
    } else if (Mod::get()->getSettingValue<bool>("use-yt-dlp")) {
      downloadFromYtDlp();
    } else {
      Notification::create("No download method for YouTube", NotificationIcon::Error)->show();
    }
  } else if (typeid(*m_anSong) == typeid(ANHostSong)) {
    const ANHostSong *hostSong = static_cast<ANHostSong *>(m_anSong);
    const fs::path downloadPath = getFileDownloadPath(true);
    m_currentlyDownloading = true;
    Notification::create("Downloading from URL", NotificationIcon::Loading)->show();
    geode::utils::web::AsyncWebRequest()
        .get(hostSong->m_url)
        .bytes()
        .then([this, downloadPath](const geode::ByteVector &audio_data) {
          std::ofstream file(downloadPath, std::ios::out | std::ios::binary);
          file.write(reinterpret_cast<const char *>(audio_data.data()), audio_data.size());
          file.close();

          log::info("Downloaded the song successfully!");

          Loader::get()->queueInMainThread([this]() {
            Notification::create("Downloaded", NotificationIcon::Success)->show();
            setSong();
          });
          m_currentlyDownloading = false;
        })
        .expect([this](auto _) { m_currentlyDownloading = false; });
  }
}

ANSongCell *ANSongCell::create(int songId, int songJukeboxId, ANSong *song,
                               ANDropdownLayer *parentPopup, CCSize const &size,
                               bool isRobtopSong) {
  auto ret = new ANSongCell();
  if (ret && ret->init(songId, songJukeboxId, song, parentPopup, size, isRobtopSong)) {
    return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}
