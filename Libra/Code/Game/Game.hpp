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
class  Texture;     // Forward declaration
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
	SoundPlaybackID		m_attractMusicPlaybackID;
	bool				m_isAttractMusicPlaying = false;

	SoundID				m_gameplayMusicID;
	SoundPlaybackID		m_gameplayMusicPlaybackID;
	bool 				m_isGameplayMusicPlaying = false;

	SoundID             m_startGameSoundID;
	SoundID             m_pauseSoundID;
	SoundID             m_unpauseSoundID;
	SoundID             m_playerShootSoundID;
	SoundID             m_enemyShootSoundID;
	SoundID             m_playerHitSoundID;
	SoundID             m_enemyHitSoundID;
	SoundID             m_playerDeathSoundID;
	SoundID             m_enemyDeathSoundID;
	SoundID             m_nextLevelSoundID;
	SoundID             m_gameOverSoundID;
	SoundID             m_victorySoundID;
	SoundID             m_discoverySoundID;

	Texture*			m_attractModeBackgroundTexture = nullptr;
	Texture*			m_victoryScreenTexture = nullptr;
	Texture*			m_gameOverScreenTexture = nullptr;
	Texture*			m_ariesTexture = nullptr;
	Texture*			m_leoTexture = nullptr;
	Texture*			m_scorpioBaseTexture = nullptr;
	Texture*			m_scorpioTurretTexture = nullptr;
	Texture*			m_playerTankBaseTexture = nullptr;
	Texture*			m_playerTankTurretTexture = nullptr;
	Texture*			m_goodBulletTexture = nullptr;
	Texture*			m_goodBoltTexture = nullptr;
	Texture*			m_evilBulletTexture = nullptr;
	Texture*			m_evilBoltTexture = nullptr;
	Texture*			m_explosionTexture = nullptr;

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
	int                 m_mapRenderMode = 0;

	bool				m_isSlowMo = false;
	bool				m_isFastMo = false;

	SpriteSheet*		m_tileSpriteSheet = nullptr;

public:
	Game();
	~Game();

	void Update( float deltaSeconds);
	void UpdateAttractMode( float deltaSeconds );
	void UpdatePausedMode( float deltaSeconds );
	void UpdateVicoryMode( float deltaSeconds );
	void UpdateGameOverMode( float deltaSeconds );
	void UpdateFromKeyboard();
	void UpdateFromController();
	void UpdateMusic();
	void ScreenShake( float intensity );
	void LoadTextures();
	void LoadSounds();
	void Render()													  const;
	void RenderHUD()												  const;
	void RenderDevConsole()											  const;
	void RenderAttractMode()										  const;
	void RenderPausedMode()									          const;
	void RenderVictoryMode()										  const;
	void RenderGameOverMode()								          const;
	void Reset();
	void KillAllEnemies();
	void LoadNextMap();
};