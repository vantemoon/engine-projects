#include "Engine/Math/Plane3.hpp"


//-----------------------------------------------------------------------------------------------
Plane3::Plane3( Vec3 const& normal, float distanceAlongNormalFromOrigin )
	: m_normal( normal.GetNormalized() )
	, m_distanceAlongNormalFromOrigin( distanceAlongNormalFromOrigin )
{
}


//-----------------------------------------------------------------------------------------------
Plane3::~Plane3()
{
}