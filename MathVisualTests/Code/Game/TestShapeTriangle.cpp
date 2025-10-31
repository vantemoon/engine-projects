#include "Game/TestShapeTriangle.hpp"

//-----------------------------------------------------------------------------------------------
TestShapeTriangle::TestShapeTriangle( Vec2 const& ccw0, Vec2 const& ccw1, Vec2 const& ccw2 )
	: m_ccw0( ccw0 )
	, m_ccw1( ccw1 )
	, m_ccw2( ccw2 )
{
}


//-----------------------------------------------------------------------------------------------
TestShapeTriangle::~TestShapeTriangle()
{
}