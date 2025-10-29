#pragma once
#include "Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Player : public Entity
{
public:
	bool				 m_isMovingForward = false;
	float    			 m_targetMovementDirection = 0.f;
	float				 m_thrustFraction = 0.f;
	float				 m_turretOrientationDegrees = 0.f;
	float				 m_turretRelativeDegrees = 0.f;
	float				 m_turretTargetDegrees = 0.f;
	bool				 m_isTurretAiming = false;

	std::vector<Vertex>  m_tankBodyVertexArray;
	std::vector<Vertex>  m_turretVertexArray;

public:
	Player( Vec2 startingPosition );
	~Player();

	void Update( float deltaSeconds )							override;
	void UpdateFromKeyboard( float deltaSeconds );
	void UpdateFromController( float deltaSeconds );
	void Render()									      const override;
	void TakeDamage( int damage )								override;
	void Die()													override;

protected:
	void InitializeVertexArray()								override;
	void TurnTowardMovementDirection( float deltaSeconds );
	void TurnTurretTowardAimDirection( float deltaSeconds );
};