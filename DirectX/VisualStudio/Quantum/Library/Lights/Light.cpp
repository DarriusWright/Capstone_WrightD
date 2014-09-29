#include "Light.h"

#include <Misc\Colors.h>

namespace Library
{
	RTTI_DEFINITIONS(Light)
	Light::Light(Game & game) : GameComponent(game), mColor(reinterpret_cast<float*>(&Colors::WHITE))
	{

	}


	Light::~Light()
	{

	}


	const XMCOLOR & Light::color()const
	{
		return mColor;
	}
	const XMVECTOR & Light::colorVector()const
	{
		return XMLoadColor(&mColor);
	}
	void Light::setColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
	{
		XMCOLOR color = XMCOLOR(r, g, b, a);
		setColor(color);
	}
	void Light::setColor(XMCOLOR color)
	{
		mColor = color;
	}
	void Light::setColor(FXMVECTOR color)
	{
		XMStoreColor(&mColor, color);
	}


}