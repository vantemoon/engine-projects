#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <math.h>


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::ZERO = Vec2( 0.f, 0.f );
Vec2 const Vec2::ONE = Vec2( 1.f, 1.f );


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( Vec2 const& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarDegrees( float orientationDegrees, float length )
{
	float x = length * CosDegrees( orientationDegrees );
	float y = length * SinDegrees( orientationDegrees );
	Vec2 result( x, y );
	return result;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarRadians( float orientationRadians, float length )
{
	float degrees = ConvertRadiansToDegrees( orientationRadians );
	Vec2 result = MakeFromPolarDegrees( degrees, length );
	return result;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromIntVec2( struct IntVec2 const& intVec2 )
{
	return Vec2( static_cast<float>( intVec2.x ), static_cast<float>( intVec2.y ) );
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetLengthSquared() const
{
	float lengthSquared = ( x * x ) + ( y * y );
	return lengthSquared;
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetLength() const
{
	float lengthSquared = GetLengthSquared();
	float length = sqrtf( lengthSquared );
	return length;
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetOrientationDegrees() const
{
	float orientationDegrees = Atan2Degrees( y, x );
	return orientationDegrees;
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetOrientationRadians() const
{
	float orientationDegrees = GetOrientationDegrees();
	float orientationRadians = ConvertDegreesToRadians( orientationDegrees );
	return orientationRadians;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedBy90Degrees() const
{
	return Vec2( -y, x );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedByMinus90Degrees() const
{
	return Vec2( y, -x );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedByDegrees( float rotationDegrees ) const
{
	// Convert to polar
	float currentOrientation = GetOrientationDegrees();
	float length = GetLength();

	// Rotation
	float newOrientation = currentOrientation + rotationDegrees;

	// Convert back to Cartesian
	Vec2 result = MakeFromPolarDegrees( newOrientation, length );
	return result;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedByRadians( float rotationRadians ) const
{
	float rotationDegrees = ConvertRadiansToDegrees( rotationRadians );
	Vec2 result = GetRotatedByDegrees( rotationDegrees );
	return result;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetClamped( float maxLength ) const
{
	float length = GetLength();
	if ( length <= maxLength )
	{
		return *this;
	}
	else
	{
		Vec2 direction = GetNormalized();
		Vec2 result = direction * maxLength;
		return result;
	}
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetNormalized() const
{
	float length = GetLength();
	if ( length == 0.f )
	{
		return Vec2( 0.f, 0.f );
	}
	else
	{
		return Vec2( x / length, y / length );
	}
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetReflected( Vec2 const& normalOfSurfaceToReflectOffOf ) const
{
	Vec2 vn = DotProduct2D( *this, normalOfSurfaceToReflectOffOf ) * normalOfSurfaceToReflectOffOf;
	Vec2 vt = *this - vn;
	Vec2 reflected = vt - vn;
	return reflected;
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetOrientationDegrees( float newOrientationDegrees )
{
	float length = GetLength();
	*this = MakeFromPolarDegrees( newOrientationDegrees, length );
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetOrientationRadians( float newOrientationRadians )
{
	float newOrientationDegrees = ConvertRadiansToDegrees( newOrientationRadians );
	SetOrientationDegrees( newOrientationDegrees );
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarDegrees( float newOrientationDegrees, float newLength )
{
	*this = MakeFromPolarDegrees( newOrientationDegrees, newLength );
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarRadians( float newOrientationRadians, float newLength )
{
	float newOrientationDegrees = ConvertRadiansToDegrees( newOrientationRadians );
	SetPolarDegrees( newOrientationDegrees, newLength );
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetFromText( char const* text )
{
	Strings splitText = SplitStringOnDelimiter( text, ',' );
	if ( splitText.size() == 2 )
	{
		x = static_cast<float>( atof( splitText[0].c_str() ) );
		y = static_cast<float>( atof( splitText[1].c_str() ) );
	}
}


//-----------------------------------------------------------------------------------------------
void Vec2::Rotate90Degrees()
{
	*this = GetRotatedBy90Degrees();
}


//-----------------------------------------------------------------------------------------------
void Vec2::RotateMinus90Degrees()
{
	*this = GetRotatedByMinus90Degrees();
}


//-----------------------------------------------------------------------------------------------
void Vec2::RotateDegrees( float rotationDegrees )
{
	*this = GetRotatedByDegrees( rotationDegrees );
}


//-----------------------------------------------------------------------------------------------
void Vec2::RotateRadians( float rotationRadians )
{
	float rotationDegrees = ConvertRadiansToDegrees( rotationRadians );
	RotateDegrees( rotationDegrees );
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetLength( float newLength )
{
	Vec2 direction = GetNormalized();
	*this = direction * newLength;
}


//-----------------------------------------------------------------------------------------------
void Vec2::ClampLength( float maxLength )
{
	*this = GetClamped( maxLength );
}


//-----------------------------------------------------------------------------------------------
void Vec2::Normalize()
{
	float length = GetLength();
	if ( length != 0.f )
	{
		*this = *this / length;
	}
}


//-----------------------------------------------------------------------------------------------
float Vec2::NormalizeAndGetPreviousLength()
{
	float length = GetLength();
	Normalize();
	return length;
}


//-----------------------------------------------------------------------------------------------
void Vec2::Reflect( Vec2 const& normalOfSurfaceToReflectOffOf )
{
	*this = GetReflected( normalOfSurfaceToReflectOffOf );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator + ( Vec2 const& vecToAdd ) const
{
	return Vec2( this->x + vecToAdd.x, this->y + vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator-( Vec2 const& vecToSubtract ) const
{
	return Vec2( this->x - vecToSubtract.x, this->y - vecToSubtract.y );
}


//------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator-() const
{
	return Vec2( -this->x, -this->y );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator*( float uniformScale ) const
{
	return Vec2( this->x * uniformScale, this->y * uniformScale );
}


//------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator*( Vec2 const& vecToMultiply ) const
{
	return Vec2( this->x * vecToMultiply.x, this->y * vecToMultiply.y );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator/( float inverseScale ) const
{
	return Vec2( this->x / inverseScale, this->y / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( Vec2 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( Vec2 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( Vec2 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
Vec2 const operator*( float uniformScale, Vec2 const& vecToScale )
{
	return Vec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( Vec2 const& compare ) const
{
	return this->x == compare.x && this->y == compare.y;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( Vec2 const& compare ) const
{
	return !( *this == compare );
}


