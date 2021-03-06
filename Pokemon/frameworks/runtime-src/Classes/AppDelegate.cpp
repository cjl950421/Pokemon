#include "AppDelegate.h"
#include "CCLuaEngine.h"
#include "SimpleAudioEngine.h"
#include "cocos2d.h"

#include "lua/lua_framework_auto.hpp"
#include "version.h"

using namespace CocosDenshion;

USING_NS_CC;
using namespace std;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
    SimpleAudioEngine::end();
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if(!glview) {
		glview = GLView::createWithRect("Pokemon Sunrise", Rect(0, 0, 640, 480));
		director->setOpenGLView(glview);
	}

    glview->setDesignResolutionSize(640, 480, ResolutionPolicy::NO_BORDER);

	FileUtils::getInstance()->addSearchPath("res");

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

	auto pEngine = LuaEngine::getInstance();
	ScriptEngineManager::getInstance()->setScriptEngine(pEngine);
	// bind our framework to lua.
	register_all_psframework(pEngine->getLuaStack()->getLuaState());
	// version management.
	::printVersion();
	lua_register_psframework_version(pEngine->getLuaStack()->getLuaState());

	pEngine->executeScriptFile("src/main.lua");

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
