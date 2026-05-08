#include "Engine/Math/RaycastUtils.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>


//-----------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsDisc2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius )
{
	RaycastResult2D result;
	result.m_rayStartPos = startPos;
	result.m_rayFwdNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;
	result.m_didImpact = false;

	Vec2 toDiscCenter = discCenter - startPos;
	float projectionLength = DotProduct2D( toDiscCenter, fwdNormal );

	if ( projectionLength < -discRadius || projectionLength > maxDist + discRadius )
		return result;

	float distCenterToRaySquared = DotProduct2D( toDiscCenter, toDiscCenter ) - projectionLength * projectionLength;

	float discRadiusSquared = discRadius * discRadius;
	if ( distCenterToRaySquared > discRadiusSquared )
		return result;

	if ( IsPointInsideDisc2D( startPos, discCenter, discRadius ) )
	{
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	float adjustmentDist = sqrtf( discRadiusSquared - distCenterToRaySquared );
	float impactDist = projectionLength - adjustmentDist;

	if ( impactDist < 0.f || impactDist > maxDist )
		return result;

	result.m_didImpact = true;
	result.m_impactDist = impactDist;
	result.m_impactPos = startPos + fwdNormal * impactDist;
	result.m_impactNormal = ( result.m_impactPos - discCenter ).GetNormalized();
	return result;
}


//-----------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsLineSegment2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 lineStart, Vec2 lineEnd )
{
	RaycastResult2D result;
	result.m_rayStartPos = startPos;
	result.m_rayFwdNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;
	result.m_didImpact = false;

	if ( maxDist < 0.f )
	{
		return result;
	}

	Vec2 lineVector = lineEnd - lineStart;
	Vec2 lineDirection = lineVector.GetNormalized();
	Vec2 perpendicular = fwdNormal.GetRotatedBy90Degrees();
	Vec2 rayStartToLineStart = lineStart - startPos;
	Vec2 rayStartToLineEnd = lineEnd - startPos;
	float rayStartToLineStartPerpendicular = DotProduct2D( rayStartToLineStart, perpendicular );
	float rayStartToLineEndPerpendicular = DotProduct2D( rayStartToLineEnd, perpendicular );

	if ( rayStartToLineStartPerpendicular * rayStartToLineEndPerpendicular >= 0.f )
		return result;

	float rayStartToLineStartLength = DotProduct2D( rayStartToLineStart, fwdNormal );
	float rayStartToLineEndLength = DotProduct2D( rayStartToLineEnd, fwdNormal );

	if ( rayStartToLineStartLength <= 0.f && rayStartToLineEndLength <= 0.f )
		return result;

	if ( rayStartToLineStartLength >= maxDist && rayStartToLineEndLength >= maxDist )
		return result;

	float lineSegmentHitFraction = rayStartToLineStartPerpendicular / ( rayStartToLineStartPerpendicular - rayStartToLineEndPerpendicular );
	float impactDist = rayStartToLineStartLength + lineSegmentHitFraction * ( rayStartToLineEndLength - rayStartToLineStartLength );

	if ( impactDist < 0.f || impactDist > maxDist )
		return result;

	Vec2 impactPos = startPos + fwdNormal * impactDist;
	Vec2 impactNormal = lineDirection.GetRotatedBy90Degrees();
	if ( DotProduct2D( impactNormal, fwdNormal ) > 0.f )
	{
		impactNormal = -impactNormal;
	}

	result.m_didImpact = true;
	result.m_impactDist = impactDist;
	result.m_impactPos = impactPos;
	result.m_impactNormal = impactNormal;
	return result;
}


//-----------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsAABB2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2 box )
{
	RaycastResult2D result;
	result.m_rayStartPos = startPos;
	result.m_rayFwdNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;
	result.m_didImpact = false;

	if ( maxDist < 0.f )
	{
		return result;
	}

	if ( IsPointInsideAABB2D( startPos, box ) )
	{
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	FloatRange tRange( 0.f, maxDist );

	Vec2 nearNormalX( 0.f, 0.f );
	Vec2 nearNormalY( 0.f, 0.f );

	if ( fwdNormal.x == 0.f )
	{
		if ( startPos.x < box.m_mins.x || startPos.x > box.m_maxs.x )
		{
			return result;
		}
	}
	else
	{
		float tNear = ( box.m_mins.x - startPos.x ) / fwdNormal.x;
		float tFar = ( box.m_maxs.x - startPos.x ) / fwdNormal.x;

		Vec2 nearNormal( -1.f, 0.f );
		Vec2 farNormal( 1.f, 0.f );

		if ( tNear > tFar )
		{
			float tempT = tNear; tNear = tFar; tFar = tempT;
			Vec2  tempN = nearNormal; nearNormal = farNormal; farNormal = tempN;
		}

		nearNormalX = nearNormal;

		tRange.m_min = fmaxf( tRange.m_min, tNear );
		tRange.m_max = fminf( tRange.m_max, tFar );
		if ( tRange.m_min > tRange.m_max )
		{
			return result;
		}
	}

	float tEnterX = tRange.m_min;

	if ( fwdNormal.y == 0.f )
	{
		if ( startPos.y < box.m_mins.y || startPos.y > box.m_maxs.y )
		{
			return result;
		}
	}
	else
	{
		float tNear = ( box.m_mins.y - startPos.y ) / fwdNormal.y;
		float tFar = ( box.m_maxs.y - startPos.y ) / fwdNormal.y;

		Vec2 nearNormal( 0.f, -1.f );
		Vec2 farNormal( 0.f, 1.f );

		if ( tNear > tFar )
		{
			float tempT = tNear; tNear = tFar; tFar = tempT;
			Vec2  tempN = nearNormal; nearNormal = farNormal; farNormal = tempN;
		}

		nearNormalY = nearNormal;

		tRange.m_min = fmaxf( tRange.m_min, tNear );
		tRange.m_max = fminf( tRange.m_max, tFar );
		if ( tRange.m_min > tRange.m_max )
		{
			return result;
		}
	}

	float tEnter = tRange.m_min;
	if ( tEnter > maxDist )
	{
		return result;
	}

	Vec2 impactNormal = nearNormalX;
	if ( tEnter > tEnterX )
	{
		impactNormal = nearNormalY;
	}

	result.m_didImpact = true;
	result.m_impactDist = tEnter;
	result.m_impactPos = startPos + fwdNormal * tEnter;
	result.m_impactNormal = impactNormal;
	return result;
}


//-----------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsAABB3D( Vec3 startPos, Vec3 fwdNormal, float maxDist, AABB3 box )
{
	RaycastResult3D result;
	result.m_rayStartPos = startPos;
	result.m_rayFwdNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;
	result.m_didImpact = false;

	if ( maxDist < 0.f )
	{
		return result;
	}

	if ( startPos.x >= box.m_mins.x && startPos.x <= box.m_maxs.x &&
		startPos.y >= box.m_mins.y && startPos.y <= box.m_maxs.y &&
		startPos.z >= box.m_mins.z && startPos.z <= box.m_maxs.z )
	{
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	FloatRange tRange( 0.f, maxDist );

	Vec3 nearNormalX( 0.f, 0.f, 0.f );
	Vec3 nearNormalY( 0.f, 0.f, 0.f );
	Vec3 nearNormalZ( 0.f, 0.f, 0.f );

	if ( fwdNormal.x == 0.f )
	{
		if ( startPos.x < box.m_mins.x || startPos.x > box.m_maxs.x )
		{
			return result;
		}
	}
	else
	{
		float tNear = ( box.m_mins.x - startPos.x ) / fwdNormal.x;
		float tFar = ( box.m_maxs.x - startPos.x ) / fwdNormal.x;

		Vec3 nearNormal( -1.f, 0.f, 0.f );
		Vec3 farNormal( 1.f, 0.f, 0.f );

		if ( tNear > tFar )
		{
			float tempT = tNear; tNear = tFar; tFar = tempT;
			Vec3 tempN = nearNormal; nearNormal = farNormal; farNormal = tempN;
		}

		nearNormalX = nearNormal;

		tRange.m_min = fmaxf( tRange.m_min, tNear );
		tRange.m_max = fminf( tRange.m_max, tFar );
		if ( tRange.m_min > tRange.m_max )
		{
			return result;
		}
	}

	float tEnterX = tRange.m_min;

	if ( fwdNormal.y == 0.f )
	{
		if ( startPos.y < box.m_mins.y || startPos.y > box.m_maxs.y )
		{
			return result;
		}
	}
	else
	{
		float tNear = ( box.m_mins.y - startPos.y ) / fwdNormal.y;
		float tFar = ( box.m_maxs.y - startPos.y ) / fwdNormal.y;

		Vec3 nearNormal( 0.f, -1.f, 0.f );
		Vec3 farNormal( 0.f, 1.f, 0.f );

		if ( tNear > tFar )
		{
			float tempT = tNear; tNear = tFar; tFar = tempT;
			Vec3 tempN = nearNormal; nearNormal = farNormal; farNormal = tempN;
		}

		nearNormalY = nearNormal;

		tRange.m_min = fmaxf( tRange.m_min, tNear );
		tRange.m_max = fminf( tRange.m_max, tFar );
		if ( tRange.m_min > tRange.m_max )
		{
			return result;
		}
	}

	float tEnterY = tRange.m_min;

	if ( fwdNormal.z == 0.f )
	{
		if ( startPos.z < box.m_mins.z || startPos.z > box.m_maxs.z )
		{
			return result;
		}
	}
	else
	{
		float tNear = ( box.m_mins.z - startPos.z ) / fwdNormal.z;
		float tFar = ( box.m_maxs.z - startPos.z ) / fwdNormal.z;

		Vec3 nearNormal( 0.f, 0.f, -1.f );
		Vec3 farNormal( 0.f, 0.f, 1.f );

		if ( tNear > tFar )
		{
			float tempT = tNear; tNear = tFar; tFar = tempT;
			Vec3 tempN = nearNormal; nearNormal = farNormal; farNormal = tempN;
		}

		nearNormalZ = nearNormal;

		tRange.m_min = fmaxf( tRange.m_min, tNear );
		tRange.m_max = fminf( tRange.m_max, tFar );
		if ( tRange.m_min > tRange.m_max )
		{
			return result;
		}
	}

	float tEnter = tRange.m_min;
	if ( tEnter > maxDist )
	{
		return result;
	}

	Vec3 impactNormal = nearNormalX;
	if ( tEnter > tEnterX )
	{
		impactNormal = nearNormalY;
	}
	if ( tEnter > tEnterY )
	{
		impactNormal = nearNormalZ;
	}

	result.m_didImpact = true;
	result.m_impactDist = tEnter;
	result.m_impactPos = startPos + ( fwdNormal * tEnter );
	result.m_impactNormal = impactNormal;
	return result;
}


//-----------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsSphere3D( Vec3 startPos, Vec3 fwdNormal, float maxDist, Vec3 sphereCenter, float sphereRadius )
{
	RaycastResult3D result;
	result.m_rayStartPos = startPos;
	result.m_rayFwdNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;
	result.m_didImpact = false;

	if ( maxDist < 0.f )
	{
		return result;
	}

	Vec3 toSphereCenter = sphereCenter - startPos;
	float projectionLength = DotProduct3D( toSphereCenter, fwdNormal );

	if ( projectionLength < -sphereRadius || projectionLength > maxDist + sphereRadius )
	{
		return result;
	}

	float distCenterToRaySquared = DotProduct3D( toSphereCenter, toSphereCenter ) - ( projectionLength * projectionLength );

	float sphereRadiusSquared = sphereRadius * sphereRadius;
	if ( distCenterToRaySquared > sphereRadiusSquared )
	{
		return result;
	}

	if ( DotProduct3D( toSphereCenter, toSphereCenter ) <= sphereRadiusSquared )
	{
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	float adjustmentDist = sqrtf( sphereRadiusSquared - distCenterToRaySquared );
	float impactDist = projectionLength - adjustmentDist;

	if ( impactDist < 0.f || impactDist > maxDist )
	{
		return result;
	}

	result.m_didImpact = true;
	result.m_impactDist = impactDist;
	result.m_impactPos = startPos + ( fwdNormal * impactDist );
	result.m_impactNormal = ( result.m_impactPos - sphereCenter ).GetNormalized();
	return result;
}


//-----------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsCylinderZ3D( Vec3 startPos, Vec3 fwdNormal, float maxDist, Vec2 cylinderBaseCenter, float cylinderMinZ, float cylinderMaxZ, float cylinderRadius )
{
	RaycastResult3D result;
	result.m_rayStartPos = startPos;
	result.m_rayFwdNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;
	result.m_didImpact = false;

	if ( maxDist < 0.f )
	{
		return result;
	}

	if ( IsPointInsideCylinderZ3D( startPos, cylinderBaseCenter, cylinderMinZ, cylinderMaxZ, cylinderRadius ) )
	{
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	FloatRange tRange( 0.f, maxDist );

	Vec3 nearNormalZ( 0.f, 0.f, 0.f );
	Vec3 nearNormalXY( 0.f, 0.f, 0.f );

	if ( fwdNormal.z == 0.f )
	{
		if ( startPos.z < cylinderMinZ || startPos.z > cylinderMaxZ )
		{
			return result;
		}
	}
	else
	{
		float tNear = ( cylinderMinZ - startPos.z ) / fwdNormal.z;
		float tFar = ( cylinderMaxZ - startPos.z ) / fwdNormal.z;

		Vec3 nearNormal( 0.f, 0.f, -1.f );
		Vec3 farNormal( 0.f, 0.f, 1.f );

		if ( tNear > tFar )
		{
			float tempT = tNear; tNear = tFar; tFar = tempT;
			Vec3 tempN = nearNormal; nearNormal = farNormal; farNormal = tempN;
		}

		nearNormalZ = nearNormal;

		tRange.m_min = fmaxf( tRange.m_min, tNear );
		tRange.m_max = fminf( tRange.m_max, tFar );
		if ( tRange.m_min > tRange.m_max )
		{
			return result;
		}
	}

	float tEnterZ = tRange.m_min;

	Vec2 startPosXY( startPos.x, startPos.y );
	Vec2 fwdXY( fwdNormal.x, fwdNormal.y );
	Vec2 startToCenter = cylinderBaseCenter - startPosXY;

	float fwdLengthSquared = DotProduct2D( fwdXY, fwdXY );
	float radiusSquared = cylinderRadius * cylinderRadius;

	if ( fwdLengthSquared == 0.f )
	{
		Vec2 startToCenterXY = startPosXY - cylinderBaseCenter;
		if ( DotProduct2D( startToCenterXY, startToCenterXY ) > radiusSquared )
		{
			return result;
		}
	}
	else
	{
		float tProjected = DotProduct2D( startToCenter, fwdXY ) / fwdLengthSquared;
		Vec2 closestXY = startPosXY + ( fwdXY * tProjected );

		Vec2 closestToCenter = cylinderBaseCenter - closestXY;
		float closestDistSquared = DotProduct2D( closestToCenter, closestToCenter );
		if ( closestDistSquared > radiusSquared )
		{
			return result;
		}

		float tOffset = sqrtf( ( radiusSquared - closestDistSquared ) / fwdLengthSquared );
		float tNear = tProjected - tOffset;
		float tFar = tProjected + tOffset;

		tRange.m_min = fmaxf( tRange.m_min, tNear );
		tRange.m_max = fminf( tRange.m_max, tFar );
		if ( tRange.m_min > tRange.m_max )
		{
			return result;
		}
	}

	float tEnter = tRange.m_min;
	if ( tEnter > maxDist )
	{
		return result;
	}

	if ( fwdLengthSquared != 0.f )
	{
		Vec3 impactPos = startPos + ( fwdNormal * tEnter );
		Vec2 impactToCenterXY( impactPos.x - cylinderBaseCenter.x, impactPos.y - cylinderBaseCenter.y );
		nearNormalXY = Vec3( impactToCenterXY.x, impactToCenterXY.y, 0.f ).GetNormalized();
	}

	Vec3 impactNormal = nearNormalZ;
	if ( tEnter > tEnterZ )
	{
		impactNormal = nearNormalXY;
	}

	result.m_didImpact = true;
	result.m_impactDist = tEnter;
	result.m_impactPos = startPos + ( fwdNormal * tEnter );
	result.m_impactNormal = impactNormal;
	return result;
}


//-----------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsOBB3D( Vec3 startPos, Vec3 fwdNormal, float maxDist, OBB3 box )
{
	RaycastResult3D result;
	result.m_rayStartPos = startPos;
	result.m_rayFwdNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;
	result.m_didImpact = false;

	if ( maxDist < 0.f )
	{
		return result;
	}

	if ( IsPointInsideOBB3D( startPos, box ) )
	{
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	Vec3 localStart = box.GetLocalPosForWorldPos( startPos );
	Vec3 localFwdNormal( DotProduct3D( fwdNormal, box.m_iBasisNormal ),
		DotProduct3D( fwdNormal, box.m_jBasisNormal ),
		DotProduct3D( fwdNormal, box.m_kBasisNormal ) );

	AABB3 localBox( -box.m_halfDimensions, box.m_halfDimensions );
	RaycastResult3D localResult = RaycastVsAABB3D( localStart, localFwdNormal, maxDist, localBox );
	if ( !localResult.m_didImpact )
	{
		return result;
	}

	result.m_didImpact = true;
	result.m_impactDist = localResult.m_impactDist;
	result.m_impactPos = box.GetWorldPosForLocalPos( localResult.m_impactPos );
	result.m_impactNormal = ( box.m_iBasisNormal * localResult.m_impactNormal.x )
		+ ( box.m_jBasisNormal * localResult.m_impactNormal.y )
		+ ( box.m_kBasisNormal * localResult.m_impactNormal.z );
	result.m_impactNormal = result.m_impactNormal.GetNormalized();
	return result;
}


//-----------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsPlane3D( Vec3 startPos, Vec3 fwdNormal, float maxDist, Plane3 plane )
{
	RaycastResult3D result;
	result.m_rayStartPos = startPos;
	result.m_rayFwdNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;
	result.m_didImpact = false;

	if ( maxDist < 0.f )
	{
		return result;
	}

	float distanceToPlane = DotProduct3D( plane.m_normal, startPos ) - plane.m_distanceAlongNormalFromOrigin;
	float rayDot = DotProduct3D( plane.m_normal, fwdNormal );

	if ( distanceToPlane == 0.f )
	{
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = plane.m_normal;
		if ( rayDot > 0.f )
		{
			result.m_impactNormal = -result.m_impactNormal;
		}
		return result;
	}

	if ( rayDot == 0.f )
	{
		return result;
	}

	float impactDist = -distanceToPlane / rayDot;
	if ( impactDist < 0.f || impactDist > maxDist )
	{
		return result;
	}

	Vec3 impactNormal = plane.m_normal;
	if ( rayDot > 0.f )
	{
		impactNormal = -impactNormal;
	}

	result.m_didImpact = true;
	result.m_impactDist = impactDist;
	result.m_impactPos = startPos + ( fwdNormal * impactDist );
	result.m_impactNormal = impactNormal;
	return result;
}