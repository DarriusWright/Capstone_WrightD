#pragma once

#include <Windows.h>
#include <exception>

namespace Library
{
	class GameTimer;
	class GameClock
	{
	public:
		GameClock();
		~GameClock();
		const LARGE_INTEGER& currentTime() const ;
		double getFrequency() const ;
		void getTime(LARGE_INTEGER & time) const;
		const LARGE_INTEGER& lastTime() const ;
		void reset();
		const LARGE_INTEGER& startTime();
		void update(GameTimer & time);

	private:
		GameClock(const  GameClock & rhs);
		GameClock& operator=(const GameClock & rhs);

		LARGE_INTEGER mCurrentTime;
		double mFrequency;
		LARGE_INTEGER mLastTime;
		LARGE_INTEGER mStartTime;



	};

}
