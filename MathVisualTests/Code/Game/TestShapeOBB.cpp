#include "Game/TestShapeOBB.hpp"


//-----------------------------------------------------------------------------------------------
TestShapeOBB::TestShapeOBB( Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions, float elasticity )
	: m_orientedBox( center, iBasisNormal, halfDimensions )
	, m_elasticity( elasticity )
{
	m_boundingDiscCenter = center;
	m_boundingDiscRadius = halfDimensions.GetLength();
}


//-----------------------------------------------------------------------------------------------
TestShapeOBB::~TestShapeOBB()
{
}