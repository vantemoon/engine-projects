#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Entity; // Forward declaration
class Game;   // Forward declaration
struct Vec2;  // Forward declaration


//-----------------------------------------------------------------------------------------------
class PlayerShip : public Entity
{
public:
	static constexpr int NUM_SHIP_VERTS = 15;
	bool  m_isAccelerating = false;
	bool  m_isTurningLeft = false;
	bool  m_isTurningRight = false;
	float m_thrustFraction = 0.f;

public:
	PlayerShip( Game* game, Vec2 const& startingPosition, Vec2 const& startingVelocity );
	~PlayerShip() override;

	void Update( float deltaSeconds ) override;
	void UpdateFromKeyboard();
	void UpdateFromController();
	void Render() const override;
	void Die() override;
	void Respawn();
	void GetVertexArrayCopy( Vertex* out_vertexArray ) const;

private:
	void InitializeVertexArray() override;
	void RenderThrust() const;
	void BounceOffWorldEdges();
	void Accelerate( float deltaSeconds );
	void TurnLeft();
	void TurnRight();
};