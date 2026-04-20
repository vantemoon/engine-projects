#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
const AABB2 AABB2::ZERO_TO_ONE = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
const AABB2 AABB2::NEGATIVE_ONE_TO_ONE = AABB2( Vec2( -1.f, -1.f ), Vec2( 1.f, 1.f ) );


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


//-----------------------------------------------------------------------------------------------
AABB2 AABB2::ChopLeft( float fractionFromLeft, float padding )
{
	float originalMinX = m_mins.x;
	float originalMaxX = m_maxs.x;
	float totalWidth = originalMaxX - originalMinX;
	float chopWidth = GetClamped( totalWidth * fractionFromLeft, 0.f, totalWidth );
	float splitX = originalMinX + chopWidth;

	AABB2 choppedBox = *this;
	choppedBox.m_mins.x = originalMinX;
	choppedBox.m_maxs.x = splitX - padding;
	if ( choppedBox.m_maxs.x < choppedBox.m_mins.x )
	{
		float centerX = ( originalMinX + splitX ) * 0.5f;
		choppedBox.m_mins.x = centerX;
		choppedBox.m_maxs.x = centerX;
	}

	m_mins.x = splitX + padding;
	m_maxs.x = originalMaxX;
	if ( m_maxs.x < m_mins.x )
	{
		float centerX = ( splitX + originalMaxX ) * 0.5f;
		m_mins.x = centerX;
		m_maxs.x = centerX;
	}

	return choppedBox;
}


//-----------------------------------------------------------------------------------------------
AABB2 AABB2::ChopTop( float fractionFromTop, float padding )
{
	float originalMinY = m_mins.y;
	float originalMaxY = m_maxs.y;
	float totalHeight = originalMaxY - originalMinY;
	float chopHeight = GetClamped( totalHeight * fractionFromTop, 0.f, totalHeight );
	float splitY = originalMaxY - chopHeight;

	AABB2 choppedBox = *this;
	choppedBox.m_mins.y = splitY + padding;
	choppedBox.m_maxs.y = originalMaxY;
	if ( choppedBox.m_maxs.y < choppedBox.m_mins.y )
	{
		float centerY = ( splitY + originalMaxY ) * 0.5f;
		choppedBox.m_mins.y = centerY;
		choppedBox.m_maxs.y = centerY;
	}

	m_mins.y = originalMinY;
	m_maxs.y = splitY - padding;
	if ( m_maxs.y < m_mins.y )
	{
		float centerY = ( originalMinY + splitY ) * 0.5f;
		m_mins.y = centerY;
		m_maxs.y = centerY;
	}

	return choppedBox;
}