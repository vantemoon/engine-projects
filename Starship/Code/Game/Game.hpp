#pragma once
#include "Game/GameCommon.hpp"


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
	Camera*     m_gameCamera = nullptr;
	Camera*     m_attractCamera = nullptr;

	bool        m_isAttractMode = true;
	int         m_waveNumber = 0;
	constexpr static int NUM_OF_WAVES = 5;

public:
	Game();
	~Game();

	void Update( float deltaSeconds);
	void UpdateAttractMode( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void UpdateFromKeyboard();
	void UpdateWaves();
	bool IsReadyToStartNextWave() const;
	void StartNextWave();
	void Render() const;
	void RenderEntities() const;
	void RenderAttractMode() const;
	Vec2 GetRandomOffscreenPosition( float cosmeticRadius ) const;
	void SpawnRandomAsteroids( int numOfAsteroids );
	void SpawnBulletFromPlayerShip();
	void SpawnRandomBeetles( int numOfBeetles );
	void SpawnRandomWasps( int numOfWasps );
	void SpawnDebrisCluster( int numOfDebris, Vec2 const& position, Vec2 momentum, Rgba8 const& color, float minRadius, float maxRadius);
	void KillAllEnemies(); // For debugging

private:
	void DeleteGarbageEntities();
	void DebugDraw() const;
};