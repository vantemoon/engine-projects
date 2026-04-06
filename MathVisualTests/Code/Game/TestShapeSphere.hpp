#pragma once
#include "Game/TestShape.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapeSphere : public TestShape
{
public:
	Vec3 m_center;
	float m_radius;

public:
	TestShapeSphere( Vec3 const& center, float radius );
	~TestShapeSphere();
};