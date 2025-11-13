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
