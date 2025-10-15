#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Asteroid : public Entity
{
public:
	static constexpr int NUM_ASTEROID_VERTS = 48;
	float m_sizeScale = 1;

public:
	Asteroid( Game* game, Vec2 const& startingPosition, float orientationDegrees, Vec2 const& startingVelocity, float startingAngularVelocity, float sizeScale );
	~Asteroid() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;
	void Die() override;

private:
	void InitializeVertexArray() override;
	void WrapAroundScreen();
};