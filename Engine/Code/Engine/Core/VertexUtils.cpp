#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Vec2.hpp"

#include <math.h>


//-----------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D( int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY )
{
	Vec2 iBasis = Vec2::MakeFromPolarDegrees( rotationDegreesAboutZ, uniformScaleXY );
	Vec2 jBasis = iBasis.GetRotatedBy90Degrees();
	for ( int vertexIndex = 0; vertexIndex < numVerts; ++vertexIndex )
	{
		TransformPositionXY3D( verts[vertexIndex].m_position, iBasis, jBasis, translationXY );
	}
}


//-----------------------------------------------------------------------------------------------
void AddVertsForDisc2D( std::vector<Vertex>& verts, Vec2 const& discCenter, float discRadius, Rgba8 const& color, int numSides )
{
	float deltaDegrees = 360.f / static_cast<float>( numSides );
	for ( int sideIndex = 0; sideIndex < numSides; ++sideIndex )
	{
		float degreesA = deltaDegrees * static_cast<float>( sideIndex );
		float degreesB = deltaDegrees * static_cast<float>( ( sideIndex + 1 ) % numSides );
		Vec2 pointA = discCenter + Vec2::MakeFromPolarDegrees( degreesA, discRadius );
		Vec2 pointB = discCenter + Vec2::MakeFromPolarDegrees( degreesB, discRadius );
		verts.push_back( Vertex( Vec3( discCenter.x, discCenter.y, 0.f ), color, Vec2( 0.5f, 0.5f ) ) );
		verts.push_back( Vertex( Vec3( pointA.x, pointA.y, 0.f ), color, Vec2( 0.5f + 0.5f * CosDegrees( degreesA ), 0.5f + 0.5f * SinDegrees( degreesA ) ) ) );
		verts.push_back( Vertex( Vec3( pointB.x, pointB.y, 0.f ), color, Vec2( 0.5f + 0.5f * CosDegrees( degreesB ), 0.5f + 0.5f * SinDegrees( degreesB ) ) ) );
	}
}


//-----------------------------------------------------------------------------------------------
void AddVertsForRing2D( std::vector<Vertex>& verts, Vec2 const& ringCenter, float ringRadius, float thickness, Rgba8 const& color, int numSides )
{
	float deltaDegrees = 360.f / static_cast<float>( numSides );
	float innerRadius = ringRadius - ( thickness * 0.5f );
	float outerRadius = ringRadius + ( thickness * 0.5f );
	for ( int sideIndex = 0; sideIndex < numSides; ++sideIndex )
	{
		float degreesA = deltaDegrees * static_cast<float>( sideIndex );
		float degreesB = deltaDegrees * static_cast<float>( ( sideIndex + 1 ) % numSides );
		Vec2 innerPointA = ringCenter + Vec2::MakeFromPolarDegrees( degreesA, innerRadius );
		Vec2 innerPointB = ringCenter + Vec2::MakeFromPolarDegrees( degreesB, innerRadius );
		Vec2 outerPointA = ringCenter + Vec2::MakeFromPolarDegrees( degreesA, outerRadius );
		Vec2 outerPointB = ringCenter + Vec2::MakeFromPolarDegrees( degreesB, outerRadius );
		verts.push_back( Vertex( Vec3( innerPointA.x, innerPointA.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
		verts.push_back( Vertex( Vec3( outerPointA.x, outerPointA.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
		verts.push_back( Vertex( Vec3( outerPointB.x, outerPointB.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
		verts.push_back( Vertex( Vec3( innerPointA.x, innerPointA.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
		verts.push_back( Vertex( Vec3( outerPointB.x, outerPointB.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
		verts.push_back( Vertex( Vec3( innerPointB.x, innerPointB.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
	}
}


//-----------------------------------------------------------------------------------------------
void AddVertsForAABB2D( std::vector<Vertex>& verts, AABB2 const& alignedBox, Rgba8 const& color )
{
	verts.push_back( Vertex( Vec3( alignedBox.m_mins.x, alignedBox.m_mins.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( alignedBox.m_maxs.x, alignedBox.m_mins.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( alignedBox.m_maxs.x, alignedBox.m_maxs.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( alignedBox.m_mins.x, alignedBox.m_mins.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( alignedBox.m_maxs.x, alignedBox.m_maxs.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( alignedBox.m_mins.x, alignedBox.m_maxs.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
}


//-----------------------------------------------------------------------------------------------
void AddVertsForAABB2D( std::vector<Vertex>& verts, AABB2 const& alignedBox, Rgba8 const& color, Vec2 uvAtMins, Vec2 uvAtMaxs )
{
	verts.push_back( Vertex( Vec3( alignedBox.m_mins.x, alignedBox.m_mins.y, 0.f ), color, Vec2( uvAtMins.x, uvAtMins.y ) ) );
	verts.push_back( Vertex( Vec3( alignedBox.m_maxs.x, alignedBox.m_mins.y, 0.f ), color, Vec2( uvAtMaxs.x, uvAtMins.y ) ) );
	verts.push_back( Vertex( Vec3( alignedBox.m_maxs.x, alignedBox.m_maxs.y, 0.f ), color, Vec2( uvAtMaxs.x, uvAtMaxs.y ) ) );
	verts.push_back( Vertex( Vec3( alignedBox.m_mins.x, alignedBox.m_mins.y, 0.f ), color, Vec2( uvAtMins.x, uvAtMins.y ) ) );
	verts.push_back( Vertex( Vec3( alignedBox.m_maxs.x, alignedBox.m_maxs.y, 0.f ), color, Vec2( uvAtMaxs.x, uvAtMaxs.y ) ) );
	verts.push_back( Vertex( Vec3( alignedBox.m_mins.x, alignedBox.m_maxs.y, 0.f ), color, Vec2( uvAtMins.x, uvAtMaxs.y ) ) );
}


//-----------------------------------------------------------------------------------------------
void AddVertsForOBB2D( std::vector<Vertex>& verts, OBB2 const& orientedBox, Rgba8 const& color )
{
	Vec2 corners[4];
	orientedBox.GetCornerPoints( corners );
	verts.push_back( Vertex( Vec3( corners[0].x, corners[0].y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( corners[1].x, corners[1].y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( corners[2].x, corners[2].y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( corners[0].x, corners[0].y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( corners[2].x, corners[2].y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( corners[3].x, corners[3].y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
}


//-----------------------------------------------------------------------------------------------
void AddVertsForCapsule2D( std::vector<Vertex>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color, int numSides )
{
	Vec2 boneVector = boneEnd - boneStart;
	Vec2 boneDirection = boneVector.GetNormalized();
	Vec2 perpendicular = boneDirection.GetRotatedBy90Degrees();
	// Add cylinder part
	Vec2 topLeft = boneStart + perpendicular * radius;
	Vec2 topRight = boneStart - perpendicular * radius;
	Vec2 bottomLeft = boneEnd + perpendicular * radius;
	Vec2 bottomRight = boneEnd - perpendicular * radius;
	verts.push_back( Vertex( Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( topRight.x, topRight.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
	// Add end caps
	float deltaDegrees = 180.f / static_cast<float>( numSides );
	Vec2 centerTop = boneStart;
	Vec2 centerBottom = boneEnd;
	for ( int sideIndex = 0; sideIndex < numSides; ++sideIndex )
	{
		float degreesA = deltaDegrees * static_cast<float>( sideIndex );
		float degreesB = deltaDegrees * static_cast<float>( sideIndex + 1 );
		// Top cap
		Vec2 pointA_Top = centerTop + ( perpendicular.GetRotatedByDegrees( degreesA ) * radius );
		Vec2 pointB_Top = centerTop + ( perpendicular.GetRotatedByDegrees( degreesB ) * radius );
		verts.push_back( Vertex( Vec3( centerTop.x, centerTop.y, 0.f ), color, Vec2( 0.5f, 0.5f ) ) );
		verts.push_back( Vertex( Vec3( pointA_Top.x, pointA_Top.y, 0.f ), color, Vec2( 0.5f + 0.5f * CosDegrees( degreesA ), 0.5f + 0.5f * SinDegrees( degreesA ) ) ) );
		verts.push_back( Vertex( Vec3( pointB_Top.x, pointB_Top.y, 0.f ), color, Vec2( 0.5f + 0.5f * CosDegrees( degreesB ), 0.5f + 0.5f * SinDegrees( degreesB ) ) ) );
		// Bottom cap
		Vec2 pointA_Bottom = centerBottom + ( ( -perpendicular ).GetRotatedByDegrees( degreesA ) * radius );
		Vec2 pointB_Bottom = centerBottom + ( ( -perpendicular ).GetRotatedByDegrees( degreesB ) * radius );
		verts.push_back( Vertex( Vec3( centerBottom.x, centerBottom.y, 0.f ), color, Vec2( 0.5f, 0.5f ) ) );
		verts.push_back( Vertex( Vec3( pointB_Bottom.x, pointB_Bottom.y, 0.f ), color, Vec2( 0.5f + 0.5f * CosDegrees( degreesB ), 0.5f + 0.5f * SinDegrees( -degreesB ) ) ) );
		verts.push_back( Vertex( Vec3( pointA_Bottom.x, pointA_Bottom.y, 0.f ), color, Vec2( 0.5f + 0.5f * CosDegrees( degreesA ), 0.5f + 0.5f * SinDegrees( -degreesA ) ) ) );
	}
}


//-----------------------------------------------------------------------------------------------
void AddVertsForTriangle2D( std::vector<Vertex>& verts, Vec2 const& ccw0, Vec2 const& ccw1, Vec2 const& ccw2, Rgba8 const& color )
{
	// Counter-clockwise
	verts.push_back( Vertex( Vec3( ccw0.x, ccw0.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( ccw1.x, ccw1.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( ccw2.x, ccw2.y, 0.f ), color, Vec2( 0.5f, 1.f ) ) );
}


//-----------------------------------------------------------------------------------------------
void AddVertsForLineSegment2D( std::vector<Vertex>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color )
{
	Vec2 lineVector = end - start;
	Vec2 lineDirection = lineVector.GetNormalized();
	Vec2 perpendicular = lineDirection.GetRotatedBy90Degrees();
	Vec2 halfThicknessOffset = perpendicular * ( thickness * 0.5f );
	Vec2 topLeft = start + halfThicknessOffset;
	Vec2 topRight = start - halfThicknessOffset;
	Vec2 bottomLeft = end + halfThicknessOffset;
	Vec2 bottomRight = end - halfThicknessOffset;
	verts.push_back( Vertex( Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( topRight.x, topRight.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
}


//-----------------------------------------------------------------------------------------------
void AddVertsForArrow2D( std::vector<Vertex>& verts, Vec2 const& startTail, Vec2 const& endTip, float thickness, float arrowRadius, Rgba8 const& color )
{
	if ( startTail == endTip ) return;
	if ( arrowRadius <= 0.f ) return;
	if ( thickness <= 0.f ) return;

	float lineLength = GetDistance2D( startTail, endTip );
	if ( lineLength < arrowRadius ) return;

	Vec2 lineVector = endTip - startTail;
	Vec2 lineDirection = lineVector.GetNormalized();
	Vec2 perpendicular = lineDirection.GetRotatedBy90Degrees();
	Vec2 halfThicknessOffset = perpendicular * ( thickness * 0.5f );
	Vec2 shaftEnd = endTip - ( lineDirection * arrowRadius );

	// Add shaft
	Vec2 topLeft = startTail + halfThicknessOffset;
	Vec2 topRight = startTail - halfThicknessOffset;
	Vec2 bottomLeft = shaftEnd + halfThicknessOffset;
	Vec2 bottomRight = shaftEnd - halfThicknessOffset;
	verts.push_back( Vertex( Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( topRight.x, topRight.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );

	// Add arrowhead
	Vec2 leftPoint = shaftEnd + ( perpendicular * arrowRadius );
	Vec2 rightPoint = shaftEnd - ( perpendicular * arrowRadius );
	verts.push_back( Vertex( Vec3( endTip.x, endTip.y, 0.f ), color, Vec2( 0.5f, 0.5f ) ) );
	verts.push_back( Vertex( Vec3( leftPoint.x, leftPoint.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( rightPoint.x, rightPoint.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
}


//-----------------------------------------------------------------------------------------------
void AddVertsForQuad3D( std::vector<Vertex>& verts, 
						Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, 
						Rgba8 const& color, AABB2 const& UVs )
{
	verts.push_back( Vertex( bottomLeft, color, Vec2( UVs.m_mins.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex( bottomRight, color, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex( topRight, color, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex( bottomLeft, color, Vec2( UVs.m_mins.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex( topRight, color, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex( topLeft, color, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );
}