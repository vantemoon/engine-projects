#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Leo : public Entity
{
public:
	Vec2	m_targetPosition = Vec2::ZERO;
	float	m_timeSinceLastFire = 0.f;
	float   m_timeSinceLastTurn = 0.f;

public:
	Leo( Vec2 startingPosition, float orientationDegrees );
	~Leo();

	void Update( float deltaSeconds )                          override;
	void Render()                                        const override;
	void TakeDamage( int damage )                              override;
	void Die()                                                 override;

protected:
	void InitializeVertexArray()                               override;
	void MoveTowardTargetPosition( float deltaSeconds );
	void FireProjectile();
};