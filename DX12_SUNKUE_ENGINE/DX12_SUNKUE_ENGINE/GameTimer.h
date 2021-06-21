#pragma once

#include <chrono>
#include <array>

using namespace std::chrono;
using TimePoint = steady_clock::time_point;

inline float MillisecToSec(milliseconds time) {
	return time.count() / 1000.0f;
}

class GameTimer
{
public:
	GameTimer() = default;
	virtual ~GameTimer() = default;

public:
	void Start() { mbStopped = false; };
	void Stop() { mStopTime = mCurrentTime; mbStopped = true; };
	void Reset();
	void Tick(const milliseconds LockFPS = 0ms);
	size_t GetFrameRate() const { return mCurrentFrameRate; }
	float GetTimeElapsedSec() const { return MillisecToSec(mTimeElapsed); }
	milliseconds GetTimeElapsed() const { return mTimeElapsed; }
	milliseconds GetTimePaused() const { return mTimePaused; }
	milliseconds GetTimePlayed() const { return mTimePlayed; }

private:
	void CaculateFrameStates(const milliseconds timeElapsed);
	void ResetTimePoints();
	void ResetDurations();

private:
	TimePoint		mBaseTime;
	TimePoint		mLastTime;
	TimePoint		mCurrentTime;
	TimePoint		mStopTime;

	milliseconds	mTimeElapsed{ milliseconds::zero() };
	milliseconds	mTimePlayed{ milliseconds::zero() };
	milliseconds	mTimePaused{ milliseconds::zero() };

	std::array<milliseconds, 50>	mFrameTimes;
	size_t			mSampleCount{ 0 };
	size_t			mCurrentFrameRate{ 0 };
	size_t			mFramesPerSecond{ 0 };
	milliseconds	mFPSTimeElapsed{ milliseconds::zero() };

	bool			mbStopped{ true };
};

