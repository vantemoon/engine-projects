#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Asteroid : public Entity
{
public:
	static constexpr int NUM_ASTEROID_VERTS = 48;

public:
	Asteroid( Game* game, Vec2 const& startingPosition, float orientationDegrees, Vec2 const& startingVelocity, float startingAngularVelocity );
	~Asteroid() override;
	void InitializeVertexArray() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;

private:
	void CheckCollisionWithBullets();
	void CheckCollisionWithPlayerShip();
	void WrapAroundScreen();
};