#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Beetle : public Entity
{
public:
	Beetle( Game* game, Vec2 const& startingPosition );
	~Beetle()								  override;
	void Update( float deltaSeconds )		  override;
	void Render()						const override;
	void Die()								  override;

private:
	void InitializeVertexArray()			  override;
};