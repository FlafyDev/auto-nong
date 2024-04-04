#pragma once

#include <Geode/Geode.hpp>
#include <fleym.nongd/include/jukebox.hpp>
#include <memory>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/CustomSongWidget.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/binding/SongInfoObject.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/Loader.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/IDManager.hpp>
#include <Geode/ui/Popup.hpp>
#include "../types/an_song.hpp"
#include "an_dropdown_layer.hpp"
#include "list_cell.hpp"

#include <sstream>
#include "Geode/loader/Log.hpp"

using namespace geode::prelude;

class ANDropdownLayer;

class ANSongCell : public JBListCell {
protected:
    int m_songId;
    ANSong* m_anSong;
    ANDropdownLayer* m_parentPopup;
    CCLabelBMFont* m_songNameLabel;
    CCLabelBMFont* m_artistNameLabel;
    CCLabelBMFont* m_sourceLabel;
    CCLayer* m_songInfoLayer;
    CCMenuItemSpriteExtra* m_trashButton;
    bool m_currentlyDownloading = false;

    bool init(int songId, ANSong* song, ANDropdownLayer* parentPopup, CCSize const& size);

    // virtual void FLAlert_Clicked(FLAlertLayer*, bool btn2);
  //
    void downloadFromYtDlp();
    void downloadFromCobalt();
    void setButtonsVisible();
    fs::path getFileDownloadPath(bool create);
    void setSong();
    void onDeleteSong(CCObject*);
    void onSetSong(CCObject*);
    void onDownload(CCObject*);
public:
    static ANSongCell* create(int songId, ANSong* song, ANDropdownLayer* parentPopup, CCSize const& size);
};

