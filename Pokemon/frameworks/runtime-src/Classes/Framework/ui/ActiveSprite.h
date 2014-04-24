/******************************
	Description: ActiveSprite, extends EventNode
	Author: M.Wan
	Date: 4/22/2014
******************************/

#ifndef __UI_ACTIVESPRITE__
#define __UI_ACTIVESPRITE__

#include "EventNode.h"

namespace framework
{

class ActiveSprite : public EventNode
{
public:
	bool initWithSprite(cocos2d::Node *pSprite, const TouchCallback &touchCallback, const KeyboardCallback &keyboardCallback);

	static ActiveSprite *create(const char *szSpritePath, const TouchCallback &touchCallback = nullptr, const KeyboardCallback &keyboardCallback = nullptr);
	static ActiveSprite *create(cocos2d::Node *pSprite, const TouchCallback &touchCallback = nullptr, const KeyboardCallback &keyboardCallback = nullptr);
protected:
	virtual ~ActiveSprite();

	void updateImageVisibility() override;
};

}

#endif