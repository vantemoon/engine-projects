#pragma once
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Plane3.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
struct OBB2;
struct OBB3;


//-----------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D( int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY );
void TransformVertexArray3D( std::vector<Vertex>& verts, Mat44 const& transform );


//-----------------------------------------------------------------------------------------------
AABB2 GetVertexBounds2D( std::vector<Vertex> const& verts );


//-----------------------------------------------------------------------------------------------
void AddVertsForDisc2D( std::vector<Vertex>& verts, Vec2 const& discCenter, float discRadius, Rgba8 const& color, int numSides );
void AddVertsForRing2D( std::vector<Vertex>& verts, Vec2 const& ringCenter, float ringRadius, float thickness, Rgba8 const& color, int numSides );
void AddVertsForAABB2D( std::vector<Vertex>& verts, AABB2 const& alignedBox, Rgba8 const& color );
void AddVertsForAABB2D( std::vector<Vertex>& verts, AABB2 const& alignedBox, Rgba8 const& color, Vec2 uvAtMins, Vec2 uvAtMaxs );
void AddVertsForQuad2D( std::vector<Vertex>& verts, Vec2 const& bottomLeft, Vec2 const& bottomRight, Vec2 const& topRight, Vec2 const& topLeft, Rgba8 const& color );
void AddVertsForOBB2D( std::vector<Vertex>& verts, OBB2 const& orientedBox, Rgba8 const& color );
void AddVertsForCapsule2D( std::vector<Vertex>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color, int numSides );
void AddVertsForTriangle2D( std::vector<Vertex>& verts, Vec2 const& ccw0, Vec2 const& ccw1, Vec2 const& ccw2, Rgba8 const& color,
	Vec2 const& uv0 = Vec2( 0.f, 0.f ), Vec2 const& uv1 = Vec2( 1.f, 0.f ), Vec2 const& uv2 = Vec2( 0.5f, 1.f ) );
void AddVertsForLineSegment2D( std::vector<Vertex>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color );
void AddVertsForArrow2D( std::vector<Vertex>& verts, Vec2 const& startTail, Vec2 const& endTip, float thickness, float arrowRadius, Rgba8 const& color );


//-----------------------------------------------------------------------------------------------
void AddVertsForTriangle3D( std::vector<Vertex>& verts, 
	Vec3 const& ccw0, Vec3 const& ccw1, Vec3 const& ccw2, Rgba8 const& color = Rgba8::WHITE, 
	Vec2 const& uv0 = Vec2( 0.f, 0.f ), Vec2 const& uv1 = Vec2( 1.f, 0.f ), Vec2 const& uv2 = Vec2( 0.5f, 1.f ) );
void AddVertsForTriangle3DExplicit( std::vector<Vertex>& verts,
	Vec3 const& ccw0, Vec3 const& ccw1, Vec3 const& ccw2, Rgba8 const& color = Rgba8::WHITE,
	Vec2 const& uv0 = Vec2( 0.f, 0.f ), Vec2 const& uv1 = Vec2( 1.f, 0.f ), Vec2 const& uv2 = Vec2( 0.5f, 1.f ),
	Vec3 const& tangent = Vec3::ZERO, Vec3 const& bitangent = Vec3::ZERO, Vec3 const& normal = Vec3::ZERO );
void AddVertsForIndexedTriangle3D( std::vector<Vertex>& verts, std::vector<unsigned int>& indices, 
	Vec3 const& ccw0, Vec3 const& ccw1, Vec3 const& ccw2, Rgba8 const& color = Rgba8::WHITE,
	Vec2 const& uv0 = Vec2( 0.f, 0.f ), Vec2 const& uv1 = Vec2( 1.f, 0.f ), Vec2 const& uv2 = Vec2( 0.5f, 1.f ) );
void AddVertsForIndexedTriangle3DExplicit( std::vector<Vertex>& verts, std::vector<unsigned int>& indices,
	Vec3 const& ccw0, Vec3 const& ccw1, Vec3 const& ccw2, Rgba8 const& color = Rgba8::WHITE,
	Vec2 const& uv0 = Vec2( 0.f, 0.f ), Vec2 const& uv1 = Vec2( 1.f, 0.f ), Vec2 const& uv2 = Vec2( 0.5f, 1.f ),
	Vec3 const& tangent = Vec3::ZERO, Vec3 const& bitangent = Vec3::ZERO, Vec3 const& normal = Vec3::ZERO );
void AddVertsForQuad3D( std::vector<Vertex>& verts, 
	Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, 
	Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForIndexedQuad3D( std::vector<Vertex>& verts, std::vector<unsigned int>& indices,
	Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft,
	Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForAABB3D( std::vector<Vertex>& verts, 
	AABB3 const& bounds, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForIndexedAABB3D( std::vector<Vertex>& verts, std::vector<unsigned int>& indices,
	AABB3 const& bounds, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForAABBWireframe3D( std::vector<Vertex>& verts, 
	AABB3 const& bounds, Rgba8 const& color = Rgba8::WHITE );
void AddVertsForSphere3D( std::vector<Vertex>& verts, 
	Vec3 const& center, float radius, Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 32, int numStacks = 16 );
void AddVertsForIndexedSphere3D( std::vector<Vertex>& verts, std::vector<unsigned int>& indices,
	Vec3 const& center, float radius, Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 32, int numStacks = 16 );
void AddVertsForSphereWireframe3D( std::vector<Vertex>& verts, 
	Vec3 const& center, float radius, Rgba8 const& color = Rgba8::WHITE, int numSlices = 32, int numStacks = 16 );
void AddvertsForXYGrid3D( std::vector<Vertex>& verts, 
	Vec3 const& center, float cellSize, int numCellsX, int numCellsY );
void AddVertsForCylinderZ3D( std::vector<Vertex>& verts, 
	Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE, 
	AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSides = 32 );
void AddVertsForIndexedCylinderZ3D( std::vector<Vertex>& verts, std::vector<unsigned int>& indices,
	Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSides = 32 );
void AddVertsForCylinderZWireframe3D( std::vector<Vertex>& verts, Vec3 const& start, Vec3 const& end, float radius,
	Rgba8 const& color = Rgba8::WHITE, int numSides = 32 );
void AddVertsForCone3D( std::vector<Vertex>& verts, 
	Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE, 
	AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSides = 32 );
void AddVertsForIndexedCone3D( std::vector<Vertex>& verts, std::vector<unsigned int>& indices,
	Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSides = 32 );
void AddVertsForConeWireframe3D( std::vector<Vertex>& verts, Vec3 const& start, Vec3 const& end, float radius,
	Rgba8 const& color = Rgba8::WHITE, int numSides = 32 );
void AddVertsForArrow3D( std::vector<Vertex>& verts, 
	Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE, int numSides = 32 );
void AddVertsForIndexedArrow3D( std::vector<Vertex>& verts, std::vector<unsigned int>& indices,
	Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE, int numSides = 32 );
void AddVertsForLineSegment3D( std::vector<Vertex>& verts, 
	Vec3 const& start, Vec3 const& end, float thickness, Rgba8 const& color = Rgba8::WHITE );
void AddVertsForIndexedLineSegment3D( std::vector<Vertex>& verts, std::vector<unsigned int>& indices, 
	Vec3 const& start, Vec3 const& end, float thickness, Rgba8 const& color = Rgba8::WHITE );
void AddVertsForOBB3D( std::vector<Vertex>& verts, 
	OBB3 const& orientedBox, Rgba8 const& color = Rgba8::WHITE );
void AddVertsForIndexedOBB3D( std::vector<Vertex>& verts, std::vector<unsigned int>& indices, 
	OBB3 const& orientedBox, Rgba8 const& color = Rgba8::WHITE );
void AddVertsForOBBWireframe3D( std::vector<Vertex>& verts, 
	OBB3 const& orientedBox, Rgba8 const& color = Rgba8::WHITE );
void AddVertsForPlane3D( std::vector<Vertex>& verts, Plane3 const& plane, float planeSize, Rgba8 const& color = Rgba8::WHITE );