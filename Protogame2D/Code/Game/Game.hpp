#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Rgba8.hpp"


//-----------------------------------------------------------------------------------------------
class  Camera;     // Forward declaration
class  Entity;     // Forward declaration
struct Vec3;       // Forward declaration


//-----------------------------------------------------------------------------------------------
class Game
{
public:
	Camera*     m_worldCamera = nullptr;
	Camera*     m_screenCamera = nullptr;

	bool        m_isBackgroundMusicPlaying = false;
	SoundID     m_backgroundMusicSoundID = 0;

	bool        m_isScreenShaking = false;
	float	    m_screenShakeIntensity = 0.f;
	float       m_screenShakeDuration = 0.f;
	float       m_screenShakeStartTime = 0.f;

	GameState   m_currentGameState;

	bool        m_isDebugFeaturesOn = false;
	bool        m_isPausedAfterNextUpdate = false;

	Clock*		m_gameClock = nullptr;

public:
	Game();
	~Game();

	void Update();
	void UpdateAttractMode();
	void UpdateEntities();
	void UpdateFromKeyboard();
	void UpdateFromController();
	void ScreenShake( float intensity );
	void Render()													  const;
	void RenderEntities()											  const;
	void RenderHUD()												  const;
	void RenderAttractMode()										  const;
	void RenderDevConsole()											  const;
	Vec2 GetRandomOffscreenPosition( float cosmeticRadius )		      const;
	void Reset();
	void KillAllEnemies(); // For testing and debugging

private:
	void DeleteGarbageEntities();
	void DebugDraw()												   const;
	Vec3 TransformWorldToScreen( Vec3 const& worldPosition )		   const;
	bool IsOnScreen( Vec2 const& worldPosition, float cosmeticRadius ) const;
};