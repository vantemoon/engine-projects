#pragma once
#include "Game/TestShape.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/OBB3.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapeOBB3D : public TestShape
{
public:
	OBB3 m_orientedBox;

public:
	TestShapeOBB3D( Vec3 const& center, Vec3 const& iBasisNormal, Vec3 const& jBasisNormal, Vec3 const& halfDimensions );
	~TestShapeOBB3D();
};