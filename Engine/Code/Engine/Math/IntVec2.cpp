#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>


//-----------------------------------------------------------------------------------------------
IntVec2::IntVec2( const IntVec2& copyFrom )
	: x( copyFrom.x )
	, y( copyFrom.y )
{
}


//-----------------------------------------------------------------------------------------------
IntVec2::IntVec2( int initialX, int initialY )
	: x( initialX )
	, y( initialY )
{
}


//-----------------------------------------------------------------------------------------------
int IntVec2::GetLengthSquared() const
{
	return ( x * x ) + ( y * y );
}


//-----------------------------------------------------------------------------------------------
float IntVec2::GetLength() const
{
	int lengthSquared = GetLengthSquared();
	float length = sqrtf( static_cast<float> ( lengthSquared ) );
	return length;
}


//-----------------------------------------------------------------------------------------------
int IntVec2::GetTaxicabLength() const
{
	return abs( x ) + abs( y );
}


//-----------------------------------------------------------------------------------------------
float IntVec2::GetOrientationRadians() const
{
	return atan2f( static_cast<float> ( y ), static_cast<float> ( x ) );
}


//-----------------------------------------------------------------------------------------------
float IntVec2::GetOrientationDegrees() const
{
	return ConvertRadiansToDegrees( GetOrientationRadians() );
}


//-----------------------------------------------------------------------------------------------
IntVec2 const IntVec2::GetRotatedBy90Degrees() const
{
	return IntVec2( -y, x );
}


//-----------------------------------------------------------------------------------------------
IntVec2 const IntVec2::GetRotatedByMinus90Degrees() const
{
	return IntVec2( y, -x );
}


//-----------------------------------------------------------------------------------------------
void IntVec2::Rotate90Degrees()
{
	int oldX = x;
	x = -y;
	y = oldX;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::RotateMinus90Degrees()
{
	int oldX = x;
	x = y;
	y = -oldX;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator=( const IntVec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}