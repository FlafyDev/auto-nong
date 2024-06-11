#include "an_song_cell.hpp"
#include "../managers/auto_nong_manager.hpp"
#include "../types/an_song.hpp"
#include "../utils/utils.hpp"
#include "an_add_local_popup.hpp"
#include "fleym.nongd/include/jukebox.hpp"
#include "list_cell.hpp"

bool ANSongCell::init(int songId, int songJukeboxId, std::shared_ptr<ANSong> song,
                      ANDropdownLayer *parentPopup, CCSize const &size, bool isRobtopSong) {
  if (!JBListCell::init(size))
    return false;

  m_songId = songId;
  m_songJukeboxId = songJukeboxId;
  m_anSong = song;
  m_parentPopup = parentPopup;
  m_isRobtopSong = isRobtopSong;
  m_customSongWidget = parentPopup->m_parentWidget;
  m_songPath = AutoNongManager::get()->getSongPath(*m_anSong);

  CCSprite *spr;
  CCMenu *menu;

  auto height = this->getContentSize().height;
  auto contentSize = this->getContentSize();

  // Progress Bar
  m_progressBarBack = CCSprite::createWithSpriteFrameName("d_circle_01_001.png");
  m_progressBarBack->setColor(ccc3(50, 50, 50));
  m_progressBarBack->setPosition(ccp(295.f, height / 2));
  m_progressBarBack->setScale(0.62f);

  spr = CCSprite::createWithSpriteFrameName("d_circle_01_001.png");
  spr->setColor(ccc3(0, 255, 0));
  m_progressBar =
      CCProgressTimer::create(spr);
  m_progressBar->setType(CCProgressTimerType::kCCProgressTimerTypeRadial);
  m_progressBar->setPercentage(50.f);
  m_progressBar->setID("progress-bar");
  m_progressBar->setPosition(ccp(295.f, height / 2));
  m_progressBar->setScale(0.66f);

  this->addChild(m_progressBar);
  this->addChild(m_progressBarBack);

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

  if (song->m_index == "local") {
    // Edit button
    auto editSpr = CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png");
    editSpr->setScale(.3f);
    auto editBtn =
        CCMenuItemSpriteExtra::create(editSpr, this, menu_selector(ANSongCell::onEditSong));
    editBtn->setID("edit-btn");
    menu = CCMenu::create();
    menu->setAnchorPoint(ccp(0, 0));
    menu->setPosition(ccp(17.f, height * 0.7));
    menu->setID("button-menu3");
    menu->addChild(editBtn);
    menu->setZOrder(2);
    this->addChild(menu);

    // Publish button
    auto publishSpr = CCSprite::createWithSpriteFrameName("GJ_shareBtn_001.png");
    publishSpr->setScale(.3f);
    auto publishBtn =
        CCMenuItemSpriteExtra::create(publishSpr, this, menu_selector(ANSongCell::onPublishSong));
    publishBtn->setID("publish-btn");
    menu = CCMenu::create();
    menu->setAnchorPoint(ccp(0, 0));
    menu->setPosition(ccp(17.f, height * 0.3));
    menu->setID("button-menu3");
    menu->addChild(publishBtn);
    menu->setZOrder(2);
    this->addChild(menu);
  }

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
  m_sourceLabel->limitLabelWidth(120.f, 0.5f, 0.1f);
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
  m_songInfoLayer->setPosition(ccp(12.f + (song->m_index == "local" ? 20.f : 0.f), 1.5f));
  m_songInfoLayer->setLayout(layout);

  this->addChild(m_songInfoLayer);
  return true;
}

void ANSongCell::onSetSong(CCObject *target) {
  if (m_setToggle->isToggled()) {
    AutoNongManager::get()->unsetSong(m_songJukeboxId);
  } else {
    AutoNongManager::get()->setSong(*m_anSong, m_songJukeboxId);
  }
  // Cancel the toggling of the button that happens after the callback.
  m_setToggle->toggle(!m_setToggle->isToggled());
}

void ANSongCell::setButtonsState() {
  auto status = AutoNongManager::get()->getSongDownloadStatus(*m_anSong, m_songJukeboxId);

  if (auto *d = status.getStatus<ANSongDownloadStatus::Downloading>()) {
    m_trashButton->setVisible(false);
    m_downloadButton->setVisible(true);
    m_setToggle->setVisible(false);

    m_progressBar->setPercentage(d->progress*100.f);
    m_progressBar->setVisible(true);
    m_progressBarBack->setVisible(true);
    m_downloadButton->setColor(ccc3(105, 105, 105));
    return;
  }

  m_downloadButton->setColor(ccc3(255, 255, 255));
  m_progressBar->setVisible(false);
  m_progressBarBack->setVisible(false);

  const bool downloaded = status.isStatus<ANSongDownloadStatus::Downloaded>();
  m_trashButton->setVisible(downloaded);
  m_downloadButton->setVisible(!downloaded);
  m_setToggle->setVisible(downloaded);
  m_setToggle->toggle(status.isStatus<ANSongDownloadStatus::Active>());
}

void ANSongCell::onDeleteSong(CCObject *target) {
  AutoNongManager::get()->deleteSong(*m_anSong, m_songJukeboxId);
}

void ANSongCell::onEditSong(CCObject *target) {
  ANAddLocalPopup::create(m_parentPopup, m_anSong)->show();
}

void ANSongCell::onDownload(CCObject *target) {
  AutoNongManager::get()->downloadSong(m_anSong, m_songJukeboxId);
}

void ANSongCell::onPublishSong(CCObject *target) {
  if (auto url = AutoNongManager::get()->generatePublishUrl(*m_anSong, m_songId)) {
    CCApplication::get()->openURL(url->c_str());
  }
}

ANSongCell *ANSongCell::create(int songId, int songJukeboxId, std::shared_ptr<ANSong> song,
                               ANDropdownLayer *parentPopup, CCSize const &size,
                               bool isRobtopSong) {
  auto ret = new ANSongCell();
  if (ret && ret->init(songId, songJukeboxId, song, parentPopup, size, isRobtopSong)) {
    return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}

// void ANSongCell::setSong() {
//   try {
//     if (m_parentPopup) {
//       m_parentPopup->updateCellsButtonsState();
//     }
//
//     if (!fs::exists(m_songPath)) {
//       Notification::create("File doesn't exist", NotificationIcon::Error)->show();
//       return;
//     }
//
//     jukebox::SongInfo song = {
//         .path = m_songPath,
//         .songName = m_anSong->m_name,
//         .authorName = m_anSong->m_artist,
//         .songUrl = "local",
//         // .startOffset = m_anSong->m_startOffsetMS,
//     };
//
//     // jukebox::deleteNong(song, m_songJukeboxId, false);
//     jukebox::deleteNong(song, m_songJukeboxId);
//     jukebox::addNong(song, m_songJukeboxId);
//     jukebox::setActiveNong(song, m_songJukeboxId, m_customSongWidget);
//   } catch (const std::exception &e) {
//     log::error("Failed to set song: {}", e.what());
//     Notification::create("Failed to set song", NotificationIcon::Error)->show();
//   }
//   if (m_parentPopup) {
//     m_parentPopup->updateCellsButtonsState();
//   }
// }

// fs::path ANSongCell::getFileDownloadPath(bool create) {
//   const auto baseDir = fs::path(Loader::get()->getLoadedMod("fleym.nongd")->getSaveDir()) /
//   "nongs";
//
//   if (create) {
//     fs::create_directories(baseDir);
//   }
//
//   const std::string filename = fmt::format("autonong-{}.mp3", m_anSong->getHash());
//
//   return baseDir / filename;
// }

// void ANSongCell::downloadFromYtDlp() {
//   m_currentlyDownloading = true;
//   const ANYTSong *ytSong = static_cast<ANYTSong *>(m_anSong.get());
//   const fs::path ytDlpPath = Mod::get()->getSettingValue<std::string>("yt-dlp-path");
//   const std::string videoId = ytSong->m_ytId;
//   const fs::path downloadPath = getFileDownloadPath(true);
//
//   if (!fs::exists(ytDlpPath)) {
//     log::info("yt-dlp.exe not found at specified path: {}", ytDlpPath);
//     Notification::create("yt-dlp.exe not found at specified path",
//     NotificationIcon::Error)->show(); return;
//   }
//
//   std::string ytDlpCommand = fmt::format("\"{}\" -x --audio-format mp3 \"{}\" -o \"{}\"",
//   ytDlpPath,
//                                          videoId, downloadPath);
//
//   auto notif = Notification::create("Downloading using yt dlp", NotificationIcon::Loading, 0);
//   notif->setTime(NOTIFICATION_DEFAULT_TIME);
//   notif->show();
//
//   std::thread commandThread([this, ytSong, ytDlpCommand]() {
//     std::string command = ytDlpCommand;
//
//     int result = std::system(fmt::format("\"{}\"", command).c_str());
//
//     log::info("command({}): {}", result, command);
//
//     m_currentlyDownloading = false;
//
//     if (result != 0) {
//       log::info("Failed to execute command({}): {}", result, command);
//       Loader::get()->queueInMainThread([]() {
//         Notification::create("Failed to download from YouTube", NotificationIcon::Error)->show();
//       });
//       return;
//     }
//
//     log::info("Successfully executed command: {}", command);
//
//     Loader::get()->queueInMainThread([this]() {
//       Notification::create("Downloaded", NotificationIcon::Success)->show();
//       setSong();
//     });
//   });
//
//   commandThread.detach();
// }

// void ANSongCell::downloadFromCobalt() {
//   m_currentlyDownloading = true;
//   // const std::string apiUrl = "https://co.wuk.sh/api/json";
//   // const ANYTSong *ytSong = static_cast<ANYTSong *>(m_anSong.get());
//   // const std::string videoId = ytSong->m_ytId;
//   //
//   // std::unordered_map<std::string, std::string> parameters = {
//   //     {"url", fmt::format("https://www.youtube.com/watch?v={}", videoId)},
//   //     {"aFormat", "mp3"},
//   //     {"isAudioOnly", "true"}};
//   //
//   // matjson::Value parameters_json = matjson::Object();
//   // for (auto const &[key, value] : parameters) {
//   //   parameters_json[key] = value;
//   // }
//   // std::string body = parameters_json.dump(matjson::NO_INDENTATION);
//   //
//   // auto notif = Notification::create("Downloading using Cobalt Tools",
//   NotificationIcon::Loading,
//   // 0); notif->setTime(NOTIFICATION_DEFAULT_TIME); notif->show();
//
//   // web::WebRequest request = web::WebRequest();
//   // auto hi = request.bodyString("").get("https://example.com/").map([](web::WebResponse
//   *response)
//   // {
//   //   if (response->ok()) {
//   //     log::info("Response: {}", response->string().ok());
//   //   } else {
//   //     log::info("Failed to fetch. Code: {}", response->code());
//   //   }
//   //   return *response;
//   // });
//   // log::info("BBBBBBBB {}", hi.isFinished());
//
//   // auto req = web::WebRequest();
//   // Task<Result<std::string, std::string>, float> reqTask =
//   //     req.get("https://www.example.com/")
//   //         .map(
//   //             [](web::WebResponse *response) -> Result<std::string, std::string> {
//   //               Loader::get()->queueInMainThread([]() { log::info("test print 1"); });
//   //               if (response->ok()) {
//   //                 return response->string();
//   //               }
//   //               return Err("Failed to fetch");
//   //             },
//   //             [](web::WebProgress *progress) -> float {
//   //               Loader::get()->queueInMainThread([]() { log::info("test print 2"); });
//   //               return progress->downloadProgress().value_or(0);
//   //             });
//
//   // EventListener<Task<int, int>> *m_listener = new EventListener<Task<int, int>>();
//   // m_listener->bind([](Task<int, int>::Event *event) {
//   //   if (int *result = event->getValue()) {
//   //     log::info("Completed");
//   //   } else if (int *progress = event->getProgress()) {
//   //   } else if (event->isCancelled()) {
//   //     log::info("Task was cancelled");
//   //   }
//   // });
//   //
//   // auto req = web::WebRequest();
//   // auto task = req.get("https://www.example.com/")
//   //                 .map(
//   //                     [](web::WebResponse *response) -> int {
//   //                       if (response->ok()) {
//   //                         return 1;
//   //                       }
//   //                       return 0;
//   //                     },
//   //                     [](web::WebProgress *progress) -> int { return -1; });
//   //
//   // auto req2 = web::WebRequest();
//   // auto task2 = req2.get("https://www.example.com/")
//   //                  .map(
//   //                      [](web::WebResponse *response) -> int {
//   //                        if (response->ok()) {
//   //                          return 1;
//   //                        }
//   //                        return 0;
//   //                      },
//   //                      [](web::WebProgress *progress) -> int { return -1; });
//   //
//   // // m_listener->getPool().add()
//   //
//   // log::info("AAAAAAA {}", task.isFinished());
//   // log::info("AAAAAAA {}", task2.isFinished());
//
//   // log::info("hi: {}", hi);
//   // const MiniFunction<void(std::string const &)> then = [](std::string const &data) {
//   //   // loading = false;
//   //   // auto scene = CCDirector::sharedDirector()->getRunningScene();
//   //   // auto layer = scene->getChildren()->objectAtIndex(0);
//   //   // if (layer == nullptr)
//   //   //   return this->release();
//   //   // if (misc::getNodeName(layer) != "MoreLeaderboards")
//   //   //   return this
//   //   //       ->release(); // prevent le crash, even though the layer shouldve already been
//   //   //       destructed
//   //   // if (data == "-1" || data.length() < 2) {
//   //   //   fadeLoadingCircle();
//   //   //   geode::createQuickPopup(
//   //   //       "Error",
//   //   //       "An error occured while sending a request on <cy>our server</c>. Please try again
//   //   //       later.", "OK", nullptr, [this](auto, bool btn2) {
//   //   //         if (!btn2) {
//   //   //           g_tab = StatsListType::Diamonds;
//   //   //           keyBackClicked();
//   //   //         }
//   //   //       });
//   //   //   this->release();
//   //   //   return;
//   //   // }
//   //   // fadeLoadingCircle();
//   //   //
//   //   // handle_request_more(data);
//   //   // loading = false;
//   // };
//
//   // geode::utils::web::AsyncWebRequest();
//   //     .header("Accept", "application/json")
//   //     .header("Content-Type", "application/json")
//   //     .bodyRaw(body)
//   //     .post(apiUrl)
//   //     .text()
//   //     .then([this](const std::string &r) {
//   //       matjson::Value data = matjson::parse(r);
//   //
//   //       if (data["status"] == "stream") {
//   //         std::string audio_url = data["url"].as_string();
//   //
//   //         geode::utils::web::AsyncWebRequest()
//   //             .get(audio_url)
//   //             .bytes()
//   //             .then([this](const geode::ByteVector &audio_data) {
//   //               std::ofstream file(getFileDownloadPath(true), std::ios::out |
//   //               std::ios::binary); file.write(reinterpret_cast<const char
//   //               *>(audio_data.data()), audio_data.size()); file.close();
//   //
//   //               log::info("Downloaded the song successfully!");
//   //
//   //               Loader::get()->queueInMainThread([this]() {
//   //                 Notification::create("Downloaded", NotificationIcon::Success)->show();
//   //                 setSong();
//   //               });
//   //               m_currentlyDownloading = false;
//   //             })
//   //             .expect([this](auto _) { m_currentlyDownloading = false; });
//   //       } else {
//   //         log::warn("Failed to download the song: {}", r);
//   //         m_currentlyDownloading = false;
//   //       }
//   //     })
//   //     .expect([this](auto _) { m_currentlyDownloading = false; });
// }

// if (m_currentlyDownloading) {
//   FLAlertLayer::create(
//       "Currently downloading",
//       "Please wait for the current download to finish before downloading the same song again.",
//       "OK")
//       ->show();
//   return;
// }
//
// if (fs::exists(getFileDownloadPath(false))) {
//   createQuickPopup("Song already downloaded", "Are you sure you want to redownload the song?",
//                    "No", "Yes", [this, target](auto, bool btn2) {
//                      if (!btn2) {
//                        return;
//                      }
//                      fs::remove(getFileDownloadPath(false));
//                      onDownload(target);
//                    });
//   return;
// }
//
// if (m_anSong->getSource() == "youtube") {
//   if (Mod::get()->getSettingValue<bool>("use-cobalt")) {
//     downloadFromCobalt();
//   } else if (Mod::get()->getSettingValue<bool>("use-yt-dlp")) {
//     downloadFromYtDlp();
//   } else {
//     Notification::create("No download method for YouTube", NotificationIcon::Error)->show();
//   }
// } else if (m_anSong->getSource() == "host") {
//   const ANHostSong *hostSong = static_cast<ANHostSong *>(m_anSong.get());
//   const fs::path downloadPath = getFileDownloadPath(true);
//   m_currentlyDownloading = true;
//   Notification::create("Downloading from URL", NotificationIcon::Loading)->show();
//   // geode::utils::web::AsyncWebRequest()
//   //     .get(hostSong->m_url)
//   //     .bytes()
//   //     .then([this, downloadPath](const geode::ByteVector &audio_data) {
//   //       std::ofstream file(downloadPath, std::ios::out | std::ios::binary);
//   //       file.write(reinterpret_cast<const char *>(audio_data.data()), audio_data.size());
//   //       file.close();
//   //
//   //       log::info("Downloaded the song successfully!");
//   //
//   //       Loader::get()->queueInMainThread([this]() {
//   //         Notification::create("Downloaded", NotificationIcon::Success)->show();
//   //         setSong();
//   //       });
//   //       m_currentlyDownloading = false;
//   //     })
//   //     .expect([this](auto _) { m_currentlyDownloading = false; });
// }
