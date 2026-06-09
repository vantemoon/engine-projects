#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Board.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Rgba8.hpp"


//-----------------------------------------------------------------------------------------------
class  Camera;
class  Entity;
struct Vec3;


//-----------------------------------------------------------------------------------------------
class Game
{
public:
	Camera*     m_worldCamera = nullptr;
	Camera*     m_screenCamera = nullptr;
	Vec2        m_worldCameraPosition;

	bool        m_isScreenShaking = false;
	float	    m_screenShakeIntensity = 0.f;
	float       m_screenShakeDuration = 0.f;
	float       m_screenShakeStartTime = 0.f;

	GameState   m_currentGameState;

	bool        m_isDebugFeaturesOn = false;

	Clock*		m_gameClock = nullptr;

	Board*		m_board = nullptr;

public:
	Game();
	~Game();

	void Update();
	void UpdateCamera();
	void UpdateAttractMode();
	void UpdateFromKeyboard();
	void UpdateFromController();
	void ScreenShake( float intensity );
	void Render()													  const;
	void RenderHUD()												  const;
	void RenderAttractMode()										  const;
	void RenderDevConsole()											  const;
	void Reset();

private:
	void DeleteGarbageEntities();
	void DebugDraw()												   const;
	Vec3 TransformWorldToScreen( Vec3 const& worldPosition )		   const;
};