#pragma once

#include "../includes/geode.hpp"

class ANShieldPopup : public Popup<> {
protected:
  static std::optional<int> m_cachedCount;

  EventListener<web::WebTask> m_countListener;
  TextArea *m_label = nullptr;
  CCNode *m_inputParent = nullptr;

  bool setup() override;
  void onDiscord(CCObject *);
  void setString(std::string count);

public:
  static ANShieldPopup *create() {
    auto ret = new ANShieldPopup;
    if (ret && ret->initAnchored(320.f, 192.f)) {
      ret->autorelease();
      return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
  }
};
