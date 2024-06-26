#include "an_shield_popup.hpp"
#include "../managers/auto_nong_manager.hpp"
#include "Geode/utils/web.hpp"

std::optional<int> ANShieldPopup::m_cachedCount;

bool ANShieldPopup::setup() {
  this->setTitle("Help Required");

  auto inputParent = CCNode::create();
  m_inputParent = inputParent;
  inputParent->setContentSize({250.f, 100.f});
  inputParent->setAnchorPoint({0.5f, 1.0f});
  inputParent->setPosition(m_mainLayer->getContentSize().width / 2,
                           m_mainLayer->getContentSize().height - 40.f);

  setString("many");

  auto discordSpr = CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png");
  auto discordBtn =
      CCMenuItemSpriteExtra::create(discordSpr, this, menu_selector(ANShieldPopup::onDiscord));
  auto discordMenu = CCMenu::create();
  discordMenu->setID("discord-menu");
  discordMenu->setPosition(250.f / 2.f, 0.f);
  discordMenu->addChild(discordBtn);
  inputParent->addChild(discordMenu);

  m_mainLayer->addChild(inputParent);

  if (m_cachedCount.has_value()) {
    setString(std::to_string(m_cachedCount.value()));
  } else {
    auto req = web::WebRequest()
                   .header("User-Agent", "Auto-Nong")
                   .get("https://api.github.com/search/issues?q=repo:flafydev/"
                        "auto-nong-indexes+is:issue+is:open");
    m_countListener.bind([this](web::WebTask::Event *event) {
      auto res = event->getValue();
      if (res && res->ok()) {
        auto json = res->json();
        auto count = json->as_object()["total_count"].as_int();
        m_cachedCount = count;
        setString(std::to_string(count));
      }
    });
    m_countListener.setFilter(req);
  }

  return true;
}

void ANShieldPopup::onDiscord(CCObject *) {
  web::openLinkInBrowser("https://discord.gg/twuZ3X35yM");
}

void ANShieldPopup::setString(std::string count) {
  if (m_label) {
    m_label->removeFromParent();
  }

  // Recreating because setString behaves weirdly (didn't test with TextArea tho)
  auto text =
      TextArea::create(("Auto Nong currently has <cr>" + count +
                        "</c> NONG songs submitted by players for review.\nWe are looking for "
                        "volunteers to assist with approving/rejecting the submissions.")
                           .c_str(),
                       "chatFont.fnt", 1.f, 250.f, {0.5f, 0.5f}, 20.f, false);

  // text->setAnchorPoint({0.0f, 1.0f});
  text->setPosition({250.f / 2.f, 100.f / 2.f + 10.f});
  // text->setContentWidth(250.f);
  m_label = text;
  m_inputParent->addChild(text);
}
