#pragma once
#include "Game/TestShape.hpp"
#include "Engine/Math/OBB2.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapeOBB : public TestShape
{
public:
	OBB2 m_orientedBox;

public:
	TestShapeOBB( Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions );
	~TestShapeOBB();
};