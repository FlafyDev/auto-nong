#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/CCTextInputNode.hpp>
#include <Geode/binding/Slider.hpp>
#include <Geode/binding/SliderThumb.hpp>
#include <Geode/loader/Setting.hpp>
#include <Geode/loader/SettingNode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/ColorPickPopup.hpp>
#include <Geode/ui/InputNode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/string.hpp>
#include <Geode/loader/Mod.hpp>
#include <matjson.hpp>
#include "list_cell.hpp"
#include <vector>

using namespace geode::prelude;

struct ANClearPPBlacklistSettingStruct { int m_unused; };

class ANClearPPBlacklistSettingValue;

class ANClearPPBlacklistSettingValue : public SettingValue {
  int m_unused;
public:
    ANClearPPBlacklistSettingValue(std::string const& key, std::string const& modID, int unused)
      : SettingValue(key, modID), m_unused(unused) {}

    bool load(matjson::Value const& json) override {
      m_unused = json.as_int();
      return true;
    }

    bool save(matjson::Value& json) const override {
      json = matjson::Value(m_unused);
      return true;
    }

    SettingNode* createNode(float width) override;
};

template<>
struct SettingValueSetter<ANClearPPBlacklistSettingStruct> {
    static ANClearPPBlacklistSettingStruct get(SettingValue* setting) {
        auto posSetting = static_cast<ANClearPPBlacklistSettingValue*>(setting);
        struct ANClearPPBlacklistSettingStruct defaultStruct = { 0 };
        return defaultStruct;
    };
    static void set(ANClearPPBlacklistSettingValue* setting, ANClearPPBlacklistSettingStruct const& value) { };
};


class ANClearPPBlacklistSettingNode : public SettingNode {
protected:
    bool init(ANClearPPBlacklistSettingValue* value, float width);
    ANClearPPBlacklistSettingValue* value;
public:
    void onReset(CCObject*);
    void commit() override;
    bool hasUncommittedChanges() override;
    bool hasNonDefaultValue() override;
    void resetToDefault() override;
    static ANClearPPBlacklistSettingNode* create(ANClearPPBlacklistSettingValue* value, float width);
};

