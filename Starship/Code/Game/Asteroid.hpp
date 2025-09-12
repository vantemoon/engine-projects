#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Asteroid : public Entity
{
public:
	static constexpr int NUM_ASTEROID_VERTS = 48;

public:
	Asteroid(Game* game, Vec2 const& startingPosition, Vec2 const& startingVelocity, float startingAngularVelocity );
	~Asteroid() override;
	void Update(float deltaSeconds) override;
	void Render() const override;
	void Die() override;

	bool IsOffScreen() const override;
};