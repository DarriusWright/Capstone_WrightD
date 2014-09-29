#pragma once
#include <Components\DrawableGameComponent.h>
#include <Components\TextComponent.h>


namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}
namespace Library
{
	class FpsComponent : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(FpsComponent, DrawableGameComponent)
	public:
		FpsComponent(Game & game);
		~FpsComponent();


		XMFLOAT2& getTextPosition();
		void setTextPosition(XMFLOAT2 position);
		int frameRate()const;


		virtual void initialize() override;
		virtual void update(const GameTimer & gameTime)override;
		virtual void draw(const GameTimer & gameTime)override;


	private:
		FpsComponent();
		FpsComponent(const FpsComponent & rhs);
		FpsComponent & operator=(const FpsComponent & rhs);
		TextComponent * mFpsText;
		
		XMFLOAT2 mTextPosition;
		int mFrameCount;
		int mFrameRate;
		double mLastTotalElapsedTime;

	};
}
