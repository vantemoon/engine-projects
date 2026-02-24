#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <math.h>


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::ZERO = Vec3( 0.f, 0.f, 0.f );
Vec3 const Vec3::ONE = Vec3( 1.f, 1.f, 1.f );


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( Vec3 const& copyFrom )
	: x( copyFrom.x )
	, y( copyFrom.y )
	, z( copyFrom.z )
{
}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthSquared() const
{
	float lengthSquared = ( x * x ) + ( y * y ) + ( z * z );
	return lengthSquared;
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetLength() const
{
	float lengthSquared = GetLengthSquared();
	float length = sqrtf( lengthSquared );
	return length;
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthXYSquared() const
{
	float lengthSquared = ( x * x ) + ( y * y );
	return lengthSquared;
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthXY() const
{
	float lengthSquared = GetLengthXYSquared();
	float length = sqrtf( lengthSquared );
	return length;
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetOrientationAboutZDegrees() const
{
	float orientationDegrees = Atan2Degrees( y, x );
	return orientationDegrees;
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetOrientationAboutZRadians() const
{
	float orientationDegrees = GetOrientationAboutZDegrees();
	float orientationRadians = ConvertDegreesToRadians( orientationDegrees );
	return orientationRadians;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::GetRotatedAboutZDegrees( float rotationDegrees ) const
{
	// Convert to polar
	float currentOrientation = GetOrientationAboutZDegrees();
	float lengthXY = GetLengthXY();
	float zValue = z;

	// Rotation
	float newOrientation = currentOrientation + rotationDegrees;

	// Convert back to Cartesian
	Vec3 result;
	result.x = lengthXY * CosDegrees( newOrientation );
	result.y = lengthXY * SinDegrees( newOrientation );
	result.z = zValue;
	return result;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::GetRotatedAboutZRadians( float rotationRadians ) const
{
	float rotationDegrees = ConvertRadiansToDegrees( rotationRadians );
	Vec3 result = GetRotatedAboutZDegrees( rotationDegrees );
	return result;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::GetClampedToMaxLength( float maxLength ) const
{
	float length = GetLength();
	if ( length <= maxLength )
	{
		return *this;
	}
	else
	{
		Vec3 direction = GetNormalized();
		Vec3 result = direction * maxLength;
		return result;
	}
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::GetNormalized() const
{
	float length = GetLength();
	if ( length != 0.f )
	{
		return *this / length;
	}
	else
	{
		return Vec3( 0.f, 0.f, 0.f );
	}
}


//-----------------------------------------------------------------------------------------------
void Vec3::SetFromText( char const* text )
{
	Strings splitStrings = SplitStringOnDelimiter( text, ',' );
	if ( splitStrings.size() >= 3 )
	{
		x = static_cast<float>( atof( splitStrings[0].c_str() ) );
		y = static_cast<float>( atof( splitStrings[1].c_str() ) );
		z = static_cast<float>( atof( splitStrings[2].c_str() ) );
	}
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( Vec3 const& compare ) const
{
	return this->x == compare.x && this->y == compare.y && this->z == compare.z;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( Vec3 const& compare ) const
{
	return !( *this == compare );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator+( Vec3 const& vecToAdd ) const
{
	return Vec3( this->x + vecToAdd.x, this->y + vecToAdd.y, this->z + vecToAdd.z );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator-( Vec3 const& vecToSubtract ) const
{
	return Vec3( this->x - vecToSubtract.x, this->y - vecToSubtract.y, this->z - vecToSubtract.z );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator-() const
{
	return Vec3( -this->x, -this->y, -this->z );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator*( float uniformScale ) const
{
	return Vec3( this->x * uniformScale, this->y * uniformScale, this->z * uniformScale );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator*( Vec3 const& vecToMultiply ) const
{
	return Vec3( this->x * vecToMultiply.x, this->y * vecToMultiply.y, this->z * vecToMultiply.z );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator/( float inverseScale ) const
{
	return Vec3( this->x / inverseScale, this->y / inverseScale, this->z / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( Vec3 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( Vec3 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=( Vec3 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
Vec3 const operator*( float uniformScale, Vec3 const& vecToScale )
{
	return vecToScale * uniformScale;
}