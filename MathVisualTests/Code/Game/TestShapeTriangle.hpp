#pragma once
#include "Game/TestShape.hpp"
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapeTriangle : public TestShape
{
public:
	Vec2 m_ccw0;
	Vec2 m_ccw1;
	Vec2 m_ccw2;

public:
	TestShapeTriangle( Vec2 const& ccw0, Vec2 const& ccw1, Vec2 const& ccw2 );
	~TestShapeTriangle();
};