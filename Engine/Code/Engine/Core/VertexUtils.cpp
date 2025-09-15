#include <cmath>
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D( int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY )
{
	// Translate local space verts to world space
	float rotationRadians = ConvertDegreesToRadians( rotationDegreesAboutZ );
	float cosTheta = cosf( rotationRadians );
	float sinTheta = sinf( rotationRadians );

	for ( int vertIndex = 0; vertIndex < numVerts; ++ vertIndex )
	{
		verts[vertIndex].m_position.x *= uniformScaleXY;
		verts[vertIndex].m_position.y *= uniformScaleXY;

		float originalX = verts[vertIndex].m_position.x;
		float originalY = verts[vertIndex].m_position.y;
		verts[vertIndex].m_position.x = (originalX * cosTheta) - (originalY * sinTheta);
		verts[vertIndex].m_position.y = (originalX * sinTheta) + (originalY * cosTheta);

		verts[vertIndex].m_position.x += translationXY.x;
		verts[vertIndex].m_position.y += translationXY.y;
	}
}