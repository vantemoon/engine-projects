#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Game;       // Forward declaration
class PlayerShip; // Forward declaration
struct Vec2;      // Forward declaration


//-----------------------------------------------------------------------------------------------
class Bullet : public Entity
{
public:
	PlayerShip* m_shooter = nullptr;
	float m_ageSeconds = 0.f;
	bool m_hasHitTarget = false;
	static constexpr int NUM_BULLET_VERTS = 6;

public:
	Bullet( Game* game, PlayerShip* shooter );
	~Bullet() override;

	void InitializeVertexArray() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;
	void Die( bool shouldSpawnDebris );
};