#pragma once
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
class CubicBezierCurve2D;


//-----------------------------------------------------------------------------------------------
class CubicHermiteCurve2D
{
public:
	CubicHermiteCurve2D( Vec2 startPos, Vec2 startTangent, Vec2 endPos, Vec2 endTangent );
	explicit CubicHermiteCurve2D( CubicBezierCurve2D const& fromBezier );
	Vec2 EvaluateAtParametric( float parametricZeroToOne ) const;
	float GetApproximateLength( int numSubdivisions = 64 ) const;
	Vec2 EvaluateAtApproximateDistance( float distanceAlongCurve, int numSubdivisions = 64 ) const;

	Vec2 GetStartVelocity() const;
	Vec2 GetEndVelocity() const;
	void SetStartVelocity( Vec2 newStartVelocity );
	void SetEndVelocity( Vec2 newEndVelocity );

public:
	Vec2 m_startPos;
	Vec2 m_startTangent;
	Vec2 m_endPos;
	Vec2 m_endTangent;
};