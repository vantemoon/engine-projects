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
	Asteroid* m_asteroids[MAX_ASTEROIDS] = {};
	Bullet* m_bullets[MAX_BULLETS] = {};
	Camera* m_gameCamera = nullptr;

public:
	Game();
	~Game();

	void Update( float deltaSeconds);
	void UpdateEntities( float deltaSeconds );
	void Render() const;
	void RenderEntities() const;
	void DeleteGarbageEntities();
	void SpawnRandomAsteroid();
	void SpawnBulletFromPlayerShip();
	void DebugDraw() const;
};