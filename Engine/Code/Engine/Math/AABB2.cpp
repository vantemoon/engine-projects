#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
AABB2::AABB2( AABB2 const& copyFrom )
	: m_mins( copyFrom.m_mins )
	, m_maxs( copyFrom.m_maxs )
{
}


//-----------------------------------------------------------------------------------------------
AABB2::AABB2( float minX, float minY, float maxX, float maxY )
	: m_mins( Vec2( minX, minY ) )
	, m_maxs( Vec2( maxX, maxY ) )
{
}


//-----------------------------------------------------------------------------------------------
AABB2::AABB2( Vec2 const& mins, Vec2 const& maxs )
	: m_mins( mins )
	, m_maxs( maxs )
{
}


//-----------------------------------------------------------------------------------------------
bool AABB2::IsPointInside( Vec2 const& point ) const
{
	if ( point.x < m_mins.x )
	{
		return false;
	}
	if ( point.x > m_maxs.x )
	{
		return false;
	}
	if ( point.y < m_mins.y )
	{
		return false;
	}
	if ( point.y > m_maxs.y )
	{
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------------------------
Vec2 const AABB2::GetCenter() const
{
	float centerX = ( m_mins.x + m_maxs.x ) / 2.f;
	float centerY = ( m_mins.y + m_maxs.y ) / 2.f;
	return Vec2( centerX, centerY );
}


//-----------------------------------------------------------------------------------------------
Vec2 const AABB2::GetDimensions() const
{
	float dimX = m_maxs.x - m_mins.x;
	float dimY = m_maxs.y - m_mins.y;
	return Vec2( dimX, dimY );
}


//-----------------------------------------------------------------------------------------------
Vec2 const AABB2::GetNearestPoint( Vec2 const& point ) const
{
	float nearestX = GetClamped( point.x, m_mins.x, m_maxs.x );
	float nearestY = GetClamped( point.y, m_mins.y, m_maxs.y );
	return Vec2( nearestX, nearestY );
}


//-----------------------------------------------------------------------------------------------
Vec2 const AABB2::GetPointAtUV( Vec2 const& uv ) const
{
	float pointX = Interpolate( m_mins.x, m_maxs.x, uv.x );
	float pointY = Interpolate( m_mins.y, m_maxs.y, uv.y );
	return Vec2( pointX, pointY );
}


//-----------------------------------------------------------------------------------------------
Vec2 const AABB2::GetUVForPoint( Vec2 const& point ) const
{
	float u = GetFractionWithinRange( point.x, m_mins.x, m_maxs.x );
	float v = GetFractionWithinRange( point.y, m_mins.y, m_maxs.y );
	return Vec2( u, v );
}


//-----------------------------------------------------------------------------------------------
void AABB2::Translate( Vec2 const& translationToApply )
{
	m_mins += translationToApply;
	m_maxs += translationToApply;
}


//-----------------------------------------------------------------------------------------------
void AABB2::SetCenter( Vec2 const& newCenter )
{
	Vec2 currentCenter = GetCenter();
	Vec2 translation = newCenter - currentCenter;
	Translate( translation );
}


//-----------------------------------------------------------------------------------------------
void AABB2::SetDimensions( Vec2 const& newDimensions )
{
	Vec2 currentCenter = GetCenter();
	Vec2 halfDimensions = newDimensions * 0.5f;
	m_mins = currentCenter - halfDimensions;
	m_maxs = currentCenter + halfDimensions;
}


//-----------------------------------------------------------------------------------------------
void AABB2::StretchToIncludePoint( Vec2 const& point )
{
	if ( point.x < m_mins.x )
	{
		m_mins.x = point.x;
	}
	if ( point.x > m_maxs.x )
	{
		m_maxs.x = point.x;
	}
	if ( point.y < m_mins.y )
	{
		m_mins.y = point.y;
	}
	if ( point.y > m_maxs.y )
	{
		m_maxs.y = point.y;
	}
}