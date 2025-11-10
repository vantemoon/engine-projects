#pragma once
#include "Game/Entity.hpp"
#include "Game/Bullet.hpp"


//-----------------------------------------------------------------------------------------------
class Aries : public Entity
{
public:
	Vec2	m_targetPosition = Vec2::ZERO;
	float   m_timeSinceLastTurn = 0.f;

public:
	Aries( Vec2 startingPosition, float orientationDegrees );
	~Aries();

	void Update( float deltaSeconds )                          override;
	void Render()                                        const override;
	void TakeDamage( int damage )                              override;
	void Die()                                                 override;
	void ReflectBullet( Bullet& bullet );

protected:
	void InitializeVertexArray()                               override;
	void MoveTowardTargetPosition( float deltaSeconds );
};