#include "Game/TestShapeLineSegment.hpp"


//-----------------------------------------------------------------------------------------------
TestShapeLineSegment::TestShapeLineSegment( Vec2 const& start, Vec2 const& end, float thickness, bool isInfinite )
	: m_start( start )
	, m_end( end )
	, m_thickness( thickness )
	, m_isInfinite( isInfinite )
{
}


//-----------------------------------------------------------------------------------------------
TestShapeLineSegment::~TestShapeLineSegment()
{
}