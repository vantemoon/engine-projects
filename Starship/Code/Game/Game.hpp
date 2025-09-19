#pragma once
#include "Game/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
class Asteroid;   // Forward declaration
class Beetle;     // Forward declaration
class Bullet;     // Forward declaration
class Camera;     // Forward declaration
class PlayerShip; // Forward declaration


//-----------------------------------------------------------------------------------------------
class Game
{
public:
	PlayerShip* m_playerShip = nullptr;
	Asteroid*   m_asteroids[MAX_ASTEROIDS] = {};
	Bullet*     m_bullets[MAX_BULLETS] = {};
	Beetle*     m_beetles[2] = {};
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
	void RenderAttractMode() const;
	void SpawnRandomAsteroid( int numOfAsteroid );
	void SpawnBulletFromPlayerShip();
	void SpawnRandomBeetle();

private:
	void RenderEntities() const;
	void DeleteGarbageEntities();
	void DebugDraw() const;
};