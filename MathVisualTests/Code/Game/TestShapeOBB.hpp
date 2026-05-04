#pragma once
#include "Game/TestShape.hpp"
#include "Engine/Math/OBB2.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapeOBB : public TestShape
{
public:
	OBB2 m_orientedBox;

	float m_elasticity = 0.5f;

public:
	TestShapeOBB( Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions, float elasticity = 0.5f );
	~TestShapeOBB();
};