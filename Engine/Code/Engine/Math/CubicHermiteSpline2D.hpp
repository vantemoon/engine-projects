#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class CubicHermiteSpline2D
{
public:
	CubicHermiteSpline2D( std::vector<Vec2> const& controlPoints );
	explicit CubicHermiteSpline2D( std::vector<Vec2>&& controlPoints, std::vector<Vec2>&& tangents );
	Vec2 EvaluateAtParametric( float parametricZeroToOne ) const;
	float GetApproximateLength( int numSubdivisions = 64 ) const;
	Vec2 EvaluateAtApproximateDistance( float distanceAlongCurve, int numSubdivisions = 64 ) const;

public:
	std::vector<Vec2> m_controlPoints;
	std::vector<Vec2> m_tangents;
};