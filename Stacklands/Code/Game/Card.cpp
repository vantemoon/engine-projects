#include "Game/Card.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"


//-----------------------------------------------------------------------------------------------
Card::Card( Vec2 const& position, std::string const& name )
	: m_position( position )
	, m_name( name )
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

	AddVertsForAABB2D( verts, bounds, m_bodyTint );

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	DebugDrawLine( Vec2( bounds.m_maxs.x, bounds.m_mins.y ), bounds.m_maxs, 0.4f, m_borderTint, m_borderTint );
	DebugDrawLine( bounds.m_maxs, Vec2( bounds.m_mins.x, bounds.m_maxs.y ), 0.4f, m_borderTint, m_borderTint );
	DebugDrawLine( Vec2( bounds.m_mins.x, bounds.m_maxs.y ), bounds.m_mins, 0.4f, m_borderTint, m_borderTint );
	DebugDrawLine( bounds.m_mins, Vec2( bounds.m_maxs.x, bounds.m_mins.y ), 0.4f, m_borderTint, m_borderTint );
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