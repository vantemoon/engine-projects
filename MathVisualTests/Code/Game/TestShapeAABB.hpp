#pragma once
#include "Game/TestShape.hpp"
#include "Engine/Math/AABB2.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapeAABB : public TestShape
{
public:
	AABB2 m_alignedBox;

public:
	TestShapeAABB( AABB2 const& alignedBox );
	~TestShapeAABB();
};