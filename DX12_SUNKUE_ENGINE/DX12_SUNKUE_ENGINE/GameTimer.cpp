#include "stdafx.h"
#include "GameTimer.h"


GameTimer::GameTimer()
{
	ResetTimePoints();
	mFrameTimes.fill(milliseconds::zero());
}

void GameTimer::Tick(const milliseconds lockFPS)
{
	mCurrentTime = steady_clock::now();
	milliseconds timeElapsed = duration_cast<milliseconds>(mCurrentTime - mLastTime);

	while (timeElapsed < lockFPS)
	{
		mCurrentTime	= steady_clock::now();
		timeElapsed		= duration_cast<milliseconds>(mCurrentTime - mLastTime);
	}

	mLastTime = mCurrentTime;

	CaculateFrameStates(timeElapsed);

	mTimePlayed += timeElapsed;
	mTimePaused = mbStopped ? (mTimePaused + timeElapsed) : (milliseconds::zero());
}

void GameTimer::Reset()
{
	ResetTimePoints();
	ResetDurations();
	mbStopped = false;
}

void GameTimer::CaculateFrameStates(const milliseconds timeElapsed)
{
	if (abs(timeElapsed - mTimeElapsed) < 1s)
	{
		memmove(
			  &mFrameTimes.at(1)
			, mFrameTimes.data()
			, (mFrameTimes.max_size() - 1) * sizeof(milliseconds));
		mFrameTimes[0] = timeElapsed;
		if (mSampleCount < mFrameTimes.max_size()) ++mSampleCount;
	}

	mFramesPerSecond++;
	mFPSTimeElapsed += timeElapsed;
	if (1s < mFPSTimeElapsed)
	{
		mCurrentFrameRate	= mFramesPerSecond;
		mFramesPerSecond	= 0;
		mFPSTimeElapsed		= milliseconds::zero();
	}

	/* (mTimeElapsed) is average elapsed time for each (mSampleCount) frames */
	mTimeElapsed = milliseconds::zero();
	for (auto& mFT : mFrameTimes) mTimeElapsed += mFT;
	if (0 != mSampleCount) mTimeElapsed /= mSampleCount;
}

void GameTimer::ResetTimePoints()
{
	TimePoint now	{ steady_clock::now() };
	mBaseTime		= now;
	mLastTime		= now;
	mCurrentTime	= now;
	mStopTime		= now;
}

void GameTimer::ResetDurations()
{
	mTimeElapsed	= milliseconds::zero();
	mTimePlayed		= milliseconds::zero();
	mTimePaused		= milliseconds::zero();
}
