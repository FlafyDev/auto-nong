#pragma once

#include "../types/an_song.hpp"
#include "an_song_cell.hpp"

class ANSongCell;

class ANDropdownLayer
    : public Popup<int, std::vector<std::shared_ptr<ANSong>>, CustomSongWidget *, int, int, bool> {
protected:
  int m_songID;
  std::vector<std::shared_ptr<ANSong>> m_songCandidates;
  int m_popupNumber;
  int m_totalPopups;
  bool m_isRobtopSong;
  ListView *m_listView = nullptr;
  CCSize m_cellSize = {320.f, 60.f};

  bool setup(int songId, std::vector<std::shared_ptr<ANSong>> songCandidates,
             CustomSongWidget *parent, int popupNumber, int totalPopups,
             bool isRoptopSong) override;

  void onClose(CCObject *) override;

public:
  Ref<CustomSongWidget> m_parentWidget;
  std::vector<ANSongCell *> m_songCells;

  static ANDropdownLayer *create(int songId, std::vector<std::shared_ptr<ANSong>> songCandidates,
                                 CustomSongWidget *parent, int popupNumber, int totalPopups,
                                 bool isRobtopSong) {
    auto ret = new ANDropdownLayer;
    if (ret && ret->initAnchored(420.f, 280.f, songId, songCandidates, parent, popupNumber,
                                 totalPopups, isRobtopSong, "GJ_square02.png")) {
      ret->autorelease();
      return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
  }

  void onAddNong(CCObject *);
  void onOpenOptions(CCObject *);
  void updateCellsButtonsState();
};
