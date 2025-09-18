#pragma once
#include "Game/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
class Asteroid;   // Forward declaration
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
	Camera*     m_gameCamera = nullptr;
	int         m_numOfWaves = 5;

public:
	Game();
	~Game();

	void Update( float deltaSeconds);
	void UpdateEntities( float deltaSeconds );
	void Render() const;
	void SpawnRandomAsteroid( int numOfAsteroid );
	void SpawnBulletFromPlayerShip();

private:
	void RenderEntities() const;
	void DeleteGarbageEntities();
	void DebugDraw() const;
};