#include "stdafx.h"
#include "GameTimer.h"

GameTimer::GameTimer()
{
	if (QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&mPerformanceFrequency)))
	{
		mbHardWateHasPerformanceCounter = true;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mLastTime));
		mTimeScale = 1.0f / mPerformanceFrequency;
	}
	else
	{
		mbHardWateHasPerformanceCounter = false;
		mLastTime = chrono::high_resolution_clock::now();
		mTimeScale = 0.001f;
	}
	mSampleCount = 0;
	mCurrentFrameRate = 0;
	mFramesPerSecond = 0;
	mFPSTimeElapsed = 0.0f;
}