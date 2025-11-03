#include "Game/TestShapeOBB.hpp"


//-----------------------------------------------------------------------------------------------
TestShapeOBB::TestShapeOBB( Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions )
	: m_orientedBox( center, iBasisNormal, halfDimensions )
{
}


//-----------------------------------------------------------------------------------------------
TestShapeOBB::~TestShapeOBB()
{
}