#pragma once
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
	int			m_health;
	bool		m_isDead = false;
	bool		m_isGarbage = false;
	Game*		m_game = nullptr;

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
	bool IsAlive() const;

private:
	virtual void InitializeVertexArray();
};