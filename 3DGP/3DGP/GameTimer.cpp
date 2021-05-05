#include "stdafx.h"
#include "GameTimer.h"

GameTimer::GameTimer() :
	mSampleCount{ 0 },
	mCurrentFrameRate{ 0 },
	mFramesPerSecond{ 0 },
	mFPSTimeElapsed{ milliseconds::zero() },
	mbStopped{ false },
	mLastTime{ steady_clock::now() },
	mCurrentTime{ steady_clock::now() },
	mTimeElapsed{ milliseconds::zero() }
{
	mFrameTimes.fill(milliseconds::zero());
}

GameTimer::~GameTimer()
{

}

///////////////////////

void GameTimer::Tick(milliseconds lockFPS)
{
	mCurrentTime = steady_clock::now();
	milliseconds timeElapsed = duration_cast<milliseconds>(mCurrentTime - mLastTime);

	while (timeElapsed < lockFPS)
	{
		mCurrentTime = steady_clock::now();
		timeElapsed = duration_cast<milliseconds>(mCurrentTime - mLastTime);
	}

	mLastTime = mCurrentTime;

	if (abs(timeElapsed - mTimeElapsed) < 1s)
	{
		memmove(&mFrameTimes.at(1), mFrameTimes.data(), (mFrameTimes.max_size() - 1) * sizeof(milliseconds));
		mFrameTimes[0] = timeElapsed;
		if (mSampleCount < mFrameTimes.max_size()) ++mSampleCount;
	}

	mFramesPerSecond++;
	mFPSTimeElapsed += timeElapsed;
	if (1s < mFPSTimeElapsed)
	{
		mCurrentFrameRate = mFramesPerSecond;
		mFramesPerSecond = 0;
		mFPSTimeElapsed = milliseconds::zero();
	}

	mTimeElapsed = milliseconds::zero();
	for (auto& mFT : mFrameTimes) mTimeElapsed += mFT;
	if (0 != mSampleCount) mTimeElapsed /= mSampleCount;
}

size_t GameTimer::GetFrameRate(wchar_t* lpszString, size_t characters)
{
	if (lpszString)
	{
		_itow_s(mCurrentFrameRate, lpszString, characters, 10);
		wcscat_s(lpszString, characters, _T(" FPS)"));
	}

	return mCurrentFrameRate;
}

milliseconds GameTimer::GetTimeElapsed()
{
	return mTimeElapsed;
}

void GameTimer::Reset()
{
	mLastTime = steady_clock::now();
	mCurrentTime = steady_clock::now();

	mSampleCount = 0;
	mCurrentFrameRate = 0;
	mFramesPerSecond = 0;
	mFPSTimeElapsed = milliseconds::zero();
	mTimeElapsed = milliseconds::zero();

	mFrameTimes.fill(milliseconds::zero());

	mbStopped = false;
}