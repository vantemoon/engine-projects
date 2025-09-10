#pragma once
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"

//-----------------------------------------------------------------------------------------------
class PlayerShip : public Entity
{
public:
	PlayerShip(Game* game, Vec2 const& startingPosition, Vec2 const& startingVelocity);
	~PlayerShip() override;
	void Update(float deltaSeconds) override;
	void Render() const override;
	void Die() override;
};