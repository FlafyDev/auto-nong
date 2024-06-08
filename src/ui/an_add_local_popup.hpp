#pragma once

#include "../includes/geode.hpp"
#include "an_dropdown_layer.hpp"

class NongDropdownLayer;

// TODO: change name "indexIndex"
class ANAddLocalPopup : public Popup<ANDropdownLayer *, std::optional<std::shared_ptr<ANSong>>> {
protected:
  ANDropdownLayer *m_parentPopup;
  CCMenuItemSpriteExtra *m_infoButton;
  CCMenuItemSpriteExtra *m_deleteButton;
  CCMenuItemSpriteExtra *m_addSongButton;
  CCMenu *m_infoMenu;
  CCMenu *m_deleteMenu;
  CCMenu *m_addSongMenu;
  std::optional<std::shared_ptr<ANSong>> m_indexIndex;

  TextInput *m_songNameInput;
  TextInput *m_artistNameInput;
  TextInput *m_startOffsetInput;
  TextInput *m_youtubeLinkInput;
  TextInput *m_mp3LinkInput;

  bool setup(ANDropdownLayer *parent, std::optional<std::shared_ptr<ANSong>> indexIndex) override;

  void addSong(CCObject *);
  void showInfo(CCObject *);
  void deleteSong(CCObject *);

public:
  static ANAddLocalPopup *create(ANDropdownLayer *parent,
                                 std::optional<std::shared_ptr<ANSong>> indexIndex) {
    auto ret = new ANAddLocalPopup;
    if (ret && ret->initAnchored(320.f, 192.f, parent, indexIndex)) {
      ret->autorelease();
      return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
  }
};
