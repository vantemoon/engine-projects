#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
void DebugDrawRing( Vec2 center, float radius, float thickness, const Rgba8& color )
{
	constexpr int NUM_SEGMENTS = 32;
	constexpr int NUM_TRIANGLES = NUM_SEGMENTS * 2;
	constexpr int NUM_VERTS = NUM_TRIANGLES * 3;
	constexpr float DEGREES_PER_SEGMENT = 360.f / static_cast<float> ( NUM_SEGMENTS );

	float halfThickness = thickness / 2.f;
	float innerRadius = radius - halfThickness;
	float outerRadius = radius + halfThickness;

	Vertex verts[NUM_VERTS];

	for ( int segmentIndex = 0; segmentIndex < NUM_SEGMENTS; ++ segmentIndex )
	{
		float startDegrees = DEGREES_PER_SEGMENT * static_cast<float>( segmentIndex );
		float endDegrees = DEGREES_PER_SEGMENT * static_cast<float>( segmentIndex + 1 );

		float cosStart = CosDegrees( startDegrees );
		float sinStart = SinDegrees( startDegrees );
		float cosEnd = CosDegrees( endDegrees );
		float sinEnd = SinDegrees( endDegrees );

		Vec3 innerStart = Vec3( center.x + innerRadius * cosStart, center.y + innerRadius * sinStart, 0.f );
		Vec3 outerStart = Vec3( center.x + outerRadius * cosStart, center.y + outerRadius * sinStart, 0.f );
		Vec3 outerEnd = Vec3( center.x + outerRadius * cosEnd, center.y + outerRadius * sinEnd, 0.f );
		Vec3 innerEnd = Vec3( center.x + innerRadius * cosEnd, center.y + innerRadius * sinEnd, 0.f );
		
		int vertIndexA = segmentIndex * 6;     // Inner end
		int vertIndexB = segmentIndex * 6 + 1; // Inner start
		int vertIndexC = segmentIndex * 6 + 2; // Outer start
		int vertIndexD = segmentIndex * 6 + 3; // Inner end
		int vertIndexE = segmentIndex * 6 + 4; // Outer start
		int vertIndexF = segmentIndex * 6 + 5; // Outer end
		// Inner triangle
		verts[vertIndexA].m_position = innerEnd;
		verts[vertIndexB].m_position = innerStart;
		verts[vertIndexC].m_position = outerStart;
		verts[vertIndexA].m_color = color;
		verts[vertIndexB].m_color = color;
		verts[vertIndexC].m_color = color;

		// Outer triangle
		verts[vertIndexD].m_position = innerEnd;
		verts[vertIndexE].m_position = outerStart;
		verts[vertIndexF].m_position = outerEnd;
		verts[vertIndexD].m_color = color;
		verts[vertIndexE].m_color = color;
		verts[vertIndexF].m_color = color;

		g_engine->m_renderer->DrawVertexArray( NUM_VERTS, verts );
	}
}


//-----------------------------------------------------------------------------------------------
void DebugDrawLine( Vec2 start, Vec2 end, float thickness, const Rgba8& color )
{
	constexpr int NUM_TRIANGLES = 2;
	constexpr int NUM_VERTS = NUM_TRIANGLES * 3;

	Vertex verts[NUM_VERTS];

	Vec2 direction = end - start;
	direction.Normalize();
	Vec2 normal = Vec2( -direction.y, direction.x );
	normal *= (thickness / 2.f);

	Vec3 innerStart = Vec3( start.x - normal.x, start.y - normal.y, 0.f );
	Vec3 innerEnd = Vec3( end.x - normal.x , end.y - normal.y, 0.f );
	Vec3 outerStart = Vec3( start.x + normal.x, start.y + normal.y, 0.f );
	Vec3 outerEnd = Vec3( end.x + normal.x, end.y + normal.y, 0.f );

	// Inner triangle
	verts[0].m_position = innerStart;
	verts[1].m_position = outerStart;
	verts[2].m_position = outerEnd;
	verts[0].m_color = color;
	verts[1].m_color = color;
	verts[2].m_color = color;

	// Outer triangle
	verts[3].m_position = innerStart;
	verts[4].m_position = outerEnd;
	verts[5].m_position = innerEnd;
	verts[3].m_color = color;
	verts[4].m_color = color;
	verts[5].m_color = color;

	g_engine->m_renderer->DrawVertexArray( NUM_VERTS, verts );
}