#include "Engine/Math/CubicHermiteCurve2D.hpp"
#include "Engine/Math/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
CubicHermiteCurve2D::CubicHermiteCurve2D( Vec2 startPos, Vec2 startTangent, Vec2 endPos, Vec2 endTangent )
	: m_startPos( startPos )
	, m_startTangent( startTangent )
	, m_endPos( endPos )
	, m_endTangent( endTangent )
{
}


//-----------------------------------------------------------------------------------------------
CubicHermiteCurve2D::CubicHermiteCurve2D( CubicBezierCurve2D const& fromBezier )
	: m_startPos( fromBezier.m_startPos )
	, m_startTangent( fromBezier.m_guidePos1 - fromBezier.m_startPos )
	, m_endPos( fromBezier.m_endPos )
	, m_endTangent( fromBezier.m_guidePos2 - fromBezier.m_endPos )
{
}


//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::EvaluateAtParametric( float parametricZeroToOne ) const
{
	float t = parametricZeroToOne;
	float oneMinusT = 1.f - t;
	Vec2 result =
		( oneMinusT * oneMinusT * oneMinusT * m_startPos ) +
		( 3.f * oneMinusT * oneMinusT * t * ( m_startPos + m_startTangent ) ) +
		( 3.f * oneMinusT * t * t * ( m_endPos - m_endTangent ) ) +
		( t * t * t * m_endPos );
	return result;
}


//-----------------------------------------------------------------------------------------------
float CubicHermiteCurve2D::GetApproximateLength( int numSubdivisions ) const
{
	if ( numSubdivisions <= 0 )
	{
		return 0.f;
	}

	float length = 0.f;
	Vec2 prevPoint = m_startPos;

	for ( int segmentIndex = 0; segmentIndex < numSubdivisions; ++ segmentIndex )
	{
		float t = static_cast<float>( segmentIndex + 1 ) / static_cast<float>( numSubdivisions );
		Vec2 pointOnCurve = EvaluateAtParametric( t );
		length += GetDistance2D( prevPoint, pointOnCurve );
		prevPoint = pointOnCurve;
	}

	return length;
}


//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::EvaluateAtApproximateDistance( float distanceAlongCurve, int numSubdivisions ) const
{
	if ( numSubdivisions <= 0 )
	{
		return m_startPos;
	}

	if ( distanceAlongCurve <= 0.f )
	{
		return m_startPos;
	}

	float totalLength = GetApproximateLength( numSubdivisions );
	if ( distanceAlongCurve >= totalLength )
	{
		return m_endPos;
	}

	float lengthSoFar = 0.f;
	Vec2 prevPoint = m_startPos;

	for ( int segmentIndex = 0; segmentIndex < numSubdivisions; ++ segmentIndex )
	{
		float t = static_cast<float>( segmentIndex + 1 ) / static_cast<float>( numSubdivisions );
		Vec2 pointOnCurve = EvaluateAtParametric( t );
		lengthSoFar += GetDistance2D( prevPoint, pointOnCurve );
		if ( lengthSoFar >= distanceAlongCurve )
		{
			return pointOnCurve;
		}
		prevPoint = pointOnCurve;
	}

	return m_endPos;
}


//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::GetStartVelocity() const
{
	return m_startTangent;
}


//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::GetEndVelocity() const
{
	return m_endTangent;
}


//-----------------------------------------------------------------------------------------------
void CubicHermiteCurve2D::SetStartVelocity( Vec2 newStartVelocity )
{
	m_startTangent = newStartVelocity;
}


//-----------------------------------------------------------------------------------------------
void CubicHermiteCurve2D::SetEndVelocity( Vec2 newEndVelocity )
{
	m_endTangent = newEndVelocity;
}