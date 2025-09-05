#define _USE_MATH_DEFINES
#include <cmath>
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
float ConvertDegreesToRadians( float degrees )
{
	float radians = degrees * ( M_PI / 180.f );
	return radians;
}


//-----------------------------------------------------------------------------------------------
float ConvertRadiansToDegrees( float radians )
{
	float degrees = radians * ( 180.f / M_PI );
	return degrees;
}


//-----------------------------------------------------------------------------------------------
float CosDegrees( float degrees )
{
	float radians = ConvertDegreesToRadians( degrees );
	return cosf( radians );
}


//-----------------------------------------------------------------------------------------------
float SinDegrees( float degrees )
{
	float radians = ConvertDegreesToRadians( degrees );
	return sinf( radians );
}


//-----------------------------------------------------------------------------------------------
float Atan2Degrees( float y, float x )
{
	float radians = atan2f( y, x );
	return ConvertRadiansToDegrees( radians );
}