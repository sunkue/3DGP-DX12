#include "stdafx.h"
#include "GameTimer.h"


GameTimer::GameTimer()
{
	ResetTimePoints();
	m_FrameTimes.fill(milliseconds::zero());
}

void GameTimer::Tick(const milliseconds lockFPS)
{
	m_CurrentTime = steady_clock::now();
	milliseconds timeElapsed = duration_cast<milliseconds>(m_CurrentTime - m_LastTime);

	while (timeElapsed < lockFPS)
	{
		m_CurrentTime	= steady_clock::now();
		timeElapsed		= duration_cast<milliseconds>(m_CurrentTime - m_LastTime);
	}

	m_LastTime = m_CurrentTime;

	CaculateFrameStates(timeElapsed);

	m_TimePlayed += timeElapsed;
	m_TimePaused = m_Stopped ? (m_TimePaused + timeElapsed) : (milliseconds::zero());
}

void GameTimer::Reset()
{
	ResetTimePoints();
	ResetDurations();
	m_Stopped = false;
}

void GameTimer::CaculateFrameStates(const milliseconds timeElapsed)
{
	if (abs(timeElapsed - m_TimeElapsed) < 1s)
	{
		memmove(
			  &m_FrameTimes.at(1)
			, m_FrameTimes.data()
			, (m_FrameTimes.max_size() - 1) * sizeof(milliseconds));
		m_FrameTimes[0] = timeElapsed;
		if (m_SampleCount < m_FrameTimes.max_size()) ++m_SampleCount;
	}

	m_FramesPerSecond++;
	m_FPSTimeElapsed += timeElapsed;
	if (1s < m_FPSTimeElapsed)
	{
		m_CurrentFrameRate	= m_FramesPerSecond;
		m_FramesPerSecond	= 0;
		m_FPSTimeElapsed		= milliseconds::zero();
	}

	/* (mTimeElapsed) is average elapsed time for each (mSampleCount) frames */
	m_TimeElapsed = milliseconds::zero();
	for (auto& mFT : m_FrameTimes) m_TimeElapsed += mFT;
	if (0 != m_SampleCount) m_TimeElapsed /= m_SampleCount;
}

void GameTimer::ResetTimePoints()
{
	TimePoint now	{ steady_clock::now() };
	m_BaseTime		= now;
	m_LastTime		= now;
	m_CurrentTime	= now;
	m_StopTime		= now;
}

void GameTimer::ResetDurations()
{
	m_TimeElapsed	= milliseconds::zero();
	m_TimePlayed		= milliseconds::zero();
	m_TimePaused		= milliseconds::zero();
}
