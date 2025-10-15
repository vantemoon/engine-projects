#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
class Game; // Forward declaration
struct Vertex; // Forward declaration


//-----------------------------------------------------------------------------------------------
class Entity
{
public:
	Vec2		m_position;
	Vec2		m_velocity;
	Vertex*		m_vertexArray = nullptr;
	float		m_orientationDegrees;
	float		m_angularVelocityDegreesPerSecond;
	float		m_physicsRadius;
	float		m_cosmeticRadius;
	Rgba8		m_color;
	int			m_health;
	bool		m_isDead = false;
	bool		m_isGarbage = false;
	Game*		m_game = nullptr;

	bool        m_isAsteroid;
	bool        m_isBeetle;
	bool        m_isWasp;

public:
	Entity( Game* game, Vec2 startingPosition );
	virtual ~Entity();
	virtual void Update( float deltaSeconds );
	virtual void Render() const;
	virtual void TakeDamage( int damage );
	virtual void Die();

	// Accessors
	virtual bool IsOffScreen() const;
	virtual Vec2 GetForwardNormal() const;
	bool         IsAlive() const;
	void         GetEnemyTypeAndAction( char** out_type, char** out_action ) const;

protected:
	virtual void InitializeVertexArray();
	virtual void CheckCollisionWithBullets();
	virtual void CheckCollisionWithPlayerShip();
};