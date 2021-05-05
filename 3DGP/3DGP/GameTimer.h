#pragma once

class GameTimer
{
public:
	GameTimer();
	virtual ~GameTimer();

public:
	void Start() {}
	void Stop() {}
	void Reset();
	void Tick(milliseconds fLockFPS = 0ms);
	size_t GetFrameRate(wchar_t* lpszString = nullptr, size_t characters = 0);
	milliseconds GetTimeElapsed();

private:
	TimePoint mCurrentTime;
	TimePoint mLastTime;

	milliseconds mTimeElapsed;

	array<milliseconds, 50> mFrameTimes;
	size_t mSampleCount;

	size_t mCurrentFrameRate;
	size_t mFramesPerSecond;
	milliseconds mFPSTimeElapsed;

	bool mbStopped;
};

