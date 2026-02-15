#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"


//-----------------------------------------------------------------------------------------------
class  Asteroid;   // Forward declaration
class  Beetle;     // Forward declaration
class  Bullet;     // Forward declaration
class  Camera;     // Forward declaration
class  Clock;      // Forward declaration
class  Debris;     // Forward declaration
class  Entity;     // Forward declaration
class  ImpactWave; // Forward declaration
class  PlayerShip; // Forward declaration
class  Wasp;       // Forward declaration
struct Vec3;       // Forward declaration


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
	ImpactWave* m_impactWaves[MAX_IMPACT_WAVES] = {};
	Entity*     m_scanTargets[MAX_TARGETS] = {};

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
	bool        m_isTargetInitialized = false;
	int         m_currentSelectedEntityIndex = -1;

	bool        m_isDebugFeaturesOn = false;

	Clock*		m_gameClock = nullptr;

	bool        m_hasControlsBeenShown = false;

public:
	Game();
	~Game();

	void Update();
	void UpdateAttractMode();
	void UpdateScanMode();
	void UpdateEntities();
	void UpdateFromKeyboard();
	void UpdateFromController();
	void UpdateWaves();
	void ScreenShake( float intensity );
	bool IsReadyToStartNextWave()									  const;
	void StartNextWave();
	void Render()													  const;
	void RenderEntities()											  const;
	void RenderHUD()												  const;
	void RenderOffscreenIndicator()									  const;
	void RenderAttractMode()										  const;
	void RenderScanMode()											  const;
	void RenderDevConsole()											  const;
	void RenderParallaxBackground()									  const;
	Vec2 GetRandomOffscreenPosition( float cosmeticRadius )		      const;
	int  BuildScanTargets();
	int  GetEnemyClosestToPlayer()									  const;
	int  StepCurrentSelectedEntityIndex( int currentIndex, int step );
	void SpawnRandomAsteroids( int numOfAsteroids );
	void SpawnBulletFromPlayerShip();
	void SpawnRandomBeetles( int numOfBeetles );
	void SpawnRandomWasps( int numOfWasps );
	void SpawnDebrisCluster( int numOfDebris, Vec2 const& position, Vec2 momentum, Rgba8 const& color, float minRadius, float maxRadius);
	void SpawnImpactWave( Vec2 const& worldPosition, Rgba8 color = IMPACT_WAVE_COLOR );
	void CheckPlayerLives();
	int  FindFreeEntityIndex( Entity** entityList, int maxCount )     const;
	void Reset();
	void KillAllEnemies(); // For testing and debugging

private:
	void DeleteGarbageEntities();
	void DebugDraw()												   const;
	void InitializeTargersArray( Entity** out_targetsArray, int maxTargets );
	Vec3 TransformWorldToScreen( Vec3 const& worldPosition )		   const;
	bool IsOnScreen( Vec2 const& worldPosition, float cosmeticRadius ) const;
	void AddInstructionsToDevConsole();
	static bool Command_SetTimeScale( EventArgs& args );
};