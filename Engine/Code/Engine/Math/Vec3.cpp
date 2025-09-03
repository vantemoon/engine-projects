#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( Vec3 const& copyFrom )
	: x( copyFrom.x )
	, y( copyFrom.y )
	, z( copyFrom.z )
{
}


Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}