#include "Game/TestShapeDisc.hpp"


//-----------------------------------------------------------------------------------------------
TestShapeDisc::TestShapeDisc( Vec2 const& center, float radius, int numSides, float elasticity )
	: m_center( center )
	, m_radius( radius )
	, m_numSides( numSides )
	, m_elasticity( elasticity )
{
}


//-----------------------------------------------------------------------------------------------
TestShapeDisc::~TestShapeDisc()
{
}