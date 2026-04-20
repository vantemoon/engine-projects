#include "Engine/Math/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
CubicBezierCurve2D::CubicBezierCurve2D( Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos )
	: m_startPos( startPos )
	, m_guidePos1( guidePos1 )
	, m_guidePos2( guidePos2 )
	, m_endPos( endPos )
{
}


//-----------------------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::EvaluateAtParametric( float parametricZeroToOne ) const
{
	float t = parametricZeroToOne;
	float oneMinusT = 1.f - t;
	Vec2 result =
		( oneMinusT * oneMinusT * oneMinusT * m_startPos ) +
		( 3.f * oneMinusT * oneMinusT * t * m_guidePos1 ) +
		( 3.f * oneMinusT * t * t * m_guidePos2 ) +
		( t * t * t * m_endPos );
	return result;
}


//-----------------------------------------------------------------------------------------------
float CubicBezierCurve2D::GetApproximateLength( int numSubdivisions ) const
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
Vec2 CubicBezierCurve2D::EvaluateAtApproximateDistance( float distanceAlongCurve, int numSubdivisions ) const
{
	if ( numSubdivisions <= 0 )
	{
		return m_startPos;
	}

	if ( distanceAlongCurve <= 0.f )
	{
		return m_startPos;
	}

	float distanceAccumulated = 0.f;
	Vec2 previousPoint = m_startPos;

	for ( int segmentIndex = 0; segmentIndex < numSubdivisions; ++ segmentIndex )
	{
		float const nextT = static_cast<float>( segmentIndex + 1 ) / static_cast<float>( numSubdivisions );
		Vec2 const nextPoint = EvaluateAtParametric( nextT );
		float const segmentLength = GetDistance2D( previousPoint, nextPoint );

		if ( segmentLength > 0.f && ( distanceAccumulated + segmentLength ) >= distanceAlongCurve )
		{
			float const distanceIntoSegment = distanceAlongCurve - distanceAccumulated;
			float const segmentFraction = distanceIntoSegment / segmentLength;
			return previousPoint + ( nextPoint - previousPoint ) * segmentFraction;
		}

		distanceAccumulated += segmentLength;
		previousPoint = nextPoint;
	}

	return m_endPos;
}