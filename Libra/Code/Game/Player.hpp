#pragma once
#include "Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Player : public Entity
{
public:
	Player( Game* game, Vec2 startingPosition );
	virtual ~Player();
	virtual void Update( float deltaSeconds ) override;
	virtual void Render()												const override;
	virtual void TakeDamage( int damage )								override;
	virtual void Die()													override;

protected:
	virtual void InitializeVertexArray()								override;
};