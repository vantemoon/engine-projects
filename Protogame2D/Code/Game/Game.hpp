#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
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

public:
	Game();
	~Game();

	void Update( float deltaSeconds);
	void UpdateAttractMode( float deltaSeconds );
	// void UpdateScanMode( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void UpdateFromKeyboard();
	void UpdateFromController();
	// void UpdateWaves();
	void ScreenShake( float intensity );
	// bool IsReadyToStartNextWave()									  const;
	// void StartNextWave();
	void Render()													  const;
	void RenderEntities()											  const;
	void RenderHUD()												  const;
	// void RenderOffscreenIndicator()									  const;
	void RenderAttractMode()										  const;
	// void RenderScanMode()											  const;
	// void RenderParallaxBackground()									  const;
	Vec2 GetRandomOffscreenPosition( float cosmeticRadius )		      const;
	// int  BuildScanTargets();
	// int  GetEnemyClosestToPlayer()									  const;
	// int  StepCurrentSelectedEntityIndex( int currentIndex, int step );
	// void SpawnRandomAsteroids( int numOfAsteroids );
	// void SpawnBulletFromPlayerShip();
	// void SpawnRandomBeetles( int numOfBeetles );
	// void SpawnRandomWasps( int numOfWasps );
	// void SpawnDebrisCluster( int numOfDebris, Vec2 const& position, Vec2 momentum, Rgba8 const& color, float minRadius, float maxRadius);
	// void SpawnImpactWave( Vec2 const& worldPosition, Rgba8 color = IMPACT_WAVE_COLOR );
	// void CheckPlayerLives();
	// int  FindFreeEntityIndex( Entity** entityList, int maxCount )     const;
	void Reset();
	void KillAllEnemies(); // For testing and debugging

private:
	void DeleteGarbageEntities();
	void DebugDraw()												   const;
	// void InitializeTargersArray( Entity** out_targetsArray, int maxTargets );
	Vec3 TransformWorldToScreen( Vec3 const& worldPosition )		   const;
	bool IsOnScreen( Vec2 const& worldPosition, float cosmeticRadius ) const;
};