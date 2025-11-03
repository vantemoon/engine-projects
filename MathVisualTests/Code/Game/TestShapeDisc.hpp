#pragma once
#include "Game/TestShape.hpp"
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapeDisc : public TestShape
{
public:
	Vec2	m_center;
	float	m_radius;
	int		m_numSides;

public:
	TestShapeDisc( Vec2 const& center, float radius, int numSides );
	~TestShapeDisc();
};