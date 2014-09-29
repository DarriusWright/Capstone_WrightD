#include "TextComponent.h"
#include <SpriteFont.h>
#include <sstream>
#include <iomanip>
#include <SpriteBatch.h>
#include <Window\Game.h>
#include <Misc\Utility.h>

namespace Library
{
	RTTI_DEFINITIONS(TextComponent);

	TextComponent::TextComponent(Game & game) : DrawableGameComponent(game), mText(L"Default Text"), mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(0.0f, 100.0f)
	{

	}

	TextComponent::TextComponent(Game & game, std::wstring text, XMFLOAT4 color) : DrawableGameComponent(game), mText(text), mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(0.0f, 100.0f), mColor(color)
	{

	}


	TextComponent::~TextComponent()
	{
		DeleteObject(mSpriteFont);
		DeleteObject(mSpriteBatch);
	}

	void TextComponent::setColor(XMFLOAT4 color)
	{
		mColor = color;
	}
	void TextComponent::setColor(float r, float g, float b, float a)
	{
		mColor = XMFLOAT4(r, g, b, a);
	}

	void TextComponent::setText(std::wstring text)
	{
		mText = text;
	}
	const std::wstring & TextComponent::getText()const
	{
		return mText;
	}


	XMFLOAT2& TextComponent::getTextPosition()
	{
		return mTextPosition;
	}

	void TextComponent::setTextPosition(XMFLOAT2 position)
	{
		mTextPosition = position;
	}



	void TextComponent::initialize()
	{
		SetCurrentDirectory(L"D:/03-Captstone/DirectX/VisualStudio/Quantum/Debug/");
		mSpriteBatch = new SpriteBatch(mGame->deviceContext());
		mSpriteFont = new SpriteFont(mGame->device(), L"Content/Font/Arial_14_Regular.spritefont");

	}
	void TextComponent::update(const GameTimer & gameTime)
	{
		DrawableGameComponent::update(gameTime);
	}
	void TextComponent::draw(const GameTimer & gameTime)
	{
		mSpriteBatch->Begin();
		std::wostringstream textLabel;
		textLabel << mText;
		mSpriteFont->DrawString(mSpriteBatch, textLabel.str().c_str(), mTextPosition,XMLoadFloat4(&mColor));
		mSpriteBatch->End();
	}


}