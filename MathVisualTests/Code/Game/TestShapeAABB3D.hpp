#pragma once
#include "Game/TestShape.hpp"
#include "Engine/Math/AABB3.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapeAABB3D : public TestShape
{
public:
	AABB3 m_alignedBox;

public:
	TestShapeAABB3D( AABB3 const& alignedBox );
	~TestShapeAABB3D();
};