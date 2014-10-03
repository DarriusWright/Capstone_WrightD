#pragma once

#include <Windows.h>

typedef struct{
	LARGE_INTEGER start;
	LARGE_INTEGER stop;
} stopWatch;

class Timer
{
public:
     Timer(void);
	 ~Timer(void);
	 void start();
	 float stop();
	 float interval();
	 float getFramesPerSec()const;
	 //void newFrame();
	 void setStart(LARGE_INTEGER start);
	 void setStop(LARGE_INTEGER stop);
	 LARGE_INTEGER getStart()const;
	 LARGE_INTEGER getStop()const;

private:
	stopWatch timer;
	LARGE_INTEGER frequency;
	float fps;
	float LargeIntToSecs(LARGE_INTEGER & l);
	float LargeIntToFrames(LARGE_INTEGER & l);
	void setFramesPerSec(float frames);
};
