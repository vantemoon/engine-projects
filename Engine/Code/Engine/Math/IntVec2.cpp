#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <math.h>


//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::ZERO = IntVec2( 0, 0 );
const IntVec2 IntVec2::ONE = IntVec2( 1, 1 );


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
void IntVec2::SetFromText( const char* text )
{
	Strings splitStrings = SplitStringOnDelimiter( text, ',' );
	if ( splitStrings.size() == 2 )
	{
		x = atoi( splitStrings[0].c_str() );
		y = atoi( splitStrings[1].c_str() );
	}
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


//-----------------------------------------------------------------------------------------------
IntVec2 IntVec2::operator+( const IntVec2& vecToAdd ) const
{
	return IntVec2( x + vecToAdd.x, y + vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
IntVec2 IntVec2::operator-( const IntVec2& vecToSubtract ) const
{
	return IntVec2( x - vecToSubtract.x, y - vecToSubtract.y );
}


//-----------------------------------------------------------------------------------------------
bool IntVec2::operator==( const IntVec2& compare ) const
{
	return ( x == compare.x && y == compare.y );
}


//-----------------------------------------------------------------------------------------------
bool IntVec2::operator!=( const IntVec2& compare ) const
{
	return ( x != compare.x || y != compare.y );
}