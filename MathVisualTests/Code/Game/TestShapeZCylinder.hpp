#pragma once
#include "Game/TestShape.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapeZCylinder : public TestShape
{
public:
	Vec3 m_start;
	Vec3 m_end;
	float m_radius;

public:
	TestShapeZCylinder( Vec3 const& start, Vec3 const& end, float radius );
	~TestShapeZCylinder();
};