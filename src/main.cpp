/**
 * Include the Geode headers.
 */
#include <Geode/Geode.hpp>

/**
 * Brings cocos2d and all Geode namespaces to the current scope.
 */
using namespace geode::prelude;

/**
 * `$modify` lets you extend and modify GD's classes.
 * To hook a function in Geode, simply $modify the class
 * and write a new function definition with the signature of
 * the function you want to hook.
 *
 * Here we use the overloaded `$modify` macro to set our own class name,
 * so that we can use it for button callbacks.
 *
 * Notice the header being included, you *must* include the header for
 * the class you are modifying, or you will get a compile error.
 */
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/CustomSongWidget.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditButtonBar.hpp>
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

#include <sstream>
#include "Geode/loader/Log.hpp"

//STBI_NO_JPEG
//STBI_NO_PNG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_SIMD

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb/stb_image.h"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb/stb_image_write.h"
//
// #include "geometrize/shaperesult.h"
// #include "geometrize/bitmap/bitmap.h"
// #include "geometrize/bitmap/rgba.h"
// #include "geometrize/runner/imagerunner.h"
// #include "geometrize/runner/imagerunneroptions.h"
// #include "geometrize/shape/circle.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;


// #include <fmt/format.h>
// #include <charconv>

// class $modify(MyMenuLayer, MenuLayer) {
// 	/**
// 	 * Typically classes in GD are initialized using the `init` function, (though not always!),
// 	 * so here we use it to add our own button to the bottom menu.
// 	 *
// 	 * Note that for all hooks, your signature has to *match exactly*,
// 	 * `void init()` would not place a hook!
// 	*/
// 	bool init() {
// 		/**
// 		 * We call the original init function so that the
// 		 * original class is properly initialized.
// 		 */
// 		if (!MenuLayer::init()) {
// 			return false;
// 		}
//
// 		/**
// 		 * See this page for more info about buttons
// 		 * https://docs.geode-sdk.org/tutorials/buttons
// 		*/
// 		auto myButton = CCMenuItemSpriteExtra::create(
// 			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
// 			this,
// 			/**
// 			 * Here we use the name we set earlier for our modify class.
// 			*/
// 			menu_selector(MyMenuLayer::onMyButton)
// 		);
//
// 		/**
// 		 * Here we access the `bottom-menu` node by its ID, and add our button to it.
// 		 * Node IDs are a Geode feature, see this page for more info about it:
// 		 * https://docs.geode-sdk.org/tutorials/nodetree
// 		*/
// 		auto menu = this->getChildByID("bottom-menu");
// 		menu->addChild(myButton);
//
// 		/**
// 		 * The `_spr` string literal operator just prefixes the string with
// 		 * your mod id followed by a slash. This is good practice for setting your own node ids.
// 		*/
// 		myButton->setID("my-button"_spr);
//
// 		/**
// 		 * We update the layout of the menu to ensure that our button is properly placed.
// 		 * This is yet another Geode feature, see this page for more info about it:
// 		 * https://docs.geode-sdk.org/tutorials/layouts
// 		*/
// 		menu->updateLayout();
//
// 		/**
// 		 * We return `true` to indicate that the class was properly initialized.
// 		 */
// 		return true;
// 	}
//
// 	/**
// 	 * This is the callback function for the button we created earlier.
// 	 * The signature for button callbacks must always be the same,
// 	 * return type `void` and taking a `CCObject*`.
// 	*/
// 	void onMyButton(CCObject*) {
//     wchar_t *program = Py_DecodeLocale(argv[0], NULL);
//     if (program == NULL) {
//         fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
//         exit(1);
//     }
//     Py_SetProgramName(program);
//     Py_Initialize();
//
//     auto globals = PyDict_New();
//     PyObject *o =
//         PyRun_String("'aaa'", Py_file_input, globals, globals);
//     if (o) std::cout << "Success" << std::endl;
//
//     if (Py_FinalizeEx() < 0) {
//         exit(120);
//     }
//     PyMem_RawFree(program);
// 		FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
// 	}
// };

std::string readJsonFromClipboard()
{
	if (!OpenClipboard(nullptr))
	{
		std::cout << "Failed to open clipboard." << std::endl;
		return "";
	}

  if (!IsClipboardFormatAvailable(CF_TEXT)) {
		std::cout << "Clipboard isn't text." << std::endl;
    return "";
  }

  std::string clipboardText;

  HANDLE hData = GetClipboardData(CF_TEXT);
  if (hData != nullptr) {
      char* pszText = static_cast<char*>(GlobalLock(hData));
      if (pszText != nullptr) {
          clipboardText = pszText;
          GlobalUnlock(hData);
      }
  }

  CloseClipboard();

  return clipboardText;
}

void RGBtoHSV(int r, int g, int b, float& h, float& s, float& v)
{
	auto _max = [](auto a, auto b) -> auto {
		return a > b ? a : b;
	};

	auto _min = [](auto a, auto b) -> auto {
		return a < b ? a : b;
	};

	// Normalize RGB values to the range of 0 to 1
	float r_normalized = r / 255.0;
	float g_normalized = g / 255.0;
	float b_normalized = b / 255.0;

	// Calculate value (maximum of RGB values)
	v = _max(_max(r_normalized, g_normalized), b_normalized);

	// Calculate saturation
	if (v == 0)
		s = 0;
	else
		s = (v - _min(_min(r_normalized, g_normalized), b_normalized)) / v;

	// Calculate hue
	if (s == 0)
		h = 0;
	else
	{
		float delta = v - _min(_min(r_normalized, g_normalized), b_normalized);
		if (v == r_normalized)
			h = 60 * (g_normalized - b_normalized) / delta;
		else if (v == g_normalized)
			h = 60 * (2 + (b_normalized - r_normalized) / delta);
		else if (v == b_normalized)
			h = 60 * (4 + (r_normalized - g_normalized) / delta);

		if (h < 0)
			h += 360;
	}
}


class $modify(MyEditButtonBar, EditButtonBar) {
  bool init(cocos2d::CCArray* p0, cocos2d::CCPoint p1, int p2, bool p3, int p4, int p5) {
    // "Trigger" group
    if (p2 == 12) {
      auto spr = ButtonSprite::create(".");
      //
      auto btn = CCMenuItemSpriteExtra::create(
          spr, this, menu_selector(MyEditButtonBar::onGeometrize)
      );

      p0->addObject(btn);
    }
    if (!EditButtonBar::init(p0, p1, p2, p3, p4, p5)) return false;
    return true;
  }

	void onGeometrize(CCObject*) {
    EditorUI* editorUI = GameManager::get()->getEditorLayer()->m_editorUI;
    // editorUI->pasteObjects(readJsonFromClipboard(), false);

    // std::string clipboardText2 = readJsonFromClipboard();
    // if (clipboardText2 == "a") {
    //   gd::string aa1 = "1,3621,2,30,3,30";
    //   GameManager::get()->getEditorLayer()->createObjectsFromString(aa1, false, false);
    // } else if (clipboardText2 == "b") {
    //   gd::string aa2 = "1,3621,2,0,3,0";
    //   GameManager::get()->getEditorLayer()->createObjectsFromString(aa2, false, false);
    // } else {
    //   gd::string aa3 = "1,3621,2,0,3,0,32,0.5";
    //   GameManager::get()->getEditorLayer()->createObjectsFromString(aa3, false, false);
    // }
    //
    // return;

    if (editorUI->m_selectedObject == NULL) return;

    double start_x = editorUI->m_selectedObject->m_realXPosition;
    double start_y = editorUI->m_selectedObject->m_realYPosition;

    // log::info("obj: {}", editorUI->m_selectedObject->m_objectID);

    std::string clipboardText = readJsonFromClipboard();

    json clipboardJson = json::parse(clipboardText, nullptr, false);
    if (clipboardJson.is_discarded()) return;

    std::string objects = "";
    int shapes_drawn = 0;
    cocos2d::CCArray* arr = cocos2d::CCArray::create();
    int groupId = GameManager::get()->getEditorLayer()->getNextFreeGroupID(arr);
    int transparentGroupId = 1; // TODO: Figure groups out. Group for each opacity and then automatic alpha trigger placements?

    for (auto& [key, value] : clipboardJson.items()) {
      auto obj = value.get<json>();

      float h,s,v,a;
      RGBtoHSV(obj["color"][0].template get<int>(), obj["color"][1].template get<int>(), obj["color"][2].template get<int>(), h, s, v);
      a = obj["color"][3].template get<int>();

      std::string hsv_string = fmt::format("{:.2f}a{:.2f}a{:.2f}a1a1", h, s, v);

      int type = obj["type"].template get<int>();
      switch (type) {
        case 0: // Rectangle
          int x1 = obj["data"][0].template get<int>();
          int y1 = obj["data"][1].template get<int>();
          int x2;
          int y2;
          int angle;
          if (type == 0) { // Rectangle
            x1 = obj["data"][2].template get<int>();
            y1 = obj["data"][2].template get<int>();
            angle = 0;
          } else if (type == 4) { // Rotated Ellipses
            // radius_x = obj["data"][2].template get<int>();
            // radius_y = obj["data"][3].template get<int>();
            // angle = obj["data"][4].template get<int>();
          }
          const double sprite_size = 30.0;
          double x = (x1 + x2)/2.0;
          double y = (y1 + y2)/2.0;
          double scaling_x = std::abs(x2-x1) / sprite_size;
          double scaling_y = std::abs(y2-y1) / sprite_size;
          objects += std::format(
            "1,211,2,{},3,{},128,{},129,{},41,1,42,1,43,{},44,{},25,{},21,{},20,{},6,{},57,{};",
            start_x + x, start_y - y, scaling_x, scaling_y, hsv_string, hsv_string, shapes_drawn, 1010, 0, angle, groupId
          );
          shapes_drawn++;
          break;
          
        case 5: // Circle
        case 4: // Rotated Ellipses
          int x = obj["data"][0].template get<int>();
          int y = obj["data"][1].template get<int>();
          int radius_x;
          int radius_y;
          int angle;
          if (type == 5) { // Circle
            radius_x = obj["data"][2].template get<int>();
            radius_y = obj["data"][2].template get<int>();
            angle = 0;
          } else if (type == 4) { // Rotated Ellipses
            radius_x = obj["data"][2].template get<int>();
            radius_y = obj["data"][3].template get<int>();
            angle = obj["data"][4].template get<int>();
          } else if (type == 2) { // Ellipses
            radius_x = obj["data"][2].template get<int>();
            radius_y = obj["data"][3].template get<int>();
            angle = 0;
          }

          const double sprite_size = 30.0;
          double scaling_x = radius_x / (sprite_size / 2); // Divide sprite size by 2 because it's the diameter.
          double scaling_y = radius_y / (sprite_size / 2);
          objects += std::format(
            "1,3621,2,{},3,{},128,{},129,{},41,1,42,1,43,{},44,{},25,{},21,{},20,{},6,{},57,{};",
            start_x + x, start_y - y, scaling_x, scaling_y, hsv_string, hsv_string, shapes_drawn, 1010, 0, angle, groupId
          );
          shapes_drawn++;
          break;
      }
    }

    // editorUI->pasteObjects("1,1,2,15,3,15", true);
    // gd::string aa = "1,1,2,15,3,15";
    GameManager::get()->getEditorLayer()->createObjectsFromString(objects, false, false);
    log::info("Geometrize!");
	}
};

// class $modify(EditorUI) {
// };
//
class $modify(EditorUI) {
  // cocos2d::CCArray* pasteObjects(gd::string g0, bool p1) {
  //       log::info("WHYYYYYY");
  //       log::info("hello: {}", g0.data());
  //       EditorUI::pasteObjects(g0, p1);
  //       return cocos2d::CCArray::create();
  //   }

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        // auto label = CCLabelBMFont::create("Hello world!", "bigFont.fnt");
        // label->setPosition(100, 100);
        // this->addChild(label);

        // some CCMenu*
        // m_tabsMenu->addChild(btn);
        // m_tabsMenu->addChild(btn);
       // log::info("Obj: {}", m_tabsArray->count());
       // log::info("Obj: {}", m_editButtonDict->allKeys()->count());
        // m_editButtonBar->addButton(btn, false);

        return true;
    }
};

class $modify(JBLevelInfoLayer, LevelInfoLayer) {
  bool init(GJGameLevel* level, bool p1) {
    if (!LevelInfoLayer::init(level, p1)) {
      return false;
    }
    // log::info("Level info layer initialized {}", level->m_creatorName);
    // log::info("Clipboard: {}", readJsonFromClipboard());


            // try {

    // json ex1 = json::parse();
    //                        } catch (const std::exception& e) {
    //
    // log::info("oof");
    //                        }


    return true;
  }
};

