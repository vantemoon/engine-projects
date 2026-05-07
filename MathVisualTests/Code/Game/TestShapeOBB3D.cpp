#include "Game/TestShapeOBB3D.hpp"


//-----------------------------------------------------------------------------------------------
TestShapeOBB3D::TestShapeOBB3D( Vec3 const& center, Vec3 const& iBasisNormal, Vec3 const& jBasisNormal, Vec3 const& halfDimensions )
	: m_orientedBox( center, iBasisNormal, jBasisNormal, halfDimensions )
{
}


//-----------------------------------------------------------------------------------------------
TestShapeOBB3D::~TestShapeOBB3D()
{
}