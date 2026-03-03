#include "Engine/Math/RaycastUtils.hpp"
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