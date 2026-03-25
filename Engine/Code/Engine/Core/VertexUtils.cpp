#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Mat44.hpp"
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
void TransformVertexArray3D( std::vector<Vertex>& verts, Mat44 const& transform )
{
	for ( size_t vertexIndex = 0; vertexIndex < verts.size(); ++vertexIndex )
	{
		Vec3& position = verts[vertexIndex].m_position;
		position = transform.TransformPosition3D( position );
	}
}


//-----------------------------------------------------------------------------------------------
AABB2 GetVertexBounds2D( std::vector<Vertex> const& verts )
{
	if ( verts.empty() )
	{
		return AABB2();
	}

	AABB2 bounds;
	bounds.m_mins = Vec2( verts[0].m_position.x, verts[0].m_position.y );
	bounds.m_maxs = bounds.m_mins;

	for ( size_t vertexIndex = 1; vertexIndex < verts.size(); ++vertexIndex )
	{
		Vec3 const& position = verts[vertexIndex].m_position;
		if ( position.x < bounds.m_mins.x )
		{
			bounds.m_mins.x = position.x;
		}
		else if ( position.x > bounds.m_maxs.x )
		{
			bounds.m_maxs.x = position.x;
		}
		if ( position.y < bounds.m_mins.y )
		{
			bounds.m_mins.y = position.y;
		}
		else if ( position.y > bounds.m_maxs.y )
		{
			bounds.m_maxs.y = position.y;
		}
	}

	return bounds;
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
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( topRight.x, topRight.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
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
		verts.push_back( Vertex( Vec3( pointA_Bottom.x, pointA_Bottom.y, 0.f ), color, Vec2( 0.5f + 0.5f * CosDegrees( degreesA ), 0.5f + 0.5f * SinDegrees( -degreesA ) ) ) );
		verts.push_back( Vertex( Vec3( pointB_Bottom.x, pointB_Bottom.y, 0.f ), color, Vec2( 0.5f + 0.5f * CosDegrees( degreesB ), 0.5f + 0.5f * SinDegrees( -degreesB ) ) ) );
	}
}


//-----------------------------------------------------------------------------------------------
void AddVertsForTriangle2D( std::vector<Vertex>& verts, Vec2 const& ccw0, Vec2 const& ccw1, Vec2 const& ccw2, Rgba8 const& color, 
							Vec2 const& uv0, Vec2 const& uv1, Vec2 const& uv2 )
{
	// Counter-clockwise
	verts.push_back( Vertex( Vec3( ccw0.x, ccw0.y, 0.f ), color, uv0 ) );
	verts.push_back( Vertex( Vec3( ccw1.x, ccw1.y, 0.f ), color, uv1 ) );
	verts.push_back( Vertex( Vec3( ccw2.x, ccw2.y, 0.f ), color, uv2 ) );
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
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( topRight.x, topRight.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
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
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( topRight.x, topRight.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );

	// Add arrowhead
	Vec2 leftPoint = shaftEnd + ( perpendicular * arrowRadius );
	Vec2 rightPoint = shaftEnd - ( perpendicular * arrowRadius );
	verts.push_back( Vertex( Vec3( endTip.x, endTip.y, 0.f ), color, Vec2( 0.5f, 0.5f ) ) );
	verts.push_back( Vertex( Vec3( leftPoint.x, leftPoint.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( rightPoint.x, rightPoint.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
}


//-----------------------------------------------------------------------------------------------
void AddVertsForTriangle3D( std::vector<Vertex>& verts, Vec3 const& ccw0, Vec3 const& ccw1, Vec3 const& ccw2, Rgba8 const& color,
							Vec2 const& uv0, Vec2 const& uv1, Vec2 const& uv2 )
{
	verts.emplace_back( ccw0, color, uv0 );
	verts.emplace_back( ccw1, color, uv1 );
	verts.emplace_back( ccw2, color, uv2 );
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


//-----------------------------------------------------------------------------------------------
void AddVertsForQuad3D( std::vector<Vertex>& verts, std::vector<unsigned int>& indices,
						Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft,
						Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs )
{
	verts.push_back( Vertex( bottomLeft, color, Vec2( UVs.m_mins.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex( bottomRight, color, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex( topRight, color, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex( bottomLeft, color, Vec2( UVs.m_mins.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex( topRight, color, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex( topLeft, color, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );

	unsigned int baseIndex = static_cast< unsigned int >( verts.size() ) - 4;
	indices.push_back( baseIndex + 0 );
	indices.push_back( baseIndex + 1 );
	indices.push_back( baseIndex + 2 );
	indices.push_back( baseIndex + 0 );
	indices.push_back( baseIndex + 2 );
	indices.push_back( baseIndex + 3 );
}


//-----------------------------------------------------------------------------------------------
void AddVertsForAABB3D( std::vector<Vertex>& verts, AABB3 const& bounds, Rgba8 const& color, AABB2 const& UVs )
{
	Vec3 bottomLeftFront( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z );
	Vec3 bottomRightFront( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z );
	Vec3 topRightFront( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z );
	Vec3 topLeftFront( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z );
	Vec3 bottomLeftBack( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z );
	Vec3 bottomRightBack( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z );
	Vec3 topRightBack( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z );
	Vec3 topLeftBack( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z );

	AddVertsForQuad3D( verts, bottomRightFront, bottomLeftFront, topLeftFront, topRightFront, color, UVs );
	AddVertsForQuad3D( verts, bottomLeftBack, bottomRightBack, topRightBack, topLeftBack, color, UVs );
	AddVertsForQuad3D( verts, bottomLeftFront, bottomLeftBack, topLeftBack, topLeftFront, color, UVs );
	AddVertsForQuad3D( verts, bottomRightBack, bottomRightFront, topRightFront, topRightBack, color, UVs );
	AddVertsForQuad3D( verts, topLeftBack, topRightBack, topRightFront, topLeftFront, color, UVs );
	AddVertsForQuad3D( verts, bottomLeftFront, bottomRightFront, bottomRightBack, bottomLeftBack, color, UVs );
}


//-----------------------------------------------------------------------------------------------
void AddVertsForSphere3D( std::vector<Vertex>& verts, Vec3 const& center, float radius, Rgba8 const& color,
						  AABB2 const& UVs, int numSlices, int numStacks )
{
	if ( numSlices <= 0 || numStacks <= 0 )
	{
		return;
	}

	float degreesPerStack = 180.f / static_cast<float>( numStacks );
	float degreesPerSlice = 360.f / static_cast<float>( numSlices );

	for ( int i = 0; i < numStacks; ++i )
	{
		for ( int j = 0; j < numSlices; ++j )
		{
			float leftDegrees = degreesPerSlice * static_cast<float>( j );
			float rightDegrees = degreesPerSlice * static_cast<float>( j + 1 );
			float topDegrees = degreesPerStack * static_cast<float>( i ) - 90.f;
			float bottomDegrees = degreesPerStack * static_cast<float>( i + 1 ) - 90.f;

			Vec3 bottomLeft = center + Vec3::MakeFromPolarDegrees( bottomDegrees, leftDegrees, radius );
			Vec3 bottomRight = center + Vec3::MakeFromPolarDegrees( bottomDegrees, rightDegrees, radius );
			Vec3 topLeft = center + Vec3::MakeFromPolarDegrees( topDegrees, leftDegrees, radius );
			Vec3 topRight = center + Vec3::MakeFromPolarDegrees( topDegrees, rightDegrees, radius );

			float topV = ( float ) 1 - i / static_cast< float >( numStacks );
			float bottomV = ( float ) 1 - ( i + 1 ) / static_cast< float >( numStacks );
			float leftU = ( float ) j / static_cast<float>( numSlices );
			float rightU = ( float ) ( j + 1 ) / static_cast<float>( numSlices );

			float uMin = UVs.m_mins.x + ( UVs.m_maxs.x - UVs.m_mins.x ) * leftU;
			float uMax = UVs.m_mins.x + ( UVs.m_maxs.x - UVs.m_mins.x ) * rightU;
			float vMin = UVs.m_mins.y + ( UVs.m_maxs.y - UVs.m_mins.y ) * bottomV;
			float vMax = UVs.m_mins.y + ( UVs.m_maxs.y - UVs.m_mins.y ) * topV;

			float uPole = 0.5f * ( uMin + uMax );

			if ( i == 0 )
			{
				Vec3 pole = topLeft;
				AddVertsForTriangle3D( verts, pole, bottomLeft, bottomRight, color, Vec2( uPole, vMax ), Vec2( uMin, vMin ), Vec2( uMax, vMin ) );
			}
			else if ( i == numStacks - 1 )
			{
				Vec3 pole = bottomLeft;
				AddVertsForTriangle3D( verts, pole, topRight, topLeft, color, Vec2( uPole, vMin ), Vec2( uMax, vMax ), Vec2( uMin, vMax ) );
			}
			else
			{
				AddVertsForQuad3D( verts, bottomLeft, bottomRight, topRight, topLeft, color, AABB2( Vec2( uMin, vMin ), Vec2( uMax, vMax ) ) );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void AddvertsForXYGrid3D( std::vector<Vertex>& verts, Vec3 const& center, float cellSize, int numCellsX, int numCellsY )
{
	if ( cellSize <= 0.f || numCellsX <= 0 || numCellsY <= 0 )
	{
		return;
	}

	float halfSizeX = 0.5f * cellSize * static_cast< float >( numCellsX );
	float halfSizeY = 0.5f * cellSize * static_cast< float >( numCellsY );

	float thinThickness = 0.04f;
	float majorThickness = 0.08f;
	float axisThickness = 0.12f;

	unsigned char thinBrightness = 150;
	unsigned char majorBrightness = 200;
	unsigned char axisBrightness = 255;

	int halfCellsX = numCellsX / 2;
	int halfCellsY = numCellsY / 2;

	for ( int j = -halfCellsY; j <= halfCellsY; ++ j )
	{
		float y = center.y + static_cast< float >( j ) * cellSize;

		float thickness;
		unsigned char brightness;
		bool isAxis = ( j == 0 );
		bool isMajor = ( !isAxis && ( j % 5 == 0 ) );

		if ( isAxis )
		{
			thickness = axisThickness;
			brightness = axisBrightness;
		}
		else if ( isMajor )
		{
			thickness = majorThickness;
			brightness = majorBrightness;
		}
		else
		{
			thickness = thinThickness;
			brightness = thinBrightness;
		}

		Rgba8 color;
		if ( isAxis || isMajor )
		{
			color = Rgba8( brightness, 0, 0, 255 );
		}
		else
		{
			color = Rgba8( brightness, brightness, brightness, 255 );
		}

		AABB3 line;
		float halfThicknessZ = 0.5f * thickness;
		line.m_mins = Vec3( center.x - halfSizeX, y - 0.5f * thickness, center.z - halfThicknessZ );
		line.m_maxs = Vec3( center.x + halfSizeX, y + 0.5f * thickness, center.z + halfThicknessZ );

		AddVertsForAABB3D( verts, line, color );
	}

	for ( int i = -halfCellsX; i <= halfCellsX; ++ i )
	{
		float x = center.x + static_cast< float >( i ) * cellSize;

		float thickness;
		unsigned char brightness;
		bool isAxis = ( i == 0 );
		bool isMajor = ( !isAxis && ( i % 5 == 0 ) );

		if ( isAxis )
		{
			thickness = axisThickness;
			brightness = axisBrightness;
		}
		else if ( isMajor )
		{
			thickness = majorThickness;
			brightness = majorBrightness;
		}
		else
		{
			thickness = thinThickness;
			brightness = thinBrightness;
		}

		Rgba8 color;
		if ( isAxis || isMajor )
		{
			color = Rgba8( 0, brightness, 0, 255 );
		}
		else
		{
			color = Rgba8( brightness, brightness, brightness, 255 );
		}

		AABB3 line;
		float halfThicknessZ = 0.5f * thickness;
		line.m_mins = Vec3( x - 0.5f * thickness, center.y - halfSizeY, center.z - halfThicknessZ );
		line.m_maxs = Vec3( x + 0.5f * thickness, center.y + halfSizeY, center.z + halfThicknessZ );

		AddVertsForAABB3D( verts, line, color );
	}
}


void AddVertsForCylinder3D( std::vector<Vertex>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color,
							AABB2 const& UVs, int numSlices )
{
	if ( numSlices <= 0 || radius <= 0.f || start == end )
	{
		return;
	}

	Vec3 lookAtForwardI = ( end - start ).GetNormalized();
	Vec3 lookAtLeftJ;
	Vec3 lookAtUpK;

	Vec3 worldZAxis( 0.f, 0.f, 1.f );
	Vec3 worldYAxis( 0.f, 1.f, 0.f );

	if ( abs( DotProduct3D( lookAtForwardI, worldZAxis ) ) < 0.99999f )
	{
		lookAtLeftJ = CrossProduct3D( worldZAxis, lookAtForwardI ).GetNormalized();
		lookAtUpK = CrossProduct3D( lookAtForwardI, lookAtLeftJ ).GetNormalized();
	}
	else
	{
		lookAtUpK = CrossProduct3D( lookAtForwardI, worldYAxis ).GetNormalized();
		lookAtLeftJ = CrossProduct3D( lookAtUpK, lookAtForwardI ).GetNormalized();
	}

	float uCenter = 0.5f * ( UVs.m_mins.x + UVs.m_maxs.x );
	float vCenter = 0.5f * ( UVs.m_mins.y + UVs.m_maxs.y );
	float uHalf = 0.5f * ( UVs.m_maxs.x - UVs.m_mins.x );
	float vHalf = 0.5f * ( UVs.m_maxs.y - UVs.m_mins.y );

	for ( int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex )
	{
		float tA = static_cast< float >( sliceIndex ) / static_cast< float >( numSlices );
		float tB = static_cast< float >( sliceIndex + 1 ) / static_cast< float >( numSlices );
		float degreesA = 360.f * tA;
		float degreesB = 360.f * tB;

		Vec3 left = radius * CosDegrees( degreesA ) * lookAtLeftJ + radius * SinDegrees( degreesA ) * lookAtUpK;
		Vec3 right = radius * CosDegrees( degreesB ) * lookAtLeftJ + radius * SinDegrees( degreesB ) * lookAtUpK;

		Vec3 bottomLeft = start + left;
		Vec3 bottomRight = start + right;
		Vec3 topLeft = end + left;
		Vec3 topRight = end + right;

		float uMin = UVs.m_mins.x + ( UVs.m_maxs.x - UVs.m_mins.x ) * tA;
		float uMax = UVs.m_mins.x + ( UVs.m_maxs.x - UVs.m_mins.x ) * tB;
		float vMin = UVs.m_mins.y;
		float vMax = UVs.m_maxs.y;

		AddVertsForQuad3D( verts, bottomLeft, bottomRight, topRight, topLeft, color, AABB2( Vec2( uMin, vMin ), Vec2( uMax, vMax ) ) );

		Vec2 uvLeft( uCenter + uHalf * CosDegrees( degreesA ), vCenter + vHalf * SinDegrees( degreesA ) );
		Vec2 uvRight( uCenter + uHalf * CosDegrees( degreesB ), vCenter + vHalf * SinDegrees( degreesB ) );
		Vec2 uvCenter( uCenter, vCenter );

		AddVertsForTriangle3D( verts, start, bottomRight, bottomLeft, color, uvCenter, uvRight, uvLeft );
		AddVertsForTriangle3D( verts, end, topLeft, topRight, color, uvCenter, uvLeft, uvRight );
	}
}


//-----------------------------------------------------------------------------------------------
void AddVertsForCone3D( std::vector<Vertex>& verts, Vec3 const& start, Vec3 const& end, float baseRadius, Rgba8 const& color,
						AABB2 const& UVs, int numSlices )
{
	if ( numSlices <= 0 || baseRadius <= 0.f || start == end )
	{
		return;
	}

	Vec3 lookAtForwardI = ( end - start ).GetNormalized();
	Vec3 lookAtLeftJ;
	Vec3 lookAtUpK;

	Vec3 worldZAxis( 0.f, 0.f, 1.f );
	Vec3 worldYAxis( 0.f, 1.f, 0.f );

	if ( abs( DotProduct3D( lookAtForwardI, worldZAxis ) ) < 0.99999f )
	{
		lookAtLeftJ = CrossProduct3D( worldZAxis, lookAtForwardI ).GetNormalized();
		lookAtUpK = CrossProduct3D( lookAtForwardI, lookAtLeftJ ).GetNormalized();
	}
	else
	{
		lookAtUpK = CrossProduct3D( lookAtForwardI, worldYAxis ).GetNormalized();
		lookAtLeftJ = CrossProduct3D( lookAtUpK, lookAtForwardI ).GetNormalized();
	}

	float uCenter = 0.5f * ( UVs.m_mins.x + UVs.m_maxs.x );
	float vCenter = 0.5f * ( UVs.m_mins.y + UVs.m_maxs.y );
	float uHalf = 0.5f * ( UVs.m_maxs.x - UVs.m_mins.x );
	float vHalf = 0.5f * ( UVs.m_maxs.y - UVs.m_mins.y );

	for ( int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex )
	{
		float tA = static_cast< float >( sliceIndex ) / static_cast< float >( numSlices );
		float tB = static_cast< float >( sliceIndex + 1 ) / static_cast< float >( numSlices );
		float degreesA = 360.f * tA;
		float degreesB = 360.f * tB;

		Vec3 left = baseRadius * CosDegrees( degreesA ) * lookAtLeftJ + baseRadius * SinDegrees( degreesA ) * lookAtUpK;
		Vec3 right = baseRadius * CosDegrees( degreesB ) * lookAtLeftJ + baseRadius * SinDegrees( degreesB ) * lookAtUpK;

		Vec3 baseLeft = start + left;
		Vec3 baseRight = start + right;

		float uMin = UVs.m_mins.x + ( UVs.m_maxs.x - UVs.m_mins.x ) * tA;
		float uMax = UVs.m_mins.x + ( UVs.m_maxs.x - UVs.m_mins.x ) * tB;
		float uTip = 0.5f * ( uMin + uMax );

		AddVertsForTriangle3D( verts, end, baseLeft, baseRight, color, Vec2( uTip, UVs.m_maxs.y ), Vec2( uMin, UVs.m_mins.y ), Vec2( uMax, UVs.m_mins.y ) );

		Vec2 uvLeft( uCenter + uHalf * CosDegrees( degreesA ), vCenter + vHalf * SinDegrees( degreesA ) );
		Vec2 uvRight( uCenter + uHalf * CosDegrees( degreesB ), vCenter + vHalf * SinDegrees( degreesB ) );
		Vec2 uvCenter( uCenter, vCenter );

		AddVertsForTriangle3D( verts, start, baseRight, baseLeft, color, uvCenter, uvRight, uvLeft );
	}
}


//-----------------------------------------------------------------------------------------------
void AddVertsForArrow3D( std::vector<Vertex>& verts, Vec3 const& start, Vec3 const& end, float radius, 
						 Rgba8 const& color, int numSlices )
{
	if ( numSlices <= 0 || radius <= 0.f || start == end )
	{
		return;
	}

	Vec3 direction = end - start;
	float totalLength = direction.GetLength();
	Vec3 forward = direction / totalLength;

	float headLength = 0.25f * totalLength;
	float headRadius = 1.75f * radius;

	Vec3 shaftEnd = end - ( forward * headLength );

	AddVertsForCylinder3D( verts, start, shaftEnd, radius, color, AABB2::ZERO_TO_ONE, numSlices );
	AddVertsForCone3D( verts, shaftEnd, end, headRadius, color, AABB2::ZERO_TO_ONE, numSlices );
}