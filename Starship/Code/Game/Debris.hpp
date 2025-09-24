#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Debris : public Entity
{
private:
	constexpr static int MIN_NUM_OF_SIDES = 3;
	constexpr static int MAX_NUM_OF_SIDES = 7;
	int m_numOfSides;
	int m_numOfVerts;
	float m_minRadius = 0.5f;
	float m_maxRadius = 2.f;
	float m_ageSeconds = 0.f;
	Rgba8 m_color;

public:
	Debris( Game* game, Vec2 const& startingPosition, Vec2 const& startingVelocity, float startingAngularVelocity, Rgba8 const& color, float minRadius, float maxRadius );
	~Debris() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;

private:
	void InitializeVertexArray() override;
};