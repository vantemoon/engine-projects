#pragma once
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
struct OBB2;


//-----------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D( int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY );


//-----------------------------------------------------------------------------------------------
void AddVertsForDisc2D( std::vector<Vertex>& verts, Vec2 const& discCenter, float discRadius, Rgba8 const& color, int numSides );
void AddVertsForRing2D( std::vector<Vertex>& verts, Vec2 const& ringCenter, float ringRadius, float thickness, Rgba8 const& color, int numSides );
void AddVertsForAABB2D( std::vector<Vertex>& verts, AABB2 const& alignedBox, Rgba8 const& color );
void AddVertsForAABB2D( std::vector<Vertex>& verts, AABB2 const& alignedBox, Rgba8 const& color, Vec2 uvAtMins, Vec2 uvAtMaxs );
void AddVertsForOBB2D( std::vector<Vertex>& verts, OBB2 const& orientedBox, Rgba8 const& color );
void AddVertsForCapsule2D( std::vector<Vertex>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color, int numSides );
void AddVertsForTriangle2D( std::vector<Vertex>& verts, Vec2 const& ccw0, Vec2 const& ccw1, Vec2 const& ccw2, Rgba8 const& color );
void AddVertsForLineSegment2D( std::vector<Vertex>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color );
void AddVertsForArrow2D( std::vector<Vertex>& verts, Vec2 const& startTail, Vec2 const& endTip, float thickness, float arrowRadius, Rgba8 const& color );


//-----------------------------------------------------------------------------------------------
void AddVertsForQuad3D( std::vector<Vertex>& verts, 
						Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, 
	                    Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );