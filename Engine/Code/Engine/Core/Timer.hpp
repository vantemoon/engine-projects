#pragma once


//-----------------------------------------------------------------------------------------------
class Clock;


//-----------------------------------------------------------------------------------------------
class Timer
{
public:
	explicit Timer( double period, Clock const* clock = nullptr );

	void Start();
	void Stop();

	double GetElapsedSeconds() const;
	double GetElapsedFraction() const;

	bool IsStopped() const;
	bool HasPeriodElapsed() const;
	bool DecrementPeriodIfElapsed();

	Clock const* m_clock = nullptr;
	double m_startTime = -1.0;
	double m_period = 0.0;
};