#include "Game/TestShapeCapsule.hpp"


//-----------------------------------------------------------------------------------------------
TestShapeCapsule::TestShapeCapsule( Vec2 const& boneStart, Vec2 const& boneEnd, float radius, int numSides, float elasticity )
	: m_boneStart( boneStart )
	, m_boneEnd( boneEnd )
	, m_radius( radius )
	, m_numSides( numSides )
	, m_elasticity( elasticity )
{
}


//-----------------------------------------------------------------------------------------------
TestShapeCapsule::~TestShapeCapsule()
{
}