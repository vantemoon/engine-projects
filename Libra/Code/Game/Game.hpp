#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Rgba8.hpp"


//-----------------------------------------------------------------------------------------------
class  Camera;      // Forward declaration
class  Entity;      // Forward declaration
class  SpriteSheet; // Forward declaration
struct Vec3;        // Forward declaration


//-----------------------------------------------------------------------------------------------
extern Game*	g_game;

//-----------------------------------------------------------------------------------------------
class Game
{
public:
	Player*				m_player = nullptr;
	Map*				m_currentMap = nullptr;
	std::vector<Map*>	m_maps;

	Camera*				m_worldCamera = nullptr;
	Camera*				m_screenCamera = nullptr;
	Camera*				m_debugCamera = nullptr;
	int					m_numTilesInViewVertically;
	int 				m_numTilesInViewHorizontally;

	SoundID				m_attractMusicID;
	SoundID				m_gameplayMusicID;
	SoundPlaybackID		m_attractMusicPlaybackID;
	SoundPlaybackID		m_gameplayMusicPlaybackID;
	bool				m_isAttractMusicPlaying = false;
	bool 				m_isGameplayMusicPlaying = false;

	bool				m_isScreenShaking = false;
	float				m_screenShakeIntensity = 0.f;
	float				m_screenShakeDuration = 0.f;
	float				m_screenShakeStartTime = 0.f;

	GameState			m_currentGameState;
	float				m_timeSincePlayerDeath = 0.f;
	bool				m_playerDeathHandled = false;

	bool				m_isDebugOn = false;
	bool				m_isDebugCameraActive = false;
	bool				m_isPausedAfterNextUpdate = false;

	bool				m_isSlowMo = false;
	bool				m_isFastMo = false;

	SpriteSheet*		m_tileSpriteSheet = nullptr;

public:
	Game();
	~Game();

	void Update( float deltaSeconds);
	void UpdateAttractMode( float deltaSeconds );
	void UpdateFromKeyboard();
	void UpdateFromController();
	void UpdateMusic();
	void ScreenShake( float intensity );
	void LoadTextures();
	void Render()													  const;
	void RenderHUD()												  const;
	void RenderAttractMode()										  const;
	void RenderPauseScreenOverlay()									  const;
	void RenderGameOverScreenOverlay()								  const;
	void Reset();
	void KillAllEnemies(); // For testing and debugging
};