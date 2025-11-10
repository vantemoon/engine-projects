#pragma once
#include "Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Bullet : public Entity
{
public:
	EntityType	m_type;
	bool		m_isBullet = true;
	bool		m_isBolt = false;

public:
	Bullet( Vec2 startingPosition, float orientationDegrees, EntityType type );
	~Bullet() override;

	void Update( float deltaSeconds )							override;
	void UpdatePhysics( float deltaSeconds );
	void Render()									      const override;
	void TakeDamage( int damage )								override;
	void Die()													override;

protected:
	void InitializeVertexArray()								override;
	void ResolveCollision();
	EntityFaction GetOppositeFaction() const;
};