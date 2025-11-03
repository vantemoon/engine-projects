#pragma once
#include "Game/TestShape.hpp"
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapeLineSegment : public TestShape
{
public:
	Vec2	m_start;
	Vec2	m_end;
	float	m_thickness;
	bool    m_isInfinite;

public:
	TestShapeLineSegment( Vec2 const& start, Vec2 const& end, float thickness, bool isInfinite = false );
	~TestShapeLineSegment();
};