#pragma once
#include "Game/TestShape.hpp"
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapeCapsule : public TestShape
{
public:
	Vec2	m_boneStart;
	Vec2	m_boneEnd;
	float	m_radius;
	int		m_numSides;

	float   m_elasticity = 0.5f;

public:
	TestShapeCapsule( Vec2 const& boneStart, Vec2 const& boneEnd, float radius, int numSides, float elasticity = 0.5f );
	~TestShapeCapsule();
};