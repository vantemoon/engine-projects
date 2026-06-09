#include "Game/Card.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"


//-----------------------------------------------------------------------------------------------
Card::Card( Vec2 const& position, CardDefinition const* definition )
	: m_position( position )
	, m_definition( definition )
{
}


//-----------------------------------------------------------------------------------------------
void Card::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Card::Render() const
{
	AABB2 bounds = GetBounds();

	std::vector<Vertex> verts;

	AddVertsForAABB2D( verts, bounds, Rgba8::WHITE );

	g_engine->m_renderer->BindTexture( m_definition->m_texture );
	g_engine->m_renderer->DrawVertexArray( verts );
}


//-----------------------------------------------------------------------------------------------
Vec2 Card::GetPosition() const
{
	return m_position;
}


//-----------------------------------------------------------------------------------------------
void Card::SetPosition( Vec2 const& position )
{
	m_position = position;
}


//-----------------------------------------------------------------------------------------------
AABB2 Card::GetBounds() const
{
	Vec2 halfSize = m_size * 0.5f;
	return AABB2( m_position - halfSize, m_position + halfSize );
}