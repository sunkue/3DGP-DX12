#pragma once

constexpr unsigned long MAX_SAMPLE_COUNT{ 50 };

class GameTimer
{
public:
	GameTimer();
	virtual ~GameTimer();

public:
	void Start(){}
	void Stop(){}
	void Reset();
	void Tick(float fLockFPS = 0.0f);
	unsigned long GetFrameRate(LPSTR lpszString = nullptr, int characters = 0);
	float GetTimeElapsed();

private:
	bool mbHardWateHasPerformanceCounter;
	float mTimeScale;
	float mTimeElapsed;
	__int64 mCurrentTime;
	__int64 mLastTime;
	__int64 mPerformanceFrequency;

	float mFrameTime[MAX_SAMPLE_COUNT];
	unsigned long mSampleCount;

	unsigned long mCurrentFrameRate;
	unsigned long mFramesPerSecond;
	float mFPSTimeElapsed;

	bool mbStopped;
};

