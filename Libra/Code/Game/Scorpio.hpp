#pragma once
#include "Game/Entity.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class Scorpio : public Entity
{
public:
	float 				 m_targetOrientationDegrees = 0.f;
	float 				 m_timeSinceLastFire = 0.f;
	bool                 m_wasInPersuitLastFrame = false;

	std::vector<Vertex>  m_scorpioBaseVertexArray;
	std::vector<Vertex>  m_scorpioTopVertexArray;

public:
	Scorpio( Vec2 startingPosition, float orientationDegrees );
	~Scorpio();

	void Update( float deltaSeconds )                          override;
	void Render()                                        const override;
	void RenderHealthBar()                               const;
	void TakeDamage( int damage )                              override;
	void Die()                                                 override;

protected:
	void InitializeVertexArray()                               override;
	void TurnTowardTargetOrientation( float deltaSeconds );
	void FireProjectile();
};