#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"


//-----------------------------------------------------------------------------------------------
class Asteroid;   // Forward declaration
class Beetle;     // Forward declaration
class Bullet;     // Forward declaration
class Camera;     // Forward declaration
class Debris;     // Forward declaration
class PlayerShip; // Forward declaration
class Wasp;       // Forward declaration


//-----------------------------------------------------------------------------------------------
class Game
{
public:
	PlayerShip* m_playerShip = nullptr;
	Asteroid*   m_asteroids[MAX_ASTEROIDS] = {};
	Bullet*     m_bullets[MAX_BULLETS] = {};
	Beetle*     m_beetles[MAX_BEETLES] = {};
	Wasp*       m_wasps[MAX_WASPS] = {};
	Debris*     m_debris[MAX_DEBRIS] = {};

	Camera*     m_worldCamera = nullptr;
	Camera*     m_screenCamera = nullptr;

	int         m_waveNumber = 0;
	int			m_playerSpareLives = NUM_PLAYER_LIVES - 1;

	bool        m_isBackgroundMusicPlaying = false;
	SoundID     m_backgroundMusicSoundID = 0;

	bool        m_isScreenShaking = false;
	float	    m_screenShakeIntensity = 0.f;
	float       m_screenShakeDuration = 0.f;
	float       m_screenShakeStartTime = 0.f;

	GameState   m_currentGameState;

	bool        m_isScanModeOn = false;

	bool        m_isDebugFeaturesOn = false;
	bool        m_isPausedAfterNextUpdate = false;

public:
	Game();
	~Game();

	void Update( float deltaSeconds);
	void UpdateAttractMode( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void UpdateFromKeyboard();
	void UpdateFromController();
	void UpdateWaves();
	void ScreenShake( float intensity );
	bool IsReadyToStartNextWave() const;
	void StartNextWave();
	void Render() const;
	void RenderEntities() const;
	void RenderHUD() const;
	void RenderAttractMode() const;
	void RenderParallaxBackground() const;
	Vec2 GetRandomOffscreenPosition( float cosmeticRadius ) const;
	void SpawnRandomAsteroids( int numOfAsteroids );
	void SpawnBulletFromPlayerShip();
	void SpawnRandomBeetles( int numOfBeetles );
	void SpawnRandomWasps( int numOfWasps );
	void SpawnDebrisCluster( int numOfDebris, Vec2 const& position, Vec2 momentum, Rgba8 const& color, float minRadius, float maxRadius);
	void CheckPlayerLives();
	void Reset();
	void KillAllEnemies(); // For testing and debugging

private:
	void DeleteGarbageEntities();
	void DebugDraw() const;
};