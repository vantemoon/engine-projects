#include "Game/TestShapeDisc.hpp"


//-----------------------------------------------------------------------------------------------
TestShapeDisc::TestShapeDisc( Vec2 const& center, float radius, int numSides, float elasticity, Vec2 const& velocity )
	: m_center( center )
	, m_radius( radius )
	, m_numSides( numSides )
	, m_elasticity( elasticity )
	, m_velocity( velocity )
{
}


//-----------------------------------------------------------------------------------------------
TestShapeDisc::~TestShapeDisc()
{
}


//-----------------------------------------------------------------------------------------------
void TestShapeDisc::Update( float deltaSeconds )
{
	m_center += m_velocity * deltaSeconds;
}