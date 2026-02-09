#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"


//-----------------------------------------------------------------------------------------------
static Clock* g_systemClock = nullptr;


//-----------------------------------------------------------------------------------------------
Clock::Clock()
{
	if ( g_systemClock != nullptr )
	{
		g_systemClock->AddChild( this );
		m_parent = g_systemClock;
	}
	else
	{
		g_systemClock = this;
	}
}


//-----------------------------------------------------------------------------------------------
Clock::Clock( Clock& parent )
{
	parent.AddChild( this );
	m_parent = &parent;
}


//-----------------------------------------------------------------------------------------------
Clock::~Clock()
{
	if ( m_parent != nullptr )
	{
		m_parent->RemoveChild( this );
	}
	else if ( g_systemClock == this )
	{
		g_systemClock = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
void Clock::Reset()
{
	m_totalSeconds = 0.0;
	m_deltaSeconds = 0.0;
	m_frameCount = 0;
	m_lastUpdateTimeInSeconds = GetCurrentTimeSeconds();
}


//-----------------------------------------------------------------------------------------------
bool Clock::IsPaused() const
{
	return m_isPaused;
}


//-----------------------------------------------------------------------------------------------
void Clock::Pause()
{
	m_isPaused = true;
}


//-----------------------------------------------------------------------------------------------
void Clock::Unpause()
{
	m_isPaused = false;
}


//-----------------------------------------------------------------------------------------------
void Clock::TogglePause()
{
	m_isPaused = !m_isPaused;
}


//-----------------------------------------------------------------------------------------------
void Clock::StepSingleFrame()
{
	m_stepSingleFrame = true;
}


//-----------------------------------------------------------------------------------------------
void Clock::SetTimeScale( double timeScale )
{
	m_timeScale = timeScale;
}


//-----------------------------------------------------------------------------------------------
double Clock::GetTimeScale() const
{
	return m_timeScale;
}


//-----------------------------------------------------------------------------------------------
double Clock::GetDeltaSeconds() const
{
	return m_deltaSeconds;
}


//-----------------------------------------------------------------------------------------------
double Clock::GetTotalSeconds() const
{
	return m_totalSeconds;
}


//-----------------------------------------------------------------------------------------------
double Clock::GetFrameRate() const
{
	if ( m_deltaSeconds > 0.0 )
	{
		return 1.0 / m_deltaSeconds;
	}
	return 0.0;
}


//-----------------------------------------------------------------------------------------------
int Clock::GetFrameCount() const
{
	return m_frameCount;
}


//-----------------------------------------------------------------------------------------------
Clock& Clock::GetSystemClock()
{
	return *g_systemClock;
}


//-----------------------------------------------------------------------------------------------
void Clock::TickSystemClock()
{
	if ( g_systemClock != nullptr )
	{
		g_systemClock->Tick();
	}
}


//-----------------------------------------------------------------------------------------------
void Clock::Tick()
{
	double currentTimeInSeconds = GetCurrentTimeSeconds();
	m_deltaSeconds = currentTimeInSeconds - m_lastUpdateTimeInSeconds;
	m_lastUpdateTimeInSeconds = currentTimeInSeconds;

	bool shouldStepSingleFrame = m_stepSingleFrame;
	if ( shouldStepSingleFrame )
	{
		m_stepSingleFrame = false;
	}

	if ( m_isPaused && !shouldStepSingleFrame )
	{
		m_deltaSeconds = 0.0;
	}
	else
	{
		if ( shouldStepSingleFrame )
		{
			m_isPaused = true;
		}
		m_deltaSeconds *= m_timeScale;
	}

	if ( m_deltaSeconds > m_maxDeltaSeconds )
	{
		m_deltaSeconds = m_maxDeltaSeconds;
	}
	m_totalSeconds += m_deltaSeconds;
	m_frameCount++;
	for ( Clock* childClock : m_children )
	{
		childClock->Advance( m_deltaSeconds );
	}
}


//-----------------------------------------------------------------------------------------------
void Clock::Advance( double deltaTimeSeconds )
{
	bool shouldStepSingleFrame = m_stepSingleFrame;
	if ( shouldStepSingleFrame )
	{
		m_stepSingleFrame = false;
	}

	if ( m_isPaused && !shouldStepSingleFrame )
	{
		deltaTimeSeconds = 0.0;
	}
	else
	{
		if ( shouldStepSingleFrame )
		{
			m_isPaused = true;
		}
		deltaTimeSeconds *= m_timeScale;
	}

	if ( deltaTimeSeconds > m_maxDeltaSeconds )
	{
		deltaTimeSeconds = m_maxDeltaSeconds;
	}
	m_deltaSeconds = deltaTimeSeconds;
	m_totalSeconds += m_deltaSeconds;
	m_frameCount++;
	for ( Clock* childClock : m_children )
	{
		childClock->Advance( m_deltaSeconds );
	}
}


//-----------------------------------------------------------------------------------------------
void Clock::AddChild( Clock* childClock )
{
	m_children.push_back( childClock );
}


//-----------------------------------------------------------------------------------------------
void Clock::RemoveChild( Clock* childClock )
{
	for ( size_t childIndex = 0; childIndex < m_children.size(); ++childIndex )
	{
		if ( m_children[childIndex] == childClock )
		{
			m_children[childIndex] = nullptr;
		}
	}
}