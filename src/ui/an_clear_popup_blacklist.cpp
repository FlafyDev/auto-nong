#include "an_clear_popup_blacklist.hpp"
#include "../includes/geode.hpp"
#include "../types/serializable_vector.hpp"

SettingNode *ANClearPPBlacklistSettingValue::createNode(float width) {
  return ANClearPPBlacklistSettingNode::create(this, width);
}

bool ANClearPPBlacklistSettingNode::init(ANClearPPBlacklistSettingValue *value, float width) {
  if (!SettingNode::init(value)) {
    return false;
  }

  this->value = value;

  float height = 40.f;
  this->setContentSize({width, height});

  auto menu = CCMenu::create();
  menu->setPosition(0, 0);
  menu->setID("inputs-menu");

  auto label = CCLabelBMFont::create("Popup blacklist", "bigFont.fnt");
  label->setAnchorPoint({0.f, 0.5f});
  label->setPosition(20.f, height / 2);
  label->setScale(0.5f);
  menu->addChild(label);

  auto resetSpr = ButtonSprite::create("Reset");
  resetSpr->setScale(0.72f);
  auto resetBtn = CCMenuItemSpriteExtra::create(
      resetSpr, this, menu_selector(ANClearPPBlacklistSettingNode::onReset));
  resetBtn->setPosition(width - 40.f - 5.f, height - 20.f);
  menu->addChild(resetBtn);

  this->addChild(menu);
  handleTouchPriority(this);

  return true;
}

void ANClearPPBlacklistSettingNode::onReset(CCObject *target) {
  Mod::get()->setSavedValue<std::vector<int>>("show-level-popup", {});
  Notification::create("Popup blacklist cleared", NotificationIcon::Success)->show();
}

void ANClearPPBlacklistSettingNode::commit() { this->dispatchCommitted(); }
bool ANClearPPBlacklistSettingNode::hasUncommittedChanges() { return false; }
bool ANClearPPBlacklistSettingNode::hasNonDefaultValue() { return false; }
void ANClearPPBlacklistSettingNode::resetToDefault() {}
ANClearPPBlacklistSettingNode *
ANClearPPBlacklistSettingNode::create(ANClearPPBlacklistSettingValue *value, float width) {
  auto ret = new ANClearPPBlacklistSettingNode();
  if (ret && ret->init(value, width)) {
    ret->autorelease();
    return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}
