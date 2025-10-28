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
	return fabsf( angleDifferenceDegrees ) < ( sectorApertureDegrees * 0.5f );
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
	return actualCosine > minimumCosine;
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideAABB2D( Vec2 const& point, AABB2 const& alignedBox )
{
	bool isInsideX = point.x > alignedBox.m_mins.x && point.x < alignedBox.m_maxs.x;
	bool isInsideY = point.y > alignedBox.m_mins.y && point.y < alignedBox.m_maxs.y;

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

	bool isInsideX = fabsf( xProjection ) < orientedBox.m_halfDimensions.x;
	bool isInsideY = fabsf( yProjection ) < orientedBox.m_halfDimensions.y;

	bool isInside = isInsideX && isInsideY;
	return isInside;
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideCapsule2D( Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius )
{
	// Region I check
	Vec2 boneVector = boneEnd - boneStart;
	Vec2 startToPoint = point - boneStart;
	float startToPointDot = DotProduct2D( startToPoint, boneVector );
	if ( startToPointDot <= 0.f )
	{
		return IsPointInsideDisc2D( point, boneStart, radius );
	}

	// Region II check
	Vec2 endToPoint = point - boneEnd;
	float endToPointDot = DotProduct2D( endToPoint, boneVector );
	if ( endToPointDot >= 0.f )
	{
		return IsPointInsideDisc2D( point, boneEnd, radius );
	}

	// Region III
	Vec2 nearestPointOnBone = GetNearestPointOnInfiniteLine2D( point, boneStart, boneEnd );
	float distanceSquared = GetDistanceSquared2D( point, nearestPointOnBone );
	float radiusSquared = radius * radius;
	return distanceSquared < radiusSquared;
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideTriangle2D( Vec2 const& point, Vec2 const& ccw0, Vec2 const& ccw1, Vec2 const& ccw2 )
{
	Vec2 edgeAB = ccw1 - ccw0;
	Vec2 abNormal = edgeAB.GetRotatedByMinus90Degrees();
	Vec2 toPointFromA = point - ccw0;
	float abDot = DotProduct2D( toPointFromA, abNormal );
	if ( abDot >= 0.f )
	{
		return false;
	}

	Vec2 edgeBC = ccw2 - ccw1;
	Vec2 bcNormal = edgeBC.GetRotatedByMinus90Degrees();
	Vec2 toPointFromB = point - ccw1;
	float bcDot = DotProduct2D( toPointFromB, bcNormal );
	if ( bcDot >= 0.f )
	{
		return false;
	}

	Vec2 edgeCA = ccw0 - ccw2;
	Vec2 caNormal = edgeCA.GetRotatedByMinus90Degrees();
	Vec2 toPointFromC = point - ccw2;
	float caDot = DotProduct2D( toPointFromC, caNormal );
	if ( caDot >= 0.f )
	{
		return false;
	}

	return true;
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
	Vec2 lineVector = anotherPointOnLine - pointOnLine;
	if ( lineVector.GetLengthSquared() == 0.f )
	{
		return pointOnLine;
	}
	Vec2 toReference = referencePos - pointOnLine;
	Vec2 projectedVector = GetProjectedVector2D( toReference, lineVector );
	Vec2 nearestPoint = pointOnLine + projectedVector;
	return nearestPoint;
}


//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnLineSegment2D( Vec2 const& referencePos, Vec2 const& start, Vec2 const& end )
{
	Vec2 segmentVector = end - start;
	if ( segmentVector.GetLengthSquared() == 0.f )
	{
		return start;
	}

	// Region I check
	Vec2 startToReference = referencePos - start;
	float startToRefDot = DotProduct2D( startToReference, segmentVector );
	if ( startToRefDot <= 0.f )
	{
		return start;
	}

	// Region II check
	Vec2 endToReference = referencePos - end;
	float endToRefDot = DotProduct2D( endToReference, segmentVector );
	if ( endToRefDot >= 0.f )
	{
		return end;
	}

	// Region III
	Vec2 nearestPoint = GetNearestPointOnInfiniteLine2D( referencePos, start, end );
	return nearestPoint;
}


//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnCapsule2D( Vec2 const& referencePos, Vec2 const& boneStart, Vec2 const& boneEnd, float radius )
{
	// Region I check
	Vec2 boneVector = boneEnd - boneStart;
	Vec2 startToReference = referencePos - boneStart;
	float startToRefDot = DotProduct2D( startToReference, boneVector );
	if ( startToRefDot <= 0.f )
	{
		return GetNearestPointOnDisc2D( referencePos, boneStart, radius );
	}

	// Region II check
	Vec2 endToReference = referencePos - boneEnd;
	float endToRefDot = DotProduct2D( endToReference, boneVector );
	if ( endToRefDot >= 0.f )
	{
		return GetNearestPointOnDisc2D( referencePos, boneEnd, radius );
	}

	// Region III
	Vec2 nearestPointOnBone = GetNearestPointOnInfiniteLine2D( referencePos, boneStart, boneEnd );
	Vec2 toReferenceClamped = ( referencePos - nearestPointOnBone ).GetClamped( radius );
	Vec2 nearestPoint = nearestPointOnBone + toReferenceClamped;
	return nearestPoint;
}


//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnTriangle2D( Vec2 const& referencePos, Vec2 const& ccw0, Vec2 const& ccw1, Vec2 const& ccw2 )
{
	// Region AB check
	Vec2 edgeAB = ccw1 - ccw0;
	Vec2 abNormal = edgeAB.GetRotatedByMinus90Degrees();
	Vec2 toReferenceFromA = referencePos - ccw0;
	float abDot = DotProduct2D( toReferenceFromA, abNormal );
	bool isOutsideAB = abDot >= 0.f;

	// Region BC check
	Vec2 edgeBC = ccw2 - ccw1;
	Vec2 bcNormal = edgeBC.GetRotatedByMinus90Degrees();
	Vec2 toReferenceFromB = referencePos - ccw1;
	float bcDot = DotProduct2D( toReferenceFromB, bcNormal );
	bool isOutsideBC = bcDot >= 0.f;

	// Region CA check
	Vec2 edgeCA = ccw0 - ccw2;
	Vec2 caNormal = edgeCA.GetRotatedByMinus90Degrees();
	Vec2 toReferenceFromC = referencePos - ccw2;
	float caDot = DotProduct2D( toReferenceFromC, caNormal );
	bool isOutsideCA = caDot >= 0.f;

	if ( !isOutsideAB && !isOutsideBC && !isOutsideCA )
	{
		return referencePos;
	}
	else if ( isOutsideAB && !isOutsideBC && !isOutsideCA )
	{
		return GetNearestPointOnLineSegment2D( referencePos, ccw0, ccw1 );
	}
	else if ( !isOutsideAB && isOutsideBC && !isOutsideCA )
	{
		return GetNearestPointOnLineSegment2D( referencePos, ccw1, ccw2 );
	}
	else if ( !isOutsideAB && !isOutsideBC && isOutsideCA )
	{
		return GetNearestPointOnLineSegment2D( referencePos, ccw2, ccw0 );
	}
	else if ( isOutsideAB && isOutsideBC )
	{
		return ccw1;
	}
	else if ( isOutsideBC && isOutsideCA )
	{
		return ccw2;
	}
	else // if ( isOutsideCA && isOutsideAB )
	{
		return ccw0;
	}
}