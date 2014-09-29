#include "FpsComponent.h"
#include <SpriteFont.h>
#include <sstream>
#include <iomanip>
#include <SpriteBatch.h>
#include <Window\Game.h>
#include <Misc\Utility.h>

namespace Library
{
	RTTI_DEFINITIONS(FpsComponent)
	FpsComponent::FpsComponent(Game & game) : DrawableGameComponent(game), mTextPosition(0.0f, 60.0f), mFrameCount(0),
	mLastTotalElapsedTime(0.0), mFrameRate(0), mFpsText(new TextComponent(game))
	{

	}


	FpsComponent::~FpsComponent()
	{
		DeleteObject(mFpsText);
	}


	int FpsComponent::frameRate()const
	{
		return mFrameCount;
	}

	XMFLOAT2& FpsComponent::getTextPosition()
	{
		return mFpsText->getTextPosition();
	}
	void FpsComponent::setTextPosition(XMFLOAT2 position)
	{
		mFpsText->setTextPosition(position);
	}

	void FpsComponent::initialize()
	{
		mFpsText->initialize();
		mFpsText->setTextPosition(mTextPosition);
		mFpsText->setColor(0, 0, 0, 1);

	}
	void FpsComponent::update(const GameTimer & gameTime)
	{
		if (gameTime.totalGameTime() - mLastTotalElapsedTime >= 1)
		{
			mLastTotalElapsedTime = gameTime.totalGameTime();
			mFrameRate = mFrameCount;
			mFrameCount = 0;
		}
		mFrameCount++;
		mFpsText->update(gameTime);
	}
	void FpsComponent::draw(const GameTimer & gameTime)
	{
		std::wostringstream fpsLabel;
		fpsLabel << std::setprecision(4) << L"Frame Rate: " << mFrameRate << " Total Elapsed Time: " << gameTime.totalGameTime();
		mFpsText->setText(fpsLabel.str());
		mFpsText->draw(gameTime);
	}


}