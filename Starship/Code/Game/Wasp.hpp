#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Wasp : public Entity
{
public:
	Wasp( Game* game, Vec2 const& startingPosition );
	~Wasp()															  override;
	void Update()													  override;
	void Render()												const override;
	void Die()														  override;

private:
	void InitializeVertexArray()									  override;
	void Accelerate();
	void WrapAroundScreen();
};