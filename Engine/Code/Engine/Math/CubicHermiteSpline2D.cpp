#include "Engine/Math/CubicHermiteSpline2D.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
CubicHermiteSpline2D::CubicHermiteSpline2D( std::vector<Vec2> const& controlPoints )
	: m_controlPoints( controlPoints )
{
	m_tangents.clear();
	m_tangents.reserve( m_controlPoints.size() );

	if ( m_controlPoints.empty() )
	{
		return;
	}

	if ( m_controlPoints.size() == 1 )
	{
		m_tangents.push_back( Vec2::ZERO );
		return;
	}

	for ( size_t i = 0; i < m_controlPoints.size(); ++i )
	{
		Vec2 prevPoint = m_controlPoints[( i - 1 + m_controlPoints.size() ) % m_controlPoints.size()];
		Vec2 nextPoint = m_controlPoints[( i + 1 ) % m_controlPoints.size()];
		Vec2 tangent = ( nextPoint - prevPoint ) * 0.5f;
		m_tangents.push_back( tangent );
	}

	m_tangents[0] = Vec2::ZERO;
	m_tangents[m_tangents.size() - 1] = Vec2::ZERO;
}


//-----------------------------------------------------------------------------------------------
CubicHermiteSpline2D::CubicHermiteSpline2D( std::vector<Vec2>&& controlPoints, std::vector<Vec2>&& tangents )
	: m_controlPoints( std::move( controlPoints ) )
	, m_tangents( std::move( tangents ) )
{
	if ( m_tangents.size() != m_controlPoints.size() )
	{
		m_tangents.assign( m_controlPoints.size(), Vec2::ZERO );
	}
}


//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteSpline2D::EvaluateAtParametric( float parametricZeroToOne ) const
{
	size_t const pointCount = m_controlPoints.size();
	if ( pointCount == 0 )
	{
		return Vec2::ZERO;
	}
	if ( pointCount == 1 )
	{
		return m_controlPoints[0];
	}
	if ( m_tangents.size() < pointCount )
	{
		return m_controlPoints[0];
	}

	float const clampedT = GetClampedZeroToOne( parametricZeroToOne );
	int const numSegments = static_cast<int>( pointCount ) - 1;

	float const scaledT = clampedT * static_cast<float>( numSegments );
	int segmentIndex = static_cast<int>( scaledT );
	float localT = scaledT - static_cast<float>( segmentIndex );

	if ( segmentIndex >= numSegments )
	{
		segmentIndex = numSegments - 1;
		localT = 1.f;
	}

	Vec2 const& p0 = m_controlPoints[segmentIndex];
	Vec2 const& p1 = m_controlPoints[segmentIndex + 1];
	Vec2 const& m0 = m_tangents[segmentIndex];
	Vec2 const& m1 = m_tangents[segmentIndex + 1];

	float const tSquared = localT * localT;
	float const tCubed = tSquared * localT;

	float const h00 = ( 2.f * tCubed ) - ( 3.f * tSquared ) + 1.f;
	float const h10 = tCubed - ( 2.f * tSquared ) + localT;
	float const h01 = ( -2.f * tCubed ) + ( 3.f * tSquared );
	float const h11 = tCubed - tSquared;

	return ( h00 * p0 ) + ( h10 * m0 ) + ( h01 * p1 ) + ( h11 * m1 );
}


//-----------------------------------------------------------------------------------------------
float CubicHermiteSpline2D::GetApproximateLength( int numSubdivisions ) const
{
	if ( m_controlPoints.size() < 2 || numSubdivisions <= 0 )
	{
		return 0.f;
	}

	float totalLength = 0.f;
	Vec2 previousPoint = EvaluateAtParametric( 0.f );

	for ( int segmentIndex = 0; segmentIndex < numSubdivisions; ++segmentIndex )
	{
		float const t = static_cast<float>( segmentIndex + 1 ) / static_cast<float>( numSubdivisions );
		Vec2 const currentPoint = EvaluateAtParametric( t );
		totalLength += ( currentPoint - previousPoint ).GetLength();
		previousPoint = currentPoint;
	}

	return totalLength;
}


//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteSpline2D::EvaluateAtApproximateDistance( float distanceAlongCurve, int numSubdivisions ) const
{
	if ( m_controlPoints.empty() )
	{
		return Vec2::ZERO;
	}
	if ( m_controlPoints.size() == 1 )
	{
		return m_controlPoints[0];
	}

	int const subdivisions = ( numSubdivisions > 0 ) ? numSubdivisions : 1;
	float const totalLength = GetApproximateLength( subdivisions );
	if ( totalLength <= 0.f )
	{
		return m_controlPoints[0];
	}

	float const clampedDistance = GetClamped( distanceAlongCurve, 0.f, totalLength );
	if ( clampedDistance <= 0.f )
	{
		return EvaluateAtParametric( 0.f );
	}
	if ( clampedDistance >= totalLength )
	{
		return EvaluateAtParametric( 1.f );
	}

	float accumulatedLength = 0.f;
	float previousT = 0.f;
	Vec2 previousPoint = EvaluateAtParametric( previousT );

	for ( int subdivisionIndex = 1; subdivisionIndex <= subdivisions; ++subdivisionIndex )
	{
		float const currentT = static_cast<float>( subdivisionIndex ) / static_cast<float>( subdivisions );
		Vec2 const currentPoint = EvaluateAtParametric( currentT );
		float const segmentLength = ( currentPoint - previousPoint ).GetLength();
		float const nextAccumulatedLength = accumulatedLength + segmentLength;

		if ( clampedDistance <= nextAccumulatedLength )
		{
			if ( segmentLength <= 0.f )
			{
				return currentPoint;
			}

			float const segmentFraction = ( clampedDistance - accumulatedLength ) / segmentLength;
			float const mappedT = Interpolate( previousT, currentT, segmentFraction );
			return EvaluateAtParametric( mappedT );
		}

		accumulatedLength = nextAccumulatedLength;
		previousT = currentT;
		previousPoint = currentPoint;
	}

	return EvaluateAtParametric( 1.f );
}