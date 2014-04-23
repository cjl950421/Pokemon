#include "ActiveSprite.h"

using namespace cocos2d;

namespace framework
{

bool ActiveSprite::initWithSprite(Node *pSprite, TouchCallback touchCallback, KeyboardCallback keyboardCallback)
{
	if (!EventNode::init() || !pSprite)
	{
		return false;
	}

	this->_defaultImage = pSprite;
	this->_defaultImage->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
	CC_SAFE_RETAIN(this->_defaultImage);

	this->addChild(pSprite);

	this->setAnchorPoint(Point::ANCHOR_MIDDLE);
	this->setContentSize(pSprite->getContentSize());

	this->_touchCallback = touchCallback;
	this->_keyboardCallback = keyboardCallback;

	return true;
}

ActiveSprite *ActiveSprite::create(const char *szSpritePath, TouchCallback touchCallback, KeyboardCallback keyboardCallback)
{
	auto pSprite = Sprite::create(szSpritePath);
	
	return ActiveSprite::create(pSprite, touchCallback, keyboardCallback);
}

ActiveSprite *ActiveSprite::create(Node *pSprite, TouchCallback touchCallback, KeyboardCallback keyboardCallback)
{
	auto pActiveSprite = new ActiveSprite();

	if (pActiveSprite && pActiveSprite->initWithSprite(pSprite, touchCallback, keyboardCallback))
	{
		pActiveSprite->autorelease();

		return pActiveSprite;
	}

	return nullptr;
}

ActiveSprite::~ActiveSprite()
{
}

void ActiveSprite::updateImageVisibility()
{
	SAFE_SET_VISIBILITY(this->_defaultImage, true);
}

}