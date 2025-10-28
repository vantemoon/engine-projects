#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#define _USE_MATH_DEFINES
#include <math.h>


//-----------------------------------------------------------------------------------------------
float GetClamped( float value, float minValue, float maxValue )
{
	if ( value < minValue )
	{
		return minValue;
	}
	if ( value > maxValue )
	{
		return maxValue;
	}
	return value;
}


//-----------------------------------------------------------------------------------------------
float GetClampedZeroToOne( float value )
{
	if ( value < 0.f )
	{
		return 0.f;
	}
	if ( value > 1.f )
	{
		return 1.f;
	}
	return value;
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
	float fraction = ( value - rangeStart ) / ( rangeEnd - rangeStart );
	return fraction;
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
float GetAngleDegreesBetweenVectors2D( Vec2 const& a, Vec2 const& b )
{
	float aLength = a.GetLength();
	float bLength = b.GetLength();
	if ( aLength == 0.f || bLength == 0.f )
	{
		return 0.f;
	}
	float aDotB = DotProduct2D( a, b );
	float cosTheta = aDotB / ( aLength * bLength );
	cosTheta = GetClamped( cosTheta, -1.f, 1.f );
	float angleDegrees = ConvertRadiansToDegrees( acosf( cosTheta ) );
	return angleDegrees;
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
int GetTaxicabDistance2D( IntVec2 const& pointA, IntVec2 const& pointB )
{
	int deltaX = abs( pointB.x - pointA.x );
	int deltaY = abs( pointB.y - pointA.y );
	int taxicabDistance = deltaX + deltaY;
	return taxicabDistance;
}


//-----------------------------------------------------------------------------------------------
float GetProjectedLength2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto )
{
	if ( vectorToProjectOnto.GetLengthSquared() == 0.f )
	{
		return 0.f;
	}

	Vec2 ontoNormalized = vectorToProjectOnto.GetNormalized();
	float projectedLength = DotProduct2D( vectorToProject, ontoNormalized );
	return projectedLength;
}


//-----------------------------------------------------------------------------------------------
Vec2 const GetProjectedVector2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto )
{
	if ( vectorToProjectOnto.GetLengthSquared() == 0.f )
	{
		return Vec2( 0.f, 0.f );
	}

	Vec2 ontoNormalized = vectorToProjectOnto.GetNormalized();
	float projectedLength = DotProduct2D( vectorToProject, ontoNormalized );
	Vec2 projectedVector = ontoNormalized * projectedLength;
	return projectedVector;
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
bool PushDiscOutOfFixedPoint2D( Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedPoint )
{
	Vec2 toMobile = mobileDiscCenter - fixedPoint;
	float distanceSquared = toMobile.GetLengthSquared();
	float radiusSum = mobileDiscRadius;
	float radiusSumSquared = radiusSum * radiusSum;
	if ( distanceSquared < radiusSumSquared )
	{
		if ( distanceSquared == 0.f )
		{
			mobileDiscCenter += Vec2( radiusSum, 0.f );
		}
		else
		{
			float distance = sqrtf( distanceSquared );
			Vec2 moveDirection = toMobile.GetNormalized();
			Vec2 moveVector = moveDirection * ( radiusSum - distance );
			mobileDiscCenter += moveVector;
		}
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool PushDiscOutOfFixedDisc2D( Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius )
{
	Vec2 toMobile = mobileDiscCenter - fixedDiscCenter;
	float distanceSquared = toMobile.GetLengthSquared();
	float radiusSum = mobileDiscRadius + fixedDiscRadius;
	float radiusSumSquared = radiusSum * radiusSum;
	if ( distanceSquared < radiusSumSquared )
	{
		if ( distanceSquared == 0.f )
		{
			mobileDiscCenter += Vec2( radiusSum, 0.f );
		}
		else
		{
			float distance = sqrtf( distanceSquared );
			Vec2 moveDirection = toMobile.GetNormalized();
			Vec2 moveVector = moveDirection * ( radiusSum - distance );
			mobileDiscCenter += moveVector;
		}
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool PushDiscsOutOfEachOther2D( Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius )
{
	Vec2 toB = bCenter - aCenter;
	float distanceSquared = toB.GetLengthSquared();
	float radiusSum = aRadius + bRadius;
	float radiusSumSquared = radiusSum * radiusSum;
	if ( distanceSquared < radiusSumSquared )
	{
		if ( distanceSquared == 0.f )
		{
			bCenter += Vec2( radiusSum, 0.f );
		}
		else
		{
			float distance = sqrtf( distanceSquared );
			Vec2 moveDirection = toB.GetNormalized();
			Vec2 moveVector = moveDirection * ( radiusSum - distance );
			bCenter += moveVector * 0.5f;
			aCenter -= moveVector * 0.5f;
		}
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool PushDiscOutOfFixedAABB2D( Vec2& mobileDiscCenter, float mobileDiscRadius, AABB2 const& fixedBox )
{
	float nearestX = GetClamped( mobileDiscCenter.x, fixedBox.m_mins.x, fixedBox.m_maxs.x );
	float nearestY = GetClamped( mobileDiscCenter.y, fixedBox.m_mins.y, fixedBox.m_maxs.y );
	Vec2 nearestPoint( nearestX, nearestY );
	return PushDiscOutOfFixedPoint2D( mobileDiscCenter, mobileDiscRadius, nearestPoint );
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


//-----------------------------------------------------------------------------------------------
void TransformPosition2D( Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation )
{
	float newX = ( posToTransform.x * iBasis.x ) + ( posToTransform.y * jBasis.x ) + translation.x;
	float newY = ( posToTransform.x * iBasis.y ) + ( posToTransform.y * jBasis.y ) + translation.y;
	posToTransform.x = newX;
	posToTransform.y = newY;
}


//-----------------------------------------------------------------------------------------------
void TransformPositionXY3D( Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& xyTranslation )
{
	float newX = ( posToTransform.x * iBasis.x ) + ( posToTransform.y * jBasis.x ) + xyTranslation.x;
	float newY = ( posToTransform.x * iBasis.y ) + ( posToTransform.y * jBasis.y ) + xyTranslation.y;
	posToTransform.x = newX;
	posToTransform.y = newY;
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius )
{
	bool isInside = GetDistanceSquared2D( point, discCenter ) < ( discRadius * discRadius );
	return isInside;
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideOrientedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius )
{
	if ( !IsPointInsideDisc2D( point, sectorTip, sectorRadius ) )
	{
		return false;
	}

	Vec2 toPoint = point - sectorTip;
	float angleToPointDegrees = Atan2Degrees( toPoint.y, toPoint.x );
	float angleDifferenceDegrees = GetShortestAngularDispDegrees( sectorForwardDegrees, angleToPointDegrees );
	return fabsf( angleDifferenceDegrees ) <= ( sectorApertureDegrees * 0.5f );
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius )
{
	if ( !IsPointInsideDisc2D( point, sectorTip, sectorRadius ) )
	{
		return false;
	}
	
	Vec2 toPoint = point - sectorTip;
	float minimumCosine = CosDegrees( sectorApertureDegrees * 0.5f );
	float actualCosine = DotProduct2D( toPoint.GetNormalized(), sectorForwardNormal );
	return actualCosine >= minimumCosine;
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideAABB2D( Vec2 const& point, AABB2 const& alignedBox )
{
	bool isInsideX = point.x >= alignedBox.m_mins.x && point.x <= alignedBox.m_maxs.x;
	bool isInsideY = point.y >= alignedBox.m_mins.y && point.y <= alignedBox.m_maxs.y;

	bool isInside = isInsideX && isInsideY;
	return isInside;
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideOBB2D( Vec2 const& point, OBB2 const& orientedBox )
{
	Vec2 toPoint = point - orientedBox.m_center;
	float xProjection = DotProduct2D( toPoint, orientedBox.m_iBasisNormal );
	Vec2 jBasisNormal = orientedBox.m_iBasisNormal.GetRotatedBy90Degrees();
	float yProjection = DotProduct2D( toPoint, jBasisNormal );

	bool isInsideX = fabsf( xProjection ) <= orientedBox.m_halfDimensions.x;
	bool isInsideY = fabsf( yProjection ) <= orientedBox.m_halfDimensions.y;

	bool isInside = isInsideX && isInsideY;
	return isInside;
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideCapsule2D( Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius )
{
	Vec2 boneVector = boneEnd - boneStart;
	Vec2 toPoint = point - boneStart;
	float boneLengthSquared = boneVector.GetLengthSquared();
	if ( boneLengthSquared == 0.f )
	{
		return IsPointInsideDisc2D( point, boneStart, radius );
	}
	float fractionAlongBone = DotProduct2D( toPoint, boneVector ) / boneLengthSquared;
	fractionAlongBone = GetClampedZeroToOne( fractionAlongBone );
	Vec2 nearestPointOnBone = boneStart + ( boneVector * fractionAlongBone );
	return IsPointInsideDisc2D( point, nearestPointOnBone, radius );
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideTriangle2D( Vec2 const& point, Vec2 const& ccw0, Vec2 const& ccw1, Vec2 const& ccw2 )
{
	Vec2 edge0 = ccw1 - ccw0;
	Vec2 toPoint0 = point - ccw0;
	float cross0 = ( edge0.x * toPoint0.y ) - ( edge0.y * toPoint0.x );

	Vec2 edge1 = ccw2 - ccw1;
	Vec2 toPoint1 = point - ccw1;
	float cross1 = ( edge1.x * toPoint1.y ) - ( edge1.y * toPoint1.x );

	Vec2 edge2 = ccw0 - ccw2;
	Vec2 toPoint2 = point - ccw2;
	float cross2 = ( edge2.x * toPoint2.y ) - ( edge2.y * toPoint2.x );

	bool isInside = ( cross0 >= 0.f ) && ( cross1 >= 0.f ) && ( cross2 >= 0.f );
	return isInside;
}


//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnDisc2D( Vec2 const& referencePos, Vec2 const& discCenter, float discRadius )
{
	Vec2 toReference = referencePos - discCenter;
	float distanceSquared = toReference.GetLengthSquared();
	float radiusSquared = discRadius * discRadius;
	if ( distanceSquared <= radiusSquared )
	{
		return referencePos;
	}
	Vec2 direction = toReference.GetNormalized();
	Vec2 nearestPoint = discCenter + ( direction * discRadius );
	return nearestPoint;
}


//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnAABB2D( Vec2 const& referencePos, AABB2 const& alignedBox )
{
	float clampedX = GetClamped( referencePos.x, alignedBox.m_mins.x, alignedBox.m_maxs.x );
	float clampedY = GetClamped( referencePos.y, alignedBox.m_mins.y, alignedBox.m_maxs.y );
	Vec2 nearestPoint( clampedX, clampedY );
	return nearestPoint;
}


//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnOBB2D( Vec2 const& referencePos, OBB2 const& orientedBox )
{
	Vec2 toReference = referencePos - orientedBox.m_center;
	float xProjection = DotProduct2D( toReference, orientedBox.m_iBasisNormal );
	float clampedX = GetClamped( xProjection, -orientedBox.m_halfDimensions.x, orientedBox.m_halfDimensions.x );
	Vec2 iComponent = orientedBox.m_iBasisNormal * clampedX;
	Vec2 jBasisNormal = orientedBox.m_iBasisNormal.GetRotatedBy90Degrees();
	float yProjection = DotProduct2D( toReference, jBasisNormal );
	float clampedY = GetClamped( yProjection, -orientedBox.m_halfDimensions.y, orientedBox.m_halfDimensions.y );
	Vec2 jComponent = jBasisNormal * clampedY;
	Vec2 nearestPoint = orientedBox.m_center + iComponent + jComponent;
	return nearestPoint;
}


//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnInfiniteLine2D( Vec2 const& referencePos, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine )
{
	Vec2 lineDirection = anotherPointOnLine - pointOnLine;
	if ( lineDirection.GetLengthSquared() == 0.f )
	{
		return pointOnLine;
	}
	lineDirection.Normalize();
	Vec2 toReference = referencePos - pointOnLine;
	float projectedLength = DotProduct2D( toReference, lineDirection );
	Vec2 nearestPoint = pointOnLine + ( lineDirection * projectedLength );
	return nearestPoint;
}


//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnLineSegment2D( Vec2 const& referencePos, Vec2 const& start, Vec2 const& end )
{
	Vec2 segmentVector = end - start;
	float segmentLengthSquared = segmentVector.GetLengthSquared();
	if ( segmentLengthSquared == 0.f )
	{
		return start;
	}
	Vec2 toReference = referencePos - start;
	float projectedLength = DotProduct2D( toReference, segmentVector );
	float fractionAlongSegment = projectedLength / segmentLengthSquared;
	fractionAlongSegment = GetClampedZeroToOne( fractionAlongSegment );
	Vec2 nearestPoint = start + ( segmentVector * fractionAlongSegment );
	return nearestPoint;
}


//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnCapsule2D( Vec2 const& referencePos, Vec2 const& boneStart, Vec2 const& boneEnd, float radius )
{
	Vec2 nearestPointOnBone = GetNearestPointOnLineSegment2D( referencePos, boneStart, boneEnd );
	Vec2 toReference = referencePos - nearestPointOnBone;
	float distanceSquared = toReference.GetLengthSquared();
	float radiusSquared = radius * radius;
	if ( distanceSquared <= radiusSquared )
	{
		return referencePos;
	}
	Vec2 direction = toReference.GetNormalized();
	Vec2 nearestPointOnCapsule = nearestPointOnBone + ( direction * radius );
	return nearestPointOnCapsule;
}


//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnTriangle2D( Vec2 const& referencePos, Vec2 const& ccw0, Vec2 const& ccw1, Vec2 const& ccw2 )
{
	if ( IsPointInsideTriangle2D( referencePos, ccw0, ccw1, ccw2 ) )
	{
		return referencePos;
	}
	Vec2 nearest0 = GetNearestPointOnLineSegment2D( referencePos, ccw0, ccw1 );
	Vec2 nearest1 = GetNearestPointOnLineSegment2D( referencePos, ccw1, ccw2 );
	Vec2 nearest2 = GetNearestPointOnLineSegment2D( referencePos, ccw2, ccw0 );
	float distSq0 = GetDistanceSquared2D( referencePos, nearest0 );
	float distSq1 = GetDistanceSquared2D( referencePos, nearest1 );
	float distSq2 = GetDistanceSquared2D( referencePos, nearest2 );
	if ( distSq0 <= distSq1 && distSq0 <= distSq2 )
	{
		return nearest0;
	}
	else if ( distSq1 <= distSq0 && distSq1 <= distSq2 )
	{
		return nearest1;
	}
	else
	{
		return nearest2;
	}
}