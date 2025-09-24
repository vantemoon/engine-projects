#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Wasp : public Entity
{
public:
	static constexpr int NUM_WASP_VERTS = 6;

public:
	Wasp( Game* game, Vec2 const& startingPosition );
	~Wasp() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;
	void Die() override;

private:
	void InitializeVertexArray() override;
	void Accelerate( float deltaSeconds );
};