#pragma once


//-----------------------------------------------------------------------------------------------
class  Camera;     // Forward declaration
class  Clock;      // Forward declaration


//-----------------------------------------------------------------------------------------------
class Game
{
public:
	Camera*     m_worldCamera = nullptr;
	Camera*     m_screenCamera = nullptr;

	Clock*		m_gameClock = nullptr;

public:
	Game();
	~Game();

	virtual void Update( float deltaSeconds);
	virtual void Render() const;
};