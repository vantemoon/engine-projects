#include "Engine/Math/Plane2.hpp"


//-----------------------------------------------------------------------------------------------
Plane2::Plane2( Vec2 const& normal, float distanceAlongNormalFromOrigin )
	: m_normal( normal.GetNormalized() )
	, m_distanceAlongNormalFromOrigin( distanceAlongNormalFromOrigin )
{
}


//-----------------------------------------------------------------------------------------------
Plane2::~Plane2()
{
}