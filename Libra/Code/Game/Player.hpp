#pragma once
#include "Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Player : public Entity
{
public:
	bool				 m_isMovingForward = false;
	float    			 m_targetMovementDirection = 0.f;
	float                m_prevOrientationDegrees = 0.f;
	float				 m_thrustFraction = 0.f;

	float				 m_turretOrientationDegrees = 0.f;
	float				 m_turretRelativeDegrees = 0.f;
	float				 m_turretTargetDegrees = 0.f;
	bool				 m_isTurretAiming = false;

	bool                 m_isInvincible = false;
	bool                 m_noClip = false;

	std::vector<Vertex>  m_tankBodyVertexArray;
	std::vector<Vertex>  m_turretVertexArray;

	float 				 m_timeSinceLastFire = 0.f;

public:
	Player( Vec2 startingPosition, float orientationDegrees );
	~Player();

	void Update( float deltaSeconds )							override;
	void UpdatePhysics( float deltaSeconds );
	void UpdateFromKeyboard( float deltaSeconds );
	void UpdateFromController( float deltaSeconds );
	void Render()									      const override;
	void TakeDamage( int damage )								override;
	void Die()													override;

protected:
	void InitializeVertexArray()								override;
	void TurnTowardMovementDirection( float deltaSeconds );
	void TurnTurretTowardAimDirection( float deltaSeconds );
	void FireProjectile();
};