#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Asteroid : public Entity
{
public:
	float m_sizeScale = 1;

public:
	Asteroid( Game* game, Vec2 const& startingPosition, float orientationDegrees, Vec2 const& startingVelocity, float startingAngularVelocity, float sizeScale );
	~Asteroid()								  override;
	void Update()							  override;
	void Render()						const override;
	void Die()								  override;

private:
	void InitializeVertexArray()			  override;
	void WrapAroundScreen();
};