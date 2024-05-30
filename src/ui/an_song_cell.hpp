#pragma once

#include "../includes/geode.hpp"
#include "../types/an_song.hpp"
#include "an_dropdown_layer.hpp"
#include "list_cell.hpp"

using namespace geode::prelude;

class ANDropdownLayer;

class ANSongCell : public JBListCell {
protected:
  int m_songId;
  ANSong *m_anSong;
  ANDropdownLayer *m_parentPopup;
  unsigned int m_parentPopupUID;
  CCLabelBMFont *m_songNameLabel;
  CCLabelBMFont *m_artistNameLabel;
  CCLabelBMFont *m_sourceLabel;
  CCLayer *m_songInfoLayer;
  CCMenuItemSpriteExtra *m_trashButton;
  bool m_currentlyDownloading = false;
  bool m_isRobtopSong = false;

  bool init(int songId, ANSong *song, ANDropdownLayer *parentPopup, CCSize const &size,
            bool isRobtopSong);

public:
  void downloadFromYtDlp();
  void downloadFromCobalt();
  void setButtonsVisible();
  fs::path getFileDownloadPath(bool create);
  void setSong();
  void onDeleteSong(CCObject *);
  void onSetSong(CCObject *);
  void onDownload(CCObject *);
  static ANSongCell *create(int songId, ANSong *song, ANDropdownLayer *parentPopup,
                            CCSize const &size, bool isRobtopSong);
};
