/**
 * Include the Geode headers.
 */
#include <Geode/Geode.hpp>
#include <fleym.nongd/include/jukebox.hpp>
#include <matjson.hpp>
#include <zlib/zlib.h>

/**
 * Brings cocos2d and all Geode namespaces to the current scope.
 */
using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/CustomSongWidget.hpp>
#include <Geode/loader/SettingEvent.hpp>
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
#include <Geode/modify/GameManager.hpp>
#include <Geode/ui/Popup.hpp>
#include "ui/an_dropdown_layer.hpp"
#include "types/an_song.hpp"
#include "utils/utils.hpp"

#include <sstream>
#include "Geode/loader/Log.hpp"
#include "types/serializable_vector.hpp"
#include "ui/an_multi_string_setting.hpp"
#include "ui/an_clear_popup_blacklist.hpp"
#include <ranges>

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

// #include <nlohmann/json.hpp>
// using json = nlohmann::json;


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

// std::string readTextFromClipboard()
// {
// 	if (!OpenClipboard(nullptr))
// 	{
// 		std::cout << "Failed to open clipboard." << std::endl;
// 		return "";
// 	}
//
//   if (!IsClipboardFormatAvailable(CF_TEXT)) {
// 		std::cout << "Clipboard isn't text." << std::endl;
//     return "";
//   }
//
//   std::string clipboardText;
//
//   HANDLE hData = GetClipboardData(CF_TEXT);
//   if (hData != nullptr) {
//       char* pszText = static_cast<char*>(GlobalLock(hData));
//       if (pszText != nullptr) {
//           clipboardText = pszText;
//           GlobalUnlock(hData);
//       }
//   }
//
//   CloseClipboard();
//
//   return clipboardText;
// }
//
// void RGBtoHSV(int r, int g, int b, float& h, float& s, float& v)
// {
// 	auto _max = [](auto a, auto b) -> auto {
// 		return a > b ? a : b;
// 	};
//
// 	auto _min = [](auto a, auto b) -> auto {
// 		return a < b ? a : b;
// 	};
//
// 	// Normalize RGB values to the range of 0 to 1
// 	float r_normalized = r / 255.0;
// 	float g_normalized = g / 255.0;
// 	float b_normalized = b / 255.0;
//
// 	// Calculate value (maximum of RGB values)
// 	v = _max(_max(r_normalized, g_normalized), b_normalized);
//
// 	// Calculate saturation
// 	if (v == 0)
// 		s = 0;
// 	else
// 		s = (v - _min(_min(r_normalized, g_normalized), b_normalized)) / v;
//
//   if (s >= 0.98) s = 0.98;
//
// 	// Calculate hue
// 	if (s == 0)
// 		h = 0;
// 	else
// 	{
// 		float delta = v - _min(_min(r_normalized, g_normalized), b_normalized);
// 		if (v == r_normalized)
// 			h = 60 * (g_normalized - b_normalized) / delta;
// 		else if (v == g_normalized)
// 			h = 60 * (2 + (b_normalized - r_normalized) / delta);
// 		else if (v == b_normalized)
// 			h = 60 * (4 + (r_normalized - g_normalized) / delta);
//
// 		if (h < 0)
// 			h += 360;
// 	}
// }
//
//
// class $modify(MyEditButtonBar, EditButtonBar) {
//   bool init(cocos2d::CCArray* p0, cocos2d::CCPoint p1, int p2, bool p3, int p4, int p5) {
//     // "Trigger" group
//     if (p2 == 12) {
//       auto spr = ButtonSprite::create("G");
//       //
//       auto btn = CCMenuItemSpriteExtra::create(
//           spr, this, menu_selector(MyEditButtonBar::onGeometrize)
//       );
//
//       p0->addObject(btn);
//
//       auto spr2 = ButtonSprite::create("M");
//       //
//       auto btn2 = CCMenuItemSpriteExtra::create(
//           spr2, this, menu_selector(MyEditButtonBar::onMusicTrigger)
//       );
//
//       p0->addObject(btn2);
//     }
//     if (!EditButtonBar::init(p0, p1, p2, p3, p4, p5)) return false;
//     return true;
//   }
//
//   void onMusicTrigger(CCObject*) {
//     EditorUI* editorUI = GameManager::get()->getEditorLayer()->m_editorUI;
//
//     std::string clipboardText = readTextFromClipboard();
//
//     if (clipboardText.find("Music: ") != 0 ||
//       clipboardText.find(" ms on (") == std::string::npos ||
//       clipboardText.find(",") == std::string::npos ||
//       clipboardText.find(")") == std::string::npos) {
//       return;
//     }
//
//     int ms;
//     double x, y;
//
//     // Create a stringstream object initialized with the input string
//     std::stringstream ss(clipboardText);
//
//     // Extracting the required numbers
//     std::string temp;
//     std::getline(ss, temp, ':'); // Skip "Music: "
//     ss >> ms; // Extract milliseconds
//     std::getline(ss, temp, '('); // Skip " ms on ("
//     ss >> x; // Extract first number
//     std::getline(ss, temp, ','); // Skip ","
//     ss >> y; // Extract second number
//     
//     log::info("{}, {}, {}", ms, x, y);
//
//     GameManager::get()->getEditorLayer()->createObjectsFromString(std::format("1,1934,408,{},32,0.25,2,{},3,{},11,1,406,1.0", ms, x, y), false, false);
//   }
//
// 	void onGeometrize(CCObject*) {
//     EditorUI* editorUI = GameManager::get()->getEditorLayer()->m_editorUI;
//     // log::info("obj: {}", editorUI->m_selectedObject->m_objectID);
//     // editorUI->pasteObjects(readJsonFromClipboard(), false);
//
//     // std::string clipboardText2 = readJsonFromClipboard();
//     // if (clipboardText2 == "a") {
//     //   gd::string aa1 = "1,3621,2,30,3,30";
//     //   GameManager::get()->getEditorLayer()->createObjectsFromString(aa1, false, false);
//     // } else if (clipboardText2 == "b") {
//     //   gd::string aa2 = "1,3621,2,0,3,0";
//     //   GameManager::get()->getEditorLayer()->createObjectsFromString(aa2, false, false);
//     // } else {
//     //   gd::string aa3 = "1,3621,2,0,3,0,32,0.5";
//     //   GameManager::get()->getEditorLayer()->createObjectsFromString(aa3, false, false);
//     // }
//     //
//     // return;
//
//     if (editorUI->m_selectedObject == NULL) return;
//
//     double start_x = editorUI->m_selectedObject->m_realXPosition;
//     double start_y = editorUI->m_selectedObject->m_realYPosition;
//     double start_scale = 0.5;
//
//     std::string clipboardText = readTextFromClipboard();
//
//     json clipboardJson = json::parse(clipboardText, nullptr, false);
//     if (clipboardJson.is_discarded()) return;
//
//     std::string objects = "";
//     int shapes_drawn = 0;
//     cocos2d::CCArray* arr = cocos2d::CCArray::create();
//     int groupId = GameManager::get()->getEditorLayer()->getNextFreeGroupID(arr);
//     int transparentGroupId = 100; // TODO: Figure groups out. Group for each opacity and then automatic alpha trigger placements?
//
//     for (auto& [key, value] : clipboardJson.items()) {
//       auto obj = value.get<json>();
//
//       float h,s,v,a;
//       RGBtoHSV(obj["color"][0].template get<int>(), obj["color"][1].template get<int>(), obj["color"][2].template get<int>(), h, s, v);
//       a = obj["color"][3].template get<int>();
//
//       std::string hsv_string = fmt::format("{:.2f}a{:.2f}a{:.2f}a1a1", h, s, v);
//
//       int type = obj["type"].template get<int>();
//       if (type == 0 || type == 1) {
//         int x1 = obj["data"][0].template get<int>();
//         int y1 = obj["data"][1].template get<int>();
//         int x2;
//         int y2;
//         int angle;
//         if (type == 0) { // Rectangle
//           x2 = obj["data"][2].template get<int>();
//           y2 = obj["data"][3].template get<int>();
//           angle = 0;
//         } else if (type == 1) { // Rotated Rectangle
//           x2 = obj["data"][2].template get<int>();
//           y2 = obj["data"][3].template get<int>();
//           angle = obj["data"][4].template get<int>();
//         }
//         const double spriteSize = 30.0;
//         double x = (x1 + x2)/2.0;
//         double y = (y1 + y2)/2.0;
//         double scalingX = std::abs(x2-x1) / spriteSize;
//         double scalingY = std::abs(y2-y1) / spriteSize;
//
//         std::string groups = std::format("{}", groupId);
//         if (a == 128) {
//           groups += std::format(".{}", transparentGroupId);
//         }
//
//         objects += std::format(
//           "1,211,2,{},3,{},128,{},129,{},41,1,42,1,43,{},44,{},25,{},21,{},20,{},6,{},57,{};",
//           start_x + x * start_scale, start_y - y * start_scale, scalingX * start_scale, scalingY * start_scale, hsv_string, hsv_string, shapes_drawn, 1010, 0, angle, groups
//         );
//         shapes_drawn++;
//       } else if (type == 4 || type == 5) {
//         int x = obj["data"][0].template get<int>();
//         int y = obj["data"][1].template get<int>();
//         int radius_x;
//         int radius_y;
//         int angle;
//         if (type == 5) { // Circle
//           radius_x = obj["data"][2].template get<int>();
//           radius_y = obj["data"][2].template get<int>();
//           angle = 0;
//         } else if (type == 4) { // Rotated Ellipses
//           radius_x = obj["data"][2].template get<int>();
//           radius_y = obj["data"][3].template get<int>();
//           angle = obj["data"][4].template get<int>();
//         } else if (type == 2) { // Ellipses
//           radius_x = obj["data"][2].template get<int>();
//           radius_y = obj["data"][3].template get<int>();
//           angle = 0;
//         }
//
//         std::string groups = std::format("{}", groupId);
//         if (a == 128) {
//           groups += std::format(".{}", transparentGroupId);
//         }
//
//         const double sprite_size = 30.0;
//         double scaling_x = radius_x / (sprite_size / 2); // Divide sprite size by 2 because it's the diameter.
//         double scaling_y = radius_y / (sprite_size / 2);
//         objects += std::format(
//           "1,3621,2,{},3,{},128,{},129,{},41,1,42,1,43,{},44,{},25,{},21,{},20,{},6,{},57,{};",
//           start_x + x * start_scale, start_y - y * start_scale, scaling_x * start_scale, scaling_y * start_scale, hsv_string, hsv_string, shapes_drawn, 1010, 0, angle, groups
//         );
//         shapes_drawn++;
//       }
//     }
//
//     // editorUI->pasteObjects("1,1,2,15,3,15", true);
//     // gd::string aa = "1,1,2,15,3,15";
//     GameManager::get()->getEditorLayer()->createObjectsFromString(objects, false, false);
//     log::info("Geometrize!");
// 	}
// };
//
// // class $modify(EditorUI) {
// // };
// //
// class $modify(EditorUI) {
//   // cocos2d::CCArray* pasteObjects(gd::string g0, bool p1) {
//   //       log::info("WHYYYYYY");
//   //       log::info("hello: {}", g0.data());
//   //       EditorUI::pasteObjects(g0, p1);
//   //       return cocos2d::CCArray::create();
//   //   }
//
//     bool init(LevelEditorLayer* editorLayer) {
//         if (!EditorUI::init(editorLayer)) return false;
//
//         // auto label = CCLabelBMFont::create("Hello world!", "bigFont.fnt");
//         // label->setPosition(100, 100);
//         // this->addChild(label);
//
//         // some CCMenu*
//         // m_tabsMenu->addChild(btn);
//         // m_tabsMenu->addChild(btn);
//        // log::info("Obj: {}", m_tabsArray->count());
//        // log::info("Obj: {}", m_editButtonDict->allKeys()->count());
//         // m_editButtonBar->addButton(btn, false);
//
//         return true;
//     }
// };
//
//
// // cocos2d::CCPoint PLAYER_POSITION;
// int SONG_LAST_MS = 0;
//
// class $modify(PlayerObject) {
//   void loadFromCheckpoint(PlayerCheckpoint* p0) {
//     PlayerObject::loadFromCheckpoint(p0);
//     cocos2d::CCPoint pos = PlayerObject::getRealPosition();
//     log::info("Music: {} ms on ({}, {})", SONG_LAST_MS, pos.x, pos.y-90);
//   }
// };
//
// class $modify(FMODAudioEngine) {
//   void setMusicTimeMS(unsigned int p0, bool p1, int p2) {
//     // log::info("Music: {} ms on ({}, {})", p0, PLAYER_POSITION.x, PLAYER_POSITION.y);
//     SONG_LAST_MS = p0;
//     FMODAudioEngine::setMusicTimeMS(p0, p1, p2);
//   }
// };
//
//
// // class $modify(JBLevelInfoLayer, LevelInfoLayer) {
// //   bool init(GJGameLevel* level, bool p1) {
// //     if (!LevelInfoLayer::init(level, p1)) {
// //       return false;
// //     }
// //     // log::info("Level info layer initialized {}", level->m_creatorName);
// //     // log::info("Clipboard: {}", readJsonFromClipboard());
// //
// //
// //             // try {
// //
// //     // json ex1 = json::parse();
// //     //                        } catch (const std::exception& e) {
// //     //
// //     // log::info("oof");
// //     //                        }
// //
// //
// //     return true;
// //   }
// // };
//
// // std::string decompress_xz_to_string(const char* compressed_data, size_t compressed_size) {
// // 	lzma_stream strm = LZMA_STREAM_INIT;
// // 	lzma_ret ret = lzma_stream_decoder(
// // 			&strm, UINT64_MAX, LZMA_CONCATENATED);
// //
// // 	// Return successfully if the initialization went fine.
// // 	if (ret == LZMA_OK)
// // 		return "";
// //
// //   return "";
// // }
//
// // std::string decompressBytes(const std::vector<uint8_t>& data) {
// //     // Prepare output string
// //     std::string output;
// //
// //     // Prepare zlib stream
// //     z_stream stream;
// //     stream.zalloc = Z_NULL;
// //     stream.zfree = Z_NULL;
// //     stream.opaque = Z_NULL;
// //     stream.avail_in = data.size();
// //     stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(data.data()));
// //
// //     // Initialize inflate
// //     auto aa = inflateInit(&stream);
// //     if (aa != Z_OK) {
// //         std::cout << Z_OK << " " << Z_MEM_ERROR << " " << Z_BUF_ERROR << " " << std::endl;
// //         std::cout << ":D " << aa << std::endl;
// //         return output; // Return empty string on failure
// //     }
// //
// //     // Decompression loop
// //     while (stream.avail_in > 0) {
// //         // Ensure output buffer is large enough
// //         char buffer[4096]; // Adjust the buffer size as needed
// //         stream.avail_out = sizeof(buffer);
// //         stream.next_out = reinterpret_cast<Bytef*>(buffer);
// //
// //         // Decompress data
// //         int ret = inflate(&stream, Z_NO_FLUSH);
// //         if (ret != Z_OK && ret != Z_STREAM_END) {
// //             inflateEnd(&stream);
// //             return output; // Return empty string on failure
// //         }
// //
// //         // Append decompressed data to output string
// //         output.append(buffer, sizeof(buffer) - stream.avail_out);
// //     }
// //
// //     // Finalize decompression
// //     inflateEnd(&stream);
// //
// //     return output;
// // }
// // std::vector<uint8_t> compress(const std::string& str, int compressionlevel = Z_BEST_COMPRESSION) {
// //     z_stream zs;                        // z_stream is zlib's control structure
// //     memset(&zs, 0, sizeof(zs));
// //
// //     if (deflateInit(&zs, compressionlevel) != Z_OK)
// //         throw(std::runtime_error("deflateInit failed while compressing."));
// //
// //     zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(str.data()));
// //     zs.avail_in = str.size();           // set the z_stream's input
// //
// //     int ret;
// //     std::vector<uint8_t> outbuffer(10240); // Use a vector to store the compressed data
// //     std::vector<uint8_t> outdata; // Vector to accumulate compressed data
// //
// //     // retrieve the compressed bytes blockwise
// //     do {
// //         zs.next_out = outbuffer.data();
// //         zs.avail_out = outbuffer.size();
// //
// //         ret = deflate(&zs, Z_FINISH);
// //
// //         // append the block to the output data vector
// //         outdata.insert(outdata.end(), outbuffer.begin(), outbuffer.begin() + (outbuffer.size() - zs.avail_out));
// //     } while (ret == Z_OK);
// //
// //     deflateEnd(&zs);
// //
// //     if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
// //         std::ostringstream oss;
// //         oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
// //         throw(std::runtime_error(oss.str()));
// //     }
// //
// //     return outdata;
// // }

std::string decompressGz(const std::vector<uint8_t>& compressed_data) {
    // Prepare output string
    std::string uncompressed_data;

    // Setup zlib stream
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = compressed_data.size();
    stream.next_in = const_cast<uint8_t*>(compressed_data.data());

    // Use zlib's inflateInit2 to specify gzip/zlib decoding
    if (inflateInit2(&stream, 16 + MAX_WBITS) != Z_OK) {
        return uncompressed_data; // Error handling
    }

    // Inflate compressed data
    constexpr size_t chunk_size = 4096; // Adjust as needed
    uint8_t out[chunk_size];
    int ret;
    do {
        stream.avail_out = chunk_size;
        stream.next_out = out;
        ret = inflate(&stream, Z_NO_FLUSH);
        switch (ret) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&stream);
                return uncompressed_data; // Error handling
        }
        size_t have = chunk_size - stream.avail_out;
        uncompressed_data.append(reinterpret_cast<char*>(out), have);
    } while (ret != Z_STREAM_END);

    // Clean up and return uncompressed data
    (void)inflateEnd(&stream);
    return uncompressed_data;
}


std::map<int, std::vector<std::shared_ptr<ANSong>>> songIDToSongs;

std::vector<std::shared_ptr<ANSong>> findSongs(int songID) {
    if (songIDToSongs.find(songID) == songIDToSongs.end()) {
        return std::vector<std::shared_ptr<ANSong>>{};
    }
    return songIDToSongs[songID];
}

bool anySongExists(std::set<int> songIDs) {
  for (int songID : songIDs) {
    if (songIDToSongs.find(songID) != songIDToSongs.end()) return true;
  }

  return false;
}


// class NongCell : public JBListCell {
// protected:
//     SongInfo m_songInfo;
//     CCLabelBMFont* m_songNameLabel;
//     CCLabelBMFont* m_authorNameLabel;
//     CCLabelBMFont* m_levelNameLabel = nullptr;
//     CCLayer* m_songInfoLayer;
//
//     NongDropdownLayer* m_parentPopup;
//
//     bool m_isDefault;
//     bool m_isActive;
//
//     bool init(SongInfo info, NongDropdownLayer* parentPopup, CCSize const& size, bool selected, bool isDefault);
//
//     virtual void FLAlert_Clicked(FLAlertLayer*, bool btn2);
// public:
//     static NongCell* create(SongInfo info, NongDropdownLayer* parentPopup, CCSize const& size, bool selected, bool isDefault);
//     void onSet(CCObject*);
//     void deleteSong(CCObject*);
//     void onFixDefault(CCObject*);
// };



int current_level_id = 0;
class $modify(MyCustomSongWidget, CustomSongWidget) {
    // NongData nongs;
    // CCMenu* menu;
    // CCMenuItemSpriteExtra* songNameLabel;
    // CCLabelBMFont* sizeIdLabel;
    // std::string songIds = "";
    // std::string sfxIds = "";
    // bool fetchedAssetInfo = false;
    // bool firstRun = true;
    // bool searching = false;
    // std::unordered_map<int, NongData> assetNongData;
  //
  //j


    // void updateWithMultiAssets(gd::string p1, gd::string p2, int p3) {
    //     CustomSongWidget::updateWithMultiAssets(p1, p2, p3);
    //     // m_fields->songIds = std::string(p1);
    //     // m_fields->sfxIds = std::string(p2);
    //     // if (m_fields->fetchedAssetInfo) {
    //     //     this->fixMultiAssetSize();
    //     // }
    //     // if (m_isRobtopSong) {
    //     //     return;
    //     // }
    //     // this->createSongLabels();
    // }

    bool showNong;
    CCMenu* nongMenu;

    std::set<int> getSongIds() {
      auto kv = std::views::keys(m_songs);
      std::set<int> songIds{ kv.begin(), kv.end() };
      songIds.insert(m_songInfoObject->m_songID);
      return songIds;
    }

    void updateSongInfo() {
      CustomSongWidget::updateSongInfo();
      if (!m_fields->showNong) {
        m_fields->showNong = GameManager::get()->m_levelEditorLayer == nullptr && anySongExists(getSongIds());
          
        if (m_fields->showNong) {
          this->scheduleOnce(schedule_selector(MyCustomSongWidget::showPopup), 0.2);
          auto spr = CCSprite::create("logo.png"_spr);
          spr->setScale(0.225f);

          auto btn = CCMenuItemSpriteExtra::create(
              spr, this, menu_selector(MyCustomSongWidget::onNongBtn)
          );

          m_fields->nongMenu = CCMenu::create();
          m_fields->nongMenu->addChild(btn);
          this->addChild(m_fields->nongMenu);
        }
      }
      if (m_fields->nongMenu != nullptr) {
        // Different position if info button is visible...
        if (m_songs.size() > 1 || m_sfx.size() > 1) {
          m_fields->nongMenu->setPosition(ccp(116.f, 30.f));
        } else {
          m_fields->nongMenu->setPosition(ccp(135.f, 28.f));
        }
      }
    }

    void showPopup(float) {
      if (current_level_id == 0) return;
      if (!Mod::get()->getSettingValue<bool>("show-level-popup")) return;

      std::vector<int> blacklistedLevels = Mod::get()->getSavedValue<std::vector<int>>("show-level-popup");
      if(std::find(blacklistedLevels.begin(), blacklistedLevels.end(), current_level_id) != blacklistedLevels.end()) return;
      blacklistedLevels.push_back(current_level_id);
      Mod::get()->setSavedValue<std::vector<int>>("show-level-popup", blacklistedLevels);

      auto popup = createQuickPopup("This level has a NONG song", 
        "A NONG song has been found for this level.\nDownload it?", 
        "No", 
        "Yes",
        [this](auto, bool btn2) {
          if (!btn2) {
            return;
          }
          onNongBtn(nullptr);
          // fs::remove(getFileDownloadPath(false));
          // onDownload(target);
        },
        false
      );
      popup->m_scene = this->getParent();
      popup->show();
    }


    void onNongBtn(CCObject*) {
      log::info("Nong button clicked {}", current_level_id);
      for (int songId : getSongIds()) {
        log::info("song: {}", songId);
        auto anSongs = findSongs(songId);
        if (!anSongs.empty()) {
          auto layer = ANDropdownLayer::create(songId, findSongs(songId), this, 1, 1);
          layer->m_noElasticity = true;
          // based robtroll
          layer->setZOrder(106);
          layer->show();
        }
      }
      // auto songs = findSongs(currentLevelID, m_songInfoObject->m_songID);
      // for (const auto& song : songs) {
      //   song->download();
      // }
      // ANSong* autoNongSong = songs.front().get();
      // SongInfo song = {
      //     .path = "Z:\\home\\flafy\\Downloads\\itsnotlikeilikeyou.mp3",
      //     .songName = autoNongSong->name,
      //     .authorName = autoNongSong->artist,
      //     .songUrl = "local",
      // };
      // jukebox::addNong(song, 945695);
      // jukebox::setActiveSong(song, 945695);
    }
};

class $modify(ANLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool p1) {
        if (!LevelInfoLayer::init(level, p1)) {
            return false;
        }
        current_level_id = level->m_levelID;
        return true;
    }
};



void loadANIndexes() {
  // std::vector<std::string> indexes = Mod::get()->getSettingValue<std::vector<std::string>>("indexes");

  // auto aa = Mod::get()->getSetting("indexes");
  // log::info("1");
  // auto aa2 = static_cast<MultiStringSettingValue*>(Mod::get()->getSetting("indexes"));
  // auto aa3 = static_cast<MultiStringSettingValue*>(Mod::get()->getSetting("indexes"))->getStrings();
  std::vector<std::string> indexes = Mod::get()->getSettingValue<MultiStringSettingStruct>("indexes").m_strings;
  //
  // std::vector<std::string> indexes = as<MultiStringSettingValue*>(Mod::get()->getSetting("indexes"))->getStrings();
  // std::vector<std::string> indexes = {};
  
  // auto indexes = as<MultiStringSettingValue*>(Mod::get()->getSetting("indexes"))->getStrings();

  // std::vector<std::string> indexes = Mod::get()->getSettingValue<CRTypeStruct>("indexes").strings;
  songIDToSongs = {};

  for (std::string index : indexes) {
    if (index.size() < 5) continue;
    geode::utils::web::AsyncWebRequest()
    .fetch(index)
    .bytes()
    .then([index](const geode::ByteVector& r) {
      std::string jsonString = decompressGz(static_cast<std::vector<uint8_t>>(r));

      matjson::Value jsonObj = matjson::parse(jsonString);

      for (const auto& songData : jsonObj.as_array()) {
        std::string name = songData["name"].as_string();
        std::string artist = songData.contains("artist") ? songData["artist"].as_string() : "";
        std::string source = songData["source"].as_string();

        std::shared_ptr<ANSong> song;
        if (source == "youtube") {
            std::string yt_id = songData["yt-id"].as_string();
            song = std::make_shared<ANYTSong>(name, artist, index, yt_id);
        } else if (source == "host") {
            std::string url = songData["url"].as_string();
            song = std::make_shared<ANHostSong>(name, artist, index, url);
        } else {
            std::cout << "Unsupported source: " << source << std::endl;
            continue;
        }

        if (songData.contains("songs")) {
            for (matjson::Value songID : songData["songs"].as_array()) {
                songIDToSongs[songID.as_int()].push_back(song);
            }
        }
      }

      // auto end = std::chrono::high_resolution_clock::now();
      // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
      //
      // std::cout << "Time taken: " << duration << " microseconds" << std::endl;

      //
      // // Print songs by song ID
      // for (const auto& pair : songIDToSongs) {
      //     std::cout << "Song ID " << pair.first << ":" << std::endl;
      //     for (const auto& song : pair.second) {
      //         song->download();
      //     }
      // }
    });
  }
}

// Every hour reload the indexes
class $modify(ANGameManager, GameManager) {
  void loadANIndexesLoop(float) {
    loadANIndexes();
  }
};

$on_mod(Loaded) {
  Mod::get()->addCustomSetting<MultiStringSettingValue>("indexes", MultiStringSettingValue::defaultIndexes);
  Mod::get()->addCustomSetting<ANClearPPBlacklistSettingValue>("_blacklistPPClear", 0);

  loadANIndexes();
  GameManager::get()->schedule(schedule_selector(ANGameManager::loadANIndexesLoop), 60*60, kCCRepeatForever, 0);
  listenForSettingChanges("indexes", +[](MultiStringSettingStruct value) {
    loadANIndexes();
  });
    
    // or, alternatively:
    // Mod::get()->registerCustomSetting(
    //     "my-setting",
    //     std::make_unique<MySettingValue>("my-setting", Mod::get()->getID(), ...)
    // );
}

// $execute {
// }


// $execute {
//   log::info("on execute!!!");
//
//   loadANIndexes(0);
//   this->scheduleOnce(schedule_selector(loadANIndexes), 1);
//
//     // geode::utils::web::AsyncWebRequest()
//     // .fetch(std::string("https://cdn.jsdelivr.net/gh/FlafyDev/auto-nong-indexes@dist/example.json.gz"))
//     // .bytes()
//     // .then([](const geode::ByteVector& r) {
//     //   std::string a = decompressGz(static_cast<std::vector<uint8_t>>(r));
//     //   std::cout << "data: " << a << std::endl;
//     //
//     //   json aJson = json::parse(a, nullptr, false);
//     //
//     //   // for (int i = 0; i < 100000; ++i) {
//     //   //     std::string name = "Song " + std::to_string(i + 3); // Start from 3 to avoid duplicates
//     //   //     std::string artist = "Artist " + std::to_string(i + 1); // Start from 1
//     //   //     std::string source = "youtube"; // Alternating sources
//     //   //     std::string yt_id = "yt-id-" + std::to_string(i + 1);
//     //   //
//     //   //     // Add generated song to JSON object
//     //   //     json newSong = {
//     //   //         {"name", name},
//     //   //         {"artist", artist},
//     //   //         {"source", source},
//     //   //         {"yt-id", yt_id}
//     //   //     };
//     //   //
//     //   //     // Add levels array
//     //   //     std::vector<int> levels;
//     //   //     for (int j = 1; j <= 5; ++j) {
//     //   //         if (i % j == 0) {
//     //   //             levels.push_back(j);
//     //   //         }
//     //   //     }
//     //   //     newSong["levels"] = levels;
//     //   //
//     //   //     // Add song to JSON array
//     //   //     aJson.push_back(newSong);
//     //   // }
//     //   // auto start = std::chrono::high_resolution_clock::now();
//     //
//     //   // for (const auto& songData : aJson) {
//     //   //     std::string name = songData["name"];
//     //   //     std::string artist = songData.value("artist", "");
//     //   //     std::string source = songData["source"];
//     //   //
//     //   //     std::shared_ptr<ANSong> song;
//     //   //     if (source == "youtube") {
//     //   //         std::string yt_id = songData["yt-id"];
//     //   //         std::optional<int> yt_offset;
//     //   //         // Not supported yet
//     //   //         // if (songData.contains("yt-offset-ms"))
//     //   //         //   yt_offset = songData["yt-offset-ms"].template get<int>();
//     //   //         // else
//     //   //         yt_offset = {};
//     //   //         artist = "idk";
//     //   //         song = std::make_shared<ANYTSong>(name, artist, yt_id, yt_offset);
//     //   //     } else {
//     //   //         // Handle other sources or create different types of songs
//     //   //         std::cout << "Unsupported source: " << source << std::endl;
//     //   //         continue;
//     //   //     }
//     //   //
//     //   //     if (songData.contains("songs"))
//     //   //       for (int songID : songData["songs"]) {
//     //   //           songIDToSongs[songID].push_back(song);
//     //   //       }
//     //   // }
//     //
//     //   // auto end = std::chrono::high_resolution_clock::now();
//     //   // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
//     //   //
//     //   // std::cout << "Time taken: " << duration << " microseconds" << std::endl;
//     //
//     //   //
//     //   // // Print songs by song ID
//     //   // for (const auto& pair : songIDToSongs) {
//     //   //     std::cout << "Song ID " << pair.first << ":" << std::endl;
//     //   //     for (const auto& song : pair.second) {
//     //   //         song->download();
//     //   //     }
//     //   // }
//     // });
// }
