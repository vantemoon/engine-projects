#pragma once


//-----------------------------------------------------------------------------------------------
struct Vec2;   // Forward declaration
struct Vertex; // Forward declaration


//-----------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D( int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY );
