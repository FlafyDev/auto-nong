#pragma once

#include "../includes/geode.hpp"
#include "../types/an_song.hpp"
#include "an_dropdown_layer.hpp"
#include "fleym.nongd/src/types/song_info.hpp"
#include "list_cell.hpp"

using namespace geode::prelude;

class ANDropdownLayer;

class ANSongCell : public JBListCell {
protected:
  int m_songId;
  int m_songJukeboxId;
  std::shared_ptr<ANSong> m_anSong;
  Ref<CustomSongWidget> m_customSongWidget;
  CCLabelBMFont *m_songNameLabel;
  CCLabelBMFont *m_artistNameLabel;
  CCLabelBMFont *m_sourceLabel;
  CCLayer *m_songInfoLayer;
  CCMenuItemSpriteExtra *m_trashButton;
  CCMenuItemSpriteExtra *m_downloadButton;
  CCProgressTimer *m_progressBar;
  CCSprite *m_progressBarBack;
  CCMenuItemToggler *m_setToggle;
  bool m_currentlyDownloading = false;
  bool m_isRobtopSong = false;
  fs::path m_songPath;

  bool init(int songId, int songJukeboxId, std::shared_ptr<ANSong> song,
            ANDropdownLayer *parentPopup, CCSize const &size, bool isRobtopSong);

public:
  ANDropdownLayer *m_parentPopup;

  void setButtonsState();
  void onDeleteSong(CCObject *);
  void onSetSong(CCObject *);
  void onDownload(CCObject *);
  void onEditSong(CCObject *);
  void onPublishSong(CCObject *);
  static ANSongCell *create(int songId, int songJukeboxId, std::shared_ptr<ANSong> song,
                            ANDropdownLayer *parentPopup, CCSize const &size, bool isRobtopSong);
};
