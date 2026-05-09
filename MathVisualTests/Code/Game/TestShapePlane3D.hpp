#pragma once
#include "Game/TestShape.hpp"
#include "Engine/Math/Plane3.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapePlane3D : public TestShape
{
public:
	Plane3 m_plane;

public:
	TestShapePlane3D( Plane3 const& plane );
	~TestShapePlane3D();
};