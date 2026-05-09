#include "Engine/Math/AABB3.hpp"


//-----------------------------------------------------------------------------------------------
AABB3::AABB3( AABB3 const& copyFrom )
	: m_mins( copyFrom.m_mins )
	, m_maxs( copyFrom.m_maxs )
{
}


//-----------------------------------------------------------------------------------------------
AABB3::AABB3( float minX, float minY, float minZ, float maxX, float maxY, float maxZ )
	: m_mins( minX, minY, minZ )
	, m_maxs( maxX, maxY, maxZ )
{
}


//-----------------------------------------------------------------------------------------------
AABB3::AABB3( Vec3 const& mins, Vec3 const& maxs )
	: m_mins( mins )
	, m_maxs( maxs )
{
}


//-----------------------------------------------------------------------------------------------
bool AABB3::IsPointInside( Vec3 const& point ) const
{
	if ( point.x < m_mins.x || point.x > m_maxs.x )
		return false;
	if ( point.y < m_mins.y || point.y > m_maxs.y )
		return false;
	if ( point.z < m_mins.z || point.z > m_maxs.z )
		return false;
	return true;
}


//-----------------------------------------------------------------------------------------------
void AABB3::GetCornerPoints( Vec3* out_eightCornerWorldPositions ) const
{
	out_eightCornerWorldPositions[0] = Vec3( m_mins.x, m_mins.y, m_mins.z );
	out_eightCornerWorldPositions[1] = Vec3( m_maxs.x, m_mins.y, m_mins.z );
	out_eightCornerWorldPositions[2] = Vec3( m_mins.x, m_maxs.y, m_mins.z );
	out_eightCornerWorldPositions[3] = Vec3( m_maxs.x, m_maxs.y, m_mins.z );
	out_eightCornerWorldPositions[4] = Vec3( m_mins.x, m_mins.y, m_maxs.z );
	out_eightCornerWorldPositions[5] = Vec3( m_maxs.x, m_mins.y, m_maxs.z );
	out_eightCornerWorldPositions[6] = Vec3( m_mins.x, m_maxs.y, m_maxs.z );
	out_eightCornerWorldPositions[7] = Vec3( m_maxs.x, m_maxs.y, m_maxs.z );
}