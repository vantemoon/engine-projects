#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Beetle : public Entity
{
public:
	static constexpr int NUM_BEETLE_VERTS = 18;

public:
	Beetle( Game* game, Vec2 const& startingPosition, float orientationDegrees, Vec2 const& startingVelocity, float startingAngularVelocity );
	~Beetle() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;

private:
	void InitializeVertexArray() override;
};