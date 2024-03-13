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
#include <Geode/binding/CustomSongWidget.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
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

#include <filesystem>
namespace fs = std::filesystem;

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

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "geometrize/shaperesult.h"
#include "geometrize/bitmap/bitmap.h"
#include "geometrize/bitmap/rgba.h"
#include "geometrize/runner/imagerunner.h"
#include "geometrize/runner/imagerunneroptions.h"
#include "geometrize/shape/circle.h"


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

// geometrize::Bitmap readImageFromClipboard()
// {
// 	if (!OpenClipboard(nullptr))
// 	{
// 		std::cout << "Failed to open clipboard." << std::endl;
// 		return geometrize::Bitmap(0, 0, geometrize::rgba{0, 0, 0, 0});
// 	}
//
//   std::cout<<"Format Bitmap: "<<IsClipboardFormatAvailable(CF_BITMAP)<<"\n";
//   std::cout<<"Format DIB: "<<IsClipboardFormatAvailable(CF_DIB)<<"\n";
//   std::cout<<"Format DIBV5: "<<IsClipboardFormatAvailable(CF_DIBV5)<<"\n";
//   std::cout<<"Format DIF: "<<IsClipboardFormatAvailable(CF_DIF)<<"\n";
//   std::cout<<"Format DSPBITMAP: "<<IsClipboardFormatAvailable(CF_DSPBITMAP)<<"\n";
//   std::cout<<"Format DSPENHMETAFILE: "<<IsClipboardFormatAvailable(CF_DSPENHMETAFILE)<<"\n";
//   std::cout<<"Format DSPMETAFILEPICT: "<<IsClipboardFormatAvailable(CF_DSPMETAFILEPICT)<<"\n";
//   std::cout<<"Format DSPTEXT: "<<IsClipboardFormatAvailable(CF_DSPTEXT)<<"\n";
//   std::cout<<"Format ENHMETAFILE: "<<IsClipboardFormatAvailable(CF_ENHMETAFILE)<<"\n";
//   std::cout<<"Format GDIOBJFIRST: "<<IsClipboardFormatAvailable(CF_GDIOBJFIRST)<<"\n";
//   std::cout<<"Format GDIOBJLAST: "<<IsClipboardFormatAvailable(CF_GDIOBJLAST)<<"\n";
//   std::cout<<"Format HDROP: "<<IsClipboardFormatAvailable(CF_HDROP)<<"\n";
//   std::cout<<"Format LOCALE: "<<IsClipboardFormatAvailable(CF_LOCALE)<<"\n";
//   std::cout<<"Format METAFILEPICT: "<<IsClipboardFormatAvailable(CF_METAFILEPICT)<<"\n";
//   std::cout<<"Format OEMTEXT: "<<IsClipboardFormatAvailable(CF_OEMTEXT)<<"\n";
//   std::cout<<"Format OWNERDISPLAY: "<<IsClipboardFormatAvailable(CF_OWNERDISPLAY)<<"\n";
//   std::cout<<"Format PALETTE: "<<IsClipboardFormatAvailable(CF_PALETTE)<<"\n";
//   std::cout<<"Format PENDATA: "<<IsClipboardFormatAvailable(CF_PENDATA)<<"\n";
//   std::cout<<"Format PRIVATEFIRST: "<<IsClipboardFormatAvailable(CF_PRIVATEFIRST)<<"\n";
//   std::cout<<"Format PRIVATELAST: "<<IsClipboardFormatAvailable(CF_PRIVATELAST)<<"\n";
//   std::cout<<"Format RIFF: "<<IsClipboardFormatAvailable(CF_RIFF)<<"\n";
//   std::cout<<"Format SYLK: "<<IsClipboardFormatAvailable(CF_SYLK)<<"\n";
//   std::cout<<"Format TEXT: "<<IsClipboardFormatAvailable(CF_TEXT)<<"\n";
//   std::cout<<"Format TIFF: "<<IsClipboardFormatAvailable(CF_TIFF)<<"\n";
//   std::cout<<"Format UNICODETEXT: "<<IsClipboardFormatAvailable(CF_UNICODETEXT)<<"\n";
//   std::cout<<"Format WAVE: "<<IsClipboardFormatAvailable(CF_WAVE)<<"\n";
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
//   CloseClipboard();
//
//
//   std::cout<<"Clipboard text: "<<clipboardText <<"\n";
//
// 	HBITMAP hBitmap = static_cast<HBITMAP>(GetClipboardData(CF_BITMAP));
// 	if (!hBitmap)
// 	{
// 		std::cout << "No image found on the clipboard." << std::endl;
// 		CloseClipboard();
// 		return geometrize::Bitmap(0, 0, geometrize::rgba{0, 0, 0, 0});
// 	}
//
// 	BITMAP win_bitmap;
// 	if (!GetObject(hBitmap, sizeof(BITMAP), &win_bitmap))
// 	{
// 		std::cout << "Failed to retrieve bitmap information." << std::endl;
// 		CloseClipboard();
// 		return geometrize::Bitmap(0, 0, geometrize::rgba{0, 0, 0, 0});
// 	}
//
// 	HDC hdc = GetDC(nullptr);
// 	HDC hMemDC = CreateCompatibleDC(hdc);
// 	HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hMemDC, hBitmap));
//
// 	std::uint32_t image_w = static_cast<std::uint32_t>(win_bitmap.bmWidth);
// 	std::uint32_t image_h = static_cast<std::uint32_t>(win_bitmap.bmHeight);
// 	
// 	geometrize::Bitmap bitmap(image_w, image_h, geometrize::rgba{0, 0, 0, 0});
// 	
// 	//idk how to use win bitmap data
// 	for (std::uint32_t y = 0; y < image_h / 2; ++y)
// 	{
// 		for (std::uint32_t x = 0; x < image_w; ++x)
// 		{
// 			COLORREF topPixel = GetPixel(hMemDC, x, y);
// 			COLORREF bottomPixel = GetPixel(hMemDC, x, image_h - y - 1);
// 	
// 			std::uint8_t topRed = GetRValue(topPixel);
// 			std::uint8_t topGreen = GetGValue(topPixel);
// 			std::uint8_t topBlue = GetBValue(topPixel);
// 			std::uint8_t topAlpha = topPixel >> 24;
// 			bitmap.setPixel(x, image_h - y - 1, {topRed, topGreen, topBlue, topAlpha});
//
// 			std::uint8_t bottomRed = GetRValue(bottomPixel);
// 			std::uint8_t bottomGreen = GetGValue(bottomPixel);
// 			std::uint8_t bottomBlue = GetBValue(bottomPixel);
// 			std::uint8_t bottomAlpha = bottomPixel >> 24;
// 	
// 			bitmap.setPixel(x, y, {bottomRed, bottomGreen, bottomBlue, bottomAlpha});
// 		}
// 	}
// 	
// 	SelectObject(hMemDC, hOldBitmap);
// 	DeleteDC(hMemDC);
// 	ReleaseDC(nullptr, hdc);
// 	CloseClipboard();
// 	return bitmap;
// }

geometrize::Bitmap readImage(const std::string& filePath)
{
	if(filePath.empty()) {
		return geometrize::Bitmap(0, 0, geometrize::rgba{0, 0, 0, 0});
	}
	
	const char* path{filePath.c_str()};
	int w = 0;
	int h = 0;
	stbi_set_flip_vertically_on_load(true);
	std::uint8_t* dataPtr{stbi_load(path, &w, &h, nullptr, 4)};
	if(dataPtr == nullptr) {
		return geometrize::Bitmap(0, 0, geometrize::rgba{0, 0, 0, 0});
	}
	const std::vector<std::uint8_t> data{dataPtr, dataPtr + (w * h * 4)};
	delete dataPtr;

	const geometrize::Bitmap bitmap(static_cast<std::uint32_t>(w), static_cast<std::uint32_t>(h), data);
	return bitmap;
}


geometrize::ShapeTypes shapeTypesForNames(const std::string& str)
{
	// Split string into words based on whitespace
	std::istringstream iss(str);
	const std::vector<std::string> shapeNames(std::istream_iterator<std::string>{iss},
									std::istream_iterator<std::string>());
	
	std::vector<geometrize::ShapeTypes> shapeTypes;
	
	// Convert the shape names into ShapeTypes
	for(const std::string& shapeName : shapeNames) {
		for(const std::pair<geometrize::ShapeTypes, std::string>& p : geometrize::shapeTypeNames) {
			if(p.second == shapeName) {
				shapeTypes.push_back(p.first);
			}
		}
	}
	
	if(shapeTypes.empty()) {
		std::cout << "Bad shape names provided, defaulting to ellipses \n";
		return geometrize::ELLIPSE;
	}
	
	// Combine shape types together
	std::underlying_type<geometrize::ShapeTypes>::type combinedShapeTypes = 0;
	for (const auto& shapeType : shapeTypes) {
		combinedShapeTypes |= shapeType;
	}
	return geometrize::ShapeTypes(combinedShapeTypes);
}


void addImage(geometrize::Bitmap bitmap, int totalShapes, int shapesPerStep)
{
	if(bitmap.getWidth() == 0 || bitmap.getHeight() == 0)
	{
		return;
	}

  std::vector<geometrize::ShapeResult> shape_data;
	
	geometrize::ImageRunnerOptions options;
  options.maxThreads = 6;
	// options.shapeCount = 10;
	// options.alpha = 128;
	// options.maxShapeMutations = 86;
	// options.shapeTypes = shapeTypesForNames("circle");

    // geometrize::ShapeTypes shapeTypes = geometrize::ShapeTypes::ELLIPSE; ///< The shape types that the image runner shall use.
    // std::uint8_t alpha = 128U; ///< The alpha/opacity of the shapes (0-255).
    // std::uint32_t shapeCount = 50U; ///< The number of candidate shapes that will be tried per model step.
    // std::uint32_t maxShapeMutations = 100U; ///< The maximum number of times each candidate shape will be modified to attempt to find a better fit.
    // std::uint32_t seed = 9001U; ///< The seed for the random number generators used by the image runner.
    // std::uint32_t maxThreads = 0; ///< The maximum number of separate threads for the implementation to use. 0 lets the implementation choose a reasonable number.
    // ImageRunnerShapeBoundsOptions shapeBounds{}; ///< If zero or do not form a rectangle, the entire target image is used i.e. (0, 0, imageWidth, imageHeight)
	
	geometrize::ImageRunner runner(bitmap);
  int drawn_shapes = 0;
	
	// SHAPE_LOCK.lock();
	// SHAPE_LOCK.unlock();
	
	while(totalShapes > shape_data.size())
	{
		const std::vector<geometrize::ShapeResult> shapes{runner.step(options)};
		// SHAPE_LOCK.lock();
		for(const auto& shape : shapes)
		{
      log::info("SHAPE!! {}", shape_data.size());
      switch(shape.shape->getType())
      {
        case geometrize::ShapeTypes::CIRCLE:
        {
          geometrize::rgba color = shape.color;
          auto circle = dynamic_cast<geometrize::Circle*>(shape.shape.get());
          // log::info("STEP: {} | Circle | x: {}, y: {}, r: {}, RGBA: {},{},{},{}",
          //   step, circle->m_x, circle->m_y, circle->m_r, color.r, color.g, color.b, color.a);
        }
      }
			shape_data.push_back(shape);
		}
		// SHAPE_LOCK.unlock();
	}
	// disableUpdateHook();
}

class $modify(JBLevelInfoLayer, LevelInfoLayer) {
  bool init(GJGameLevel* level, bool p1) {
    if (!LevelInfoLayer::init(level, p1)) {
      return false;
    }
    log::info("Level info layer initialized {}", level->m_creatorName);

    file::FilePickOptions options = {
        std::nullopt,
        {}
    };

    geometrize::Bitmap _bitmap = geometrize::Bitmap(0, 0, geometrize::rgba{0, 0, 0, 0});
    _bitmap = readImage("Z:\\home\\flafy\\Downloads\\hhkey.png");
    addImage(_bitmap, 20, 1);

    // auto callback = [this](ghc::filesystem::path result) {
    //     auto path = fs::path(result.c_str());
    //     #ifdef GEODE_IS_WINDOWS
    //     auto strPath = geode::utils::string::wideToUtf8(result.c_str());
    //     #else
    //     std::string strPath = result.c_str();
    //     #endif
    //
    //     geometrize::Bitmap _bitmap = geometrize::Bitmap(0, 0, geometrize::rgba{0, 0, 0, 0});
    //     _bitmap = readImage(strPath);
    // };
    //
    // auto failedCallback = []() {
    //     FLAlertLayer::create("Error", "Failed to open file", "Ok")->show();
    // };
    //
    // file::pickFile(
    //     file::PickMode::OpenFile,
    //     options,
    //     callback,
    //     failedCallback
    // );

    return true;
  }
};

