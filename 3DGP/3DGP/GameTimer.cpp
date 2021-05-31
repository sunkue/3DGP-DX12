#include "stdafx.h"
#include "GameTimer.h"


//////////////// init //////////////////////////////////

GameTimer::GameTimer()
	:mSampleCount		{ 0 }
	, mCurrentFrameRate	{ 0 }
	, mFramesPerSecond	{ 0 }
	, mFPSTimeElapsed	{ milliseconds::zero() }
	, mbStopped			{ false }
	, mTimeElapsed		{ milliseconds::zero() }
	, mTimePaused		{ milliseconds::zero() }
	, mTimePlayed		{ milliseconds::zero() }
{
	ResetTimePoints();
	mFrameTimes.fill(milliseconds::zero());
}

GameTimer::~GameTimer()
{

}

///////////////// public ////////////////////////////////


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

size_t GameTimer::GetFrameRate(wchar_t* lpszString, const size_t characters)
{
	if (lpszString)
	{
		_itow_s(static_cast<int>(mCurrentFrameRate), lpszString, characters, 10);
		wcscat_s(lpszString, characters, _T(" FPS)"));
	}

	return mCurrentFrameRate;
}

void GameTimer::Reset()
{
	ResetTimePoints();
	ResetDurations();

	mbStopped = false;
}


///////////////// private ////////////////////////////////

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

_forceinline void GameTimer::ResetTimePoints()
{
	TimePoint now	{ steady_clock::now() };
	mBaseTime		= now;
	mLastTime		= now;
	mCurrentTime	= now;
	mStopTime		= now;
}

_forceinline void GameTimer::ResetDurations()
{
	mTimeElapsed	= milliseconds::zero();
	mTimePlayed		= milliseconds::zero();
	mTimePaused		= milliseconds::zero();
}
