#pragma once
#include <Windows.h>
#include <exception>
namespace Library
{
	class GameTimer
	{
	public:
		GameTimer();
		GameTimer(double totalGameTime, double elapsedGameTime);
		~GameTimer();
		double elapsedGameTime() const;
		double totalGameTime() const;
		void setElapsedGameTime(double  elapsedTime);
		void setTotalGameTime(double totalTime);

	private:
		double mElapsedGameTime;
		double mTotalGameTime;

	};

}
