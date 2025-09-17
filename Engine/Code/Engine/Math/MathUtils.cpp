#define _USE_MATH_DEFINES
#include <cmath>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
float GetClamped( float value, float minValue, float maxValue )
{
	if ( value < minValue )
	{
		return minValue;
	}
	else if ( value > maxValue )
	{
		return maxValue;
	}
	else
	{
		return value;
	}
}


//-----------------------------------------------------------------------------------------------
float GetClampedZeroToOne( float value )
{
	if ( value < 0.f )
	{
		return 0.f;
	}
	else if ( value > 1.f )
	{
		return 1.f;
	}
	else
	{
		return value;
	}
}


//-----------------------------------------------------------------------------------------------
float Interpolate( float start, float end, float fractionTowardEnd )
{
	float result = start + ( ( end - start ) * fractionTowardEnd );
	return result;
}


//-----------------------------------------------------------------------------------------------
float GetFractionWithinRange( float value, float rangeStart, float rangeEnd )
{
	if ( rangeStart == rangeEnd )
	{
		return 0.5f;
	}
	else
	{
		float fraction = ( value - rangeStart ) / ( rangeEnd - rangeStart );
		return fraction;
	}
}


//-----------------------------------------------------------------------------------------------
float RangeMap( float inValue, float inStart, float inEnd, float outStart, float outEnd )
{
	float fraction = GetFractionWithinRange( inValue, inStart, inEnd );
	float outValue = Interpolate( outStart, outEnd, fraction );
	return outValue;
}


//-----------------------------------------------------------------------------------------------
float RangeMapClamped( float inValue, float inStart, float inEnd, float outStart, float outEnd )
{
	float fraction = GetFractionWithinRange( inValue, inStart, inEnd );
	fraction = GetClampedZeroToOne( fraction );
	float outValue = Interpolate( outStart, outEnd, fraction );
	return outValue;
}


//-----------------------------------------------------------------------------------------------
float RoundDownToInt( float value )
{
	return floorf( value );
}


//-----------------------------------------------------------------------------------------------
float ConvertDegreesToRadians( float degrees )
{
	float radians = degrees * ( static_cast<float> ( M_PI ) / 180.f );
	return radians;
}


//-----------------------------------------------------------------------------------------------
float ConvertRadiansToDegrees( float radians )
{
	float degrees = radians * ( 180.f / static_cast< float > ( M_PI ) );
	return degrees;
}


//-----------------------------------------------------------------------------------------------
float CosDegrees( float degrees )
{
	float radians = ConvertDegreesToRadians( degrees );
	float cosine = cosf( radians );
	return cosine;
}


//-----------------------------------------------------------------------------------------------
float SinDegrees( float degrees )
{
	float radians = ConvertDegreesToRadians( degrees );
	float sine = sinf( radians );
	return sine;
}


//-----------------------------------------------------------------------------------------------
float Atan2Degrees( float y, float x )
{
	float atan2Radians = atan2f( y, x );
	float atan2Degrees = ConvertRadiansToDegrees( atan2Radians );
	return atan2Degrees;
}


//-----------------------------------------------------------------------------------------------
float GetShortestAngularDispDegrees( float startDegrees, float endDegrees )
{
	float deltaDegrees = endDegrees - startDegrees;
	while ( deltaDegrees > 180.f )
	{
		deltaDegrees -= 360.f;
	}
	while ( deltaDegrees < -180.f )
	{
		deltaDegrees += 360.f;
	}
	return deltaDegrees;
}


//-----------------------------------------------------------------------------------------------
float GetTurnedTowardDegrees( float currentDegrees, float goalDegrees, float maxDeltaDegrees )
{
	float deltaDegrees = GetShortestAngularDispDegrees( currentDegrees, goalDegrees );
	if ( deltaDegrees > maxDeltaDegrees )
	{
		deltaDegrees = maxDeltaDegrees;
	}
	else if ( deltaDegrees < -maxDeltaDegrees )
	{
		deltaDegrees = -maxDeltaDegrees;
	}
	float newDegrees = currentDegrees + deltaDegrees;
	while ( newDegrees > 180.f )
	{
		newDegrees -= 360.f;
	}
	while ( newDegrees < -180.f )
	{
		newDegrees += 360.f;
	}
	return newDegrees;
}


//-----------------------------------------------------------------------------------------------
float DotProduct2D( Vec2 const& a, Vec2 const& b )
{
	float dotProduct = ( a.x * b.x ) + ( a.y * b.y );
	return dotProduct;
}


//-----------------------------------------------------------------------------------------------
float GetDistanceSquared2D( Vec2 const& positionA, Vec2 const& positionB )
{
	float deltaX = positionB.x - positionA.x;
	float deltaY = positionB.y - positionA.y;
	float distanceSquared = ( deltaX * deltaX ) + ( deltaY * deltaY );
	return distanceSquared;
}


//-----------------------------------------------------------------------------------------------
float GetDistance2D( Vec2 const& positionA, Vec2 const& positionB )
{
	float distanceSquared = GetDistanceSquared2D( positionA, positionB );
	float distance = sqrtf( distanceSquared );
	return distance;
}


//-----------------------------------------------------------------------------------------------
float GetDistanceSquared3D( Vec3 const& positionA, Vec3 const& positionB )
{
	float deltaX = positionB.x - positionA.x;
	float deltaY = positionB.y - positionA.y;
	float deltaZ = positionB.z - positionA.z;
	float distanceSquared = ( deltaX * deltaX ) + ( deltaY * deltaY ) + ( deltaZ * deltaZ );
	return distanceSquared;
}


//-----------------------------------------------------------------------------------------------
float GetDistance3D( Vec3 const& positionA, Vec3 const& positionB )
{
	float distanceSquared = GetDistanceSquared3D( positionA, positionB );
	float distance = sqrtf( distanceSquared );
	return distance;
}


//-----------------------------------------------------------------------------------------------
float GetDistanceXYSquared3D( Vec3 const& positionA, Vec3 const& positionB )
{
	float deltaX = positionB.x - positionA.x;
	float deltaY = positionB.y - positionA.y;
	float distanceSquared = ( deltaX * deltaX ) + ( deltaY * deltaY );
	return distanceSquared;
}


//-----------------------------------------------------------------------------------------------
float GetDistanceXY3D( Vec3 const& positionA, Vec3 const& positionB )
{
	float distanceSquared = GetDistanceXYSquared3D( positionA, positionB );
	float distance = sqrtf( distanceSquared );
	return distance;
}


//-----------------------------------------------------------------------------------------------
bool DoDiscsOverlap( Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB )
{
	float radiusSum = radiusA + radiusB;
	bool doOverlap = GetDistanceSquared2D( centerA, centerB ) < ( radiusSum * radiusSum );
	return doOverlap;
}


//-----------------------------------------------------------------------------------------------
bool DoSpheresOverlap( Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB )
{
	float radiusSum = radiusA + radiusB;
	bool doOverlap = GetDistanceSquared3D( centerA, centerB ) < ( radiusSum * radiusSum );
	return doOverlap;
}


//-----------------------------------------------------------------------------------------------
void TransformPosition2D( Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation )
{
	// Scale
	posToTransform *= uniformScale;

	// Rotate
	posToTransform = posToTransform.GetRotatedByDegrees( rotationDegrees );
	
	// Translate
	posToTransform += translation;
}


//-----------------------------------------------------------------------------------------------
void TransformPositionXY3D( Vec3& posToTransform, float uniformScale, float zRotationDegrees, Vec2 const& xyTranslation )
{
	// Scale (only x and y)
	posToTransform.x *= uniformScale;
	posToTransform.y *= uniformScale;

	// Rotate
	posToTransform = posToTransform.GetRotatedAboutZDegrees( zRotationDegrees );
	

	// Translate
	posToTransform.x += xyTranslation.x;
	posToTransform.y += xyTranslation.y;
}