#pragma once
#include "Game/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
class Asteroid;   // Forward declaration
class Beetle;     // Forward declaration
class Bullet;     // Forward declaration
class Camera;     // Forward declaration
class PlayerShip; // Forward declaration
class Wasp;       // Forward declaration


//-----------------------------------------------------------------------------------------------
class Game
{
public:
	PlayerShip* m_playerShip = nullptr;
	Asteroid*   m_asteroids[MAX_ASTEROIDS] = {};
	Bullet*     m_bullets[MAX_BULLETS] = {};
	Beetle*     m_beetles[MAX_BEETLE] = {};
	Wasp*       m_wasps[MAX_WASP] = {};
	Camera*     m_gameCamera = nullptr;
	Camera*     m_attractCamera = nullptr;

	bool        m_isAttractMode = true;
	int         m_numOfWaves = 5;

public:
	Game();
	~Game();

	void Update( float deltaSeconds);
	void UpdateAttractMode( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void Render() const;
	void RenderEntities() const;
	void RenderAttractMode() const;
	Vec2 GetRandomOffscreenPosition( float cosmeticRadius ) const;
	void SpawnRandomAsteroid( int numOfAsteroid );
	void SpawnBulletFromPlayerShip();
	void SpawnRandomBeetle();
	void SpawnRandomWasp();

private:
	void DeleteGarbageEntities();
	void DebugDraw() const;
};