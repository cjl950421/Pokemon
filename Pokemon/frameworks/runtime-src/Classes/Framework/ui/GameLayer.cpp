#include "GameLayer.h"
#include "lua/LuaUtils.h"

using namespace cocos2d;

namespace framework
{

GameLayer::GameLayer()
	: _parentLayer(nullptr)
	, _eventLayer(nullptr)
	, _touchListener(nullptr)
	, _keyboardListener(nullptr)
	, _isEnabled(true)
{
}

bool GameLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	// create touch layer
	this->_eventLayer = EventLayer::create();
	// initialize touch listener
	this->_touchListener = EventListenerTouchOneByOne::create();
	this->_touchListener->onTouchBegan = CC_CALLBACK_2(EventLayer::onTouchBegan, this->_eventLayer);
	this->_touchListener->onTouchMoved = CC_CALLBACK_2(EventLayer::onTouchMoved, this->_eventLayer);
	this->_touchListener->onTouchEnded = CC_CALLBACK_2(EventLayer::onTouchEnded, this->_eventLayer);
	this->_touchListener->onTouchCancelled = CC_CALLBACK_2(EventLayer::onTouchCancelled, this->_eventLayer);
	// initialize keyboard listener
	this->_keyboardListener = EventListenerKeyboard::create();
	this->_keyboardListener->onKeyPressed = CC_CALLBACK_2(EventLayer::onKeyPressed, this->_eventLayer);
	this->_keyboardListener->onKeyReleased = CC_CALLBACK_2(EventLayer::onKeyReleased, this->_eventLayer);
}

void GameLayer::pushLayer(GameLayer *pLayer)
{
	CCASSERT(pLayer, "the layer should not be null");

	pLayer->_parentLayer = this;
	this->setEnabled(false);

	auto pCurrentScene = this->getScene();
	pCurrentScene->addChild(pLayer);
}

void GameLayer::popLayer()
{
	if (this->_parentLayer)
	{
		this->_parentLayer->setEnabled(true);
	}
	this->removeFromParentAndCleanup(true);
}

bool GameLayer::isCoreLayer()
{
	return !this->_parentLayer;
}

GameLayer *GameLayer::getParentLayer()
{
	return this->_parentLayer;
}

GameLayer *GameLayer::getCoreLayer()
{
	auto currentLayer = this;
	while (currentLayer->_parentLayer)
	{
		currentLayer = currentLayer->_parentLayer;
	}
	return currentLayer;
}

void GameLayer::setEventLayer(EventLayer *pLayer)
{
	CCASSERT(pLayer, "Event layer can't be null");
	if (this->_eventLayer)
	{
		this->removeChild(_eventLayer);
	}
	
	this->_eventLayer = pLayer;
	this->addChild(pLayer);
}

void GameLayer::onEnter()
{
	Layer::onEnter();
#if ScriptType == 1
	LuaUtils::executePeertableFunction(this, "onEnter", nullptr, nullptr, false);
#endif
}

void GameLayer::onExit()
{
	Layer::onExit();
#if ScriptType == 1
	LuaUtils::executePeertableFunction(this, "onExit", nullptr, nullptr, false);
#endif
}

void GameLayer::registerTouchEvents()
{
	if (this->_eventLayer)
	{
		this->_eventLayer->enableTouch();
	}
}

void GameLayer::unregisterTouchEvents()
{
	if (this->_eventLayer)
	{
		this->_eventLayer->disableTouch();
	}
}

void GameLayer::registerKeyboardEvents()
{
	if (this->_eventLayer)
	{
		this->_eventLayer->enableKeyboard();
	}
}

void GameLayer::unregisterKeyboardEvents()
{
	if (this->_eventLayer)
	{
		this->_eventLayer->disableKeyboard();
	}
}

GameLayer::~GameLayer()
{
	CC_SAFE_RELEASE(this->_eventLayer);
	CC_SAFE_RELEASE_NULL(this->_touchListener);
	CC_SAFE_RELEASE_NULL(this->_keyboardListener);
}

}