#include "Engine/Math/Vec4.hpp"


//-----------------------------------------------------------------------------------------------
Vec4::Vec4()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}


//-----------------------------------------------------------------------------------------------
Vec4::Vec4( float initialX, float initialY, float initialZ, float initialW )
{
	x = initialX;
	y = initialY;
	z = initialZ;
	w = initialW;
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator+( Vec4 const& vecToAdd ) const
{
	return Vec4( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w );
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator==( Vec4 const& vecToCompare ) const
{
	return ( x == vecToCompare.x ) && ( y == vecToCompare.y ) && ( z == vecToCompare.z ) && ( w == vecToCompare.w );
}