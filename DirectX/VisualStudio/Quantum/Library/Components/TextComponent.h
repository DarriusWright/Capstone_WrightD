#pragma once
#include <Components\DrawableGameComponent.h>


namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}
namespace Library
{
	class TextComponent : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(TextComponent, DrawableGameComponent)
	public:
		TextComponent(Game & game);
		TextComponent(Game & game, std::wstring text, XMFLOAT4 color = XMFLOAT4(0.0f,0.0f,0.0f,1.0f));
		~TextComponent();
		XMFLOAT2& getTextPosition();
		void setTextPosition(XMFLOAT2 position);
		int frameRate()const;
		void setText(std::wstring text);
		const std::wstring & getText()const;


		void setColor(XMFLOAT4 color);
		void setColor(float r,float g,float b, float a);

		virtual void initialize() override;
		virtual void update(const GameTimer & gameTime)override;
		virtual void draw(const GameTimer & gameTime)override;


	private:
		TextComponent();
		TextComponent(const TextComponent & rhs);
		TextComponent & operator=(const TextComponent & rhs);
		SpriteBatch * mSpriteBatch;
		SpriteFont * mSpriteFont;
		XMFLOAT2 mTextPosition;
		std::wstring mText;
		XMFLOAT4 mColor;

	};
}
