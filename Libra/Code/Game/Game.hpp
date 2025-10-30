#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Rgba8.hpp"


//-----------------------------------------------------------------------------------------------
class  Camera;     // Forward declaration
class  Entity;     // Forward declaration
struct Vec3;       // Forward declaration


//-----------------------------------------------------------------------------------------------
extern Game*	g_game;

//-----------------------------------------------------------------------------------------------
class Game
{
public:
	Player*		m_player = nullptr;
	Map*		m_currentMap = nullptr;

	Camera*     m_worldCamera = nullptr;
	Camera*     m_screenCamera = nullptr;
	int			m_numTilesInViewVertically;
	int 	    m_numTilesInViewHorizontally;

	bool        m_isBackgroundMusicPlaying = false;
	SoundID     m_backgroundMusicSoundID = 0;

	bool        m_isScreenShaking = false;
	float	    m_screenShakeIntensity = 0.f;
	float       m_screenShakeDuration = 0.f;
	float       m_screenShakeStartTime = 0.f;

	GameState   m_currentGameState;

	bool        m_isDebugFeaturesOn = false;
	bool        m_isPausedAfterNextUpdate = false;

public:
	Game();
	~Game();

	void Update( float deltaSeconds);
	void UpdateAttractMode( float deltaSeconds );
	void UpdateFromKeyboard();
	void UpdateFromController();
	void ScreenShake( float intensity );
	void Render()													  const;
	void RenderHUD()												  const;
	void RenderAttractMode()										  const;
	void Reset();
	void KillAllEnemies(); // For testing and debugging
};