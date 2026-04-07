#include "Engine/Core/Vertex.hpp"


//-----------------------------------------------------------------------------------------------
Vertex::Vertex()
	: m_position( 0.f, 0.f, 0.f )
	, m_color( 255, 255, 255, 255 )
	, m_uvTexCoords( 0.f, 0.f )
	, m_tangent( 0.f, 0.f, 0.f )
	, m_bitangent( 0.f, 0.f, 0.f )
	, m_normal( 0.f, 0.f, 0.f )
{
}


//-----------------------------------------------------------------------------------------------
Vertex::Vertex( Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords )
	: m_position( position )
	, m_color( color )
	, m_uvTexCoords( uvTexCoords )
	, m_tangent( 0.f, 0.f, 0.f )
	, m_bitangent( 0.f, 0.f, 0.f )
	, m_normal( 0.f, 0.f, 0.f )
{
}