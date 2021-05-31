#pragma once

class GameTimer
{
public:
	GameTimer();
	virtual ~GameTimer();

public:
	void Start()	{ mbStopped = false; };
	void Stop()		{ mStopTime = mCurrentTime; mbStopped = true; };
	void Reset();
	void Tick(const milliseconds fLockFPS = 0ms);
	size_t GetFrameRate(wchar_t* lpszString = nullptr, const size_t characters = 0);
	milliseconds GetTimeElapsed()	const { return mTimeElapsed; }
	milliseconds GetTimePaused()	const { return mTimePaused; }
	milliseconds GetTimePlayed()	const { return mTimePlayed; }

private:
	void CaculateFrameStates(const milliseconds timeElapsed);
	void ResetTimePoints();
	void ResetDurations();

private:
	TimePoint		mBaseTime;
	TimePoint		mLastTime;
	TimePoint		mCurrentTime;
	TimePoint		mStopTime;

	milliseconds	mTimeElapsed;
	milliseconds	mTimePlayed;
	milliseconds	mTimePaused;

	array<milliseconds, 50>		mFrameTimes;
	size_t			mSampleCount;
	size_t			mCurrentFrameRate;
	size_t			mFramesPerSecond;
	milliseconds	mFPSTimeElapsed;

	bool			mbStopped;
};

