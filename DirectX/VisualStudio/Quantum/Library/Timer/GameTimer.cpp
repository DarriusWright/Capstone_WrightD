#include "GameTimer.h"

namespace Library
{

	GameTimer::GameTimer()
	{
	}

	GameTimer::GameTimer(double totalGameTime, double elapsedGameTime) : mTotalGameTime(totalGameTime), mElapsedGameTime(elapsedGameTime)
	{

	}

	GameTimer::~GameTimer()
	{
	}

	double GameTimer::elapsedGameTime() const
	{
		return mElapsedGameTime;
	}
	double GameTimer::totalGameTime() const
	{
		return mTotalGameTime;
	}
	void   GameTimer::setElapsedGameTime(double  elapsedTime)
	{
		mElapsedGameTime = elapsedTime;
	}
	void   GameTimer::setTotalGameTime(double totalTime)
	{
		mTotalGameTime = totalTime;
	}

}
