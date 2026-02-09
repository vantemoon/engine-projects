#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "math.h"


//-----------------------------------------------------------------------------------------------
Timer::Timer( double period, Clock const* clock )
	: m_period( period )
	, m_clock( clock )
{
}


//-----------------------------------------------------------------------------------------------
void Timer::Start()
{
	if ( m_clock != nullptr )
	{
		m_startTime = m_clock->GetTotalSeconds();
	}
	else
	{
		m_startTime = GetCurrentTimeSeconds();
	}
}


//-----------------------------------------------------------------------------------------------
void Timer::Stop()
{
	m_startTime = -1.0;
}


//-----------------------------------------------------------------------------------------------
double Timer::GetElapsedSeconds() const
{
	if ( m_startTime < 0.0 )
	{
		return 0.0;
	}
	double currentTime;
	if ( m_clock != nullptr )
	{
		currentTime = m_clock->GetTotalSeconds();
	}
	else
	{
		currentTime = GetCurrentTimeSeconds();
	}
	return currentTime - m_startTime;
}


//-----------------------------------------------------------------------------------------------
double Timer::GetElapsedFraction() const
{
	double elapsedSeconds = GetElapsedSeconds();
	if ( m_period > 0.0 )
	{
		return elapsedSeconds / m_period;
	}
	return 0.0;
}


//-----------------------------------------------------------------------------------------------
bool Timer::IsStopped() const
{
	return m_startTime < 0.0;
}


//-----------------------------------------------------------------------------------------------
bool Timer::HasPeriodElapsed() const
{
	return GetElapsedSeconds() >= m_period;
}


//-----------------------------------------------------------------------------------------------
bool Timer::DecrementPeriodIfElapsed()
{
	if ( HasPeriodElapsed() )
	{
		double elapsedSeconds = GetElapsedSeconds();
		double periodsElapsed = floor( elapsedSeconds / m_period );
		m_startTime += periodsElapsed * m_period;
		return true;
	}
	return false;
}