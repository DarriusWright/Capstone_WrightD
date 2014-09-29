#include "GameClock.h"
#include "GameTimer.h"

namespace Library
{


	GameClock::GameClock() : mStartTime(), mCurrentTime(), mLastTime(), mFrequency()
	{
		mFrequency = getFrequency();
		reset();
	}


	GameClock::~GameClock()
	{
	}

	const LARGE_INTEGER& GameClock::currentTime() const
	{
		return mCurrentTime;
	}
	double GameClock::getFrequency() const
	{
		LARGE_INTEGER frequency;
		if (QueryPerformanceFrequency(&frequency) == false)
		{
			throw std::exception("QueryPerformaceFrequency() failed.");
		}

		return (double)frequency.QuadPart;
	}
	void GameClock::getTime(LARGE_INTEGER & time) const
	{
		QueryPerformanceCounter(&time);
	}
	const LARGE_INTEGER& GameClock::lastTime() const
	{
		return mLastTime;
	}
	void GameClock::reset()
	{
		getTime(mStartTime);
		mCurrentTime = mStartTime;
		mLastTime = mCurrentTime;
	}
	const LARGE_INTEGER& GameClock::startTime()
	{
		return mStartTime;
	}
	void GameClock::update(GameTimer & time)
	{
		getTime(mCurrentTime);
		time.setTotalGameTime((mCurrentTime.QuadPart - mStartTime.QuadPart)/mFrequency);
		time.setElapsedGameTime((mCurrentTime.QuadPart - mLastTime.QuadPart) / mFrequency);
		mLastTime = mCurrentTime;
	}
}

