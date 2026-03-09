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


//-----------------------------------------------------------------------------------------------
void AddVertsForCylinder3D( std::vector<Vertex>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color,
							AABB2 const& UVs, int numSlices )
{
	if ( numSlices <= 0 )
	{
		return;
	}
	
	float degreesPerSlice = 360.f / static_cast<float>( numSlices );
	for ( int i = 0; i < numSlices; ++i )
	{
		float degreesA = degreesPerSlice * static_cast<float>( i );
		float degreesB = degreesPerSlice * static_cast<float>( i + 1 );

		Vec3 offsetA = Vec3::MakeFromPolarDegrees( 0.f, degreesA, radius );
		Vec3 offsetB = Vec3::MakeFromPolarDegrees( 0.f, degreesB, radius );

		Vec3 bottomLeft = start + offsetA;
		Vec3 bottomRight = start + offsetB;

		float const uMin = UVs.m_mins.x;
		float const uMax = UVs.m_maxs.x;
		float const uRange = uMax - uMin;
		float const invNumSlices = 1.f / static_cast<float>( numSlices );

		float const uLeft = static_cast<float>( i ) * invNumSlices;
		float const uRight = static_cast<float>( i + 1 ) * invNumSlices;
		float const uCenter = ( static_cast<float>( i ) + 0.5f ) * invNumSlices;

		Vec2 uvBottomLeft = Vec2( uMin + uRange * uLeft, UVs.m_mins.y );
		Vec2 uvBottomRight = Vec2( uMin + uRange * uRight, UVs.m_mins.y );
		Vec2 uvTop = Vec2( uMin + uRange * uCenter, UVs.m_maxs.y );
		
		AddVertsForTriangle3D( verts, end, bottomLeft, bottomRight, color, uvTop, uvBottomLeft, uvBottomRight );
	}
}