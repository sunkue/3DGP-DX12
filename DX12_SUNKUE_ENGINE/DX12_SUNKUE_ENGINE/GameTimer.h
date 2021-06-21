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
	GameTimer();
	virtual ~GameTimer() = default;

public:
	void Start() { m_Stopped = false; };
	void Stop() { m_StopTime = m_CurrentTime; m_Stopped = true; };
	void Reset();
	void Tick(const milliseconds LockFPS = 0ms);
	size_t GetFrameRate() const { return m_CurrentFrameRate; }
	float GetTimeElapsedSec() const { return MillisecToSec(m_TimeElapsed); }
	milliseconds GetTimeElapsed() const { return m_TimeElapsed; }
	milliseconds GetTimePaused() const { return m_TimePaused; }
	milliseconds GetTimePlayed() const { return m_TimePlayed; }

private:
	void CaculateFrameStates(const milliseconds timeElapsed);
	void ResetTimePoints();
	void ResetDurations();

private:
	TimePoint		m_BaseTime;
	TimePoint		m_LastTime;
	TimePoint		m_CurrentTime;
	TimePoint		m_StopTime;

	milliseconds	m_TimeElapsed{ milliseconds::zero() };
	milliseconds	m_TimePlayed{ milliseconds::zero() };
	milliseconds	m_TimePaused{ milliseconds::zero() };

	std::array<milliseconds, 50>	m_FrameTimes;
	size_t			m_SampleCount{ 0 };
	size_t			m_CurrentFrameRate{ 0 };
	size_t			m_FramesPerSecond{ 0 };
	milliseconds	m_FPSTimeElapsed{ milliseconds::zero() };

	bool			m_Stopped{ true };
};

