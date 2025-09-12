#pragma once


//-----------------------------------------------------------------------------------------------
class Asteroid;   // Forward declaration
class Bullet;     // Forward declaration
class Camera;     // Forward declaration
class PlayerShip; // Forward declaration


//-----------------------------------------------------------------------------------------------
class Game
{
public:
	static constexpr int MAX_BULLETS = 20;
	static constexpr int MAX_ASTEROIDS = 12;
	PlayerShip* m_playerShip = nullptr;		// Just one player ship (for now...)
	Asteroid* m_asteroids[MAX_ASTEROIDS] = {};	// Fixed number of asteroid “slots”; nullptr if unused.
	Bullet* m_bullets[MAX_BULLETS] = {};	// The “= {};” syntax initializes the array to zeros.
	Camera* m_gameCamera = nullptr;

public:
	Game();
	~Game();

	void Update( float deltaSeconds);
	void Render() const;
	void DeleteGarbageEntities();
	void SpawnRandomAsteroid();
	void SpawnBulletFromPlayerShip();
};