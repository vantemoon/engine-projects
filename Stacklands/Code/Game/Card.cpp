#include "Game/Card.hpp"
#include "Game/CardDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"


//-----------------------------------------------------------------------------------------------
Card::Card( CardDefinition const* definition, Vec2 const& position )
	: m_definition( definition )
	, m_position( position )
{
	m_nextPosition = m_position - Vec2( 0.f, m_size.y * 0.19f );
}


//-----------------------------------------------------------------------------------------------
Card::~Card()
{
}


//-----------------------------------------------------------------------------------------------
void Card::Update()
{
	m_nextPosition = m_position - Vec2( 0.f, m_size.y * 0.19f );
}


//-----------------------------------------------------------------------------------------------
void Card::Render() const
{
	if ( m_definition == nullptr )
	{
		return;
	}

	AABB2 bounds = GetBounds();

	Rgba8 tint = m_tint;
	if ( m_isSelected ) tint = m_highlightTint;

	std::vector<Vertex> verts;
	AddVertsForAABB2D(verts, bounds, tint );

	g_engine->m_renderer->BindTexture( m_definition->m_texture );
	g_engine->m_renderer->DrawVertexArray( verts );
	g_engine->m_renderer->BindTexture( nullptr );
}


//-----------------------------------------------------------------------------------------------
Vec2 Card::GetPosition() const
{
	return m_position;
}


//-----------------------------------------------------------------------------------------------
Vec2 Card::GetNextPosition() const
{
	return m_nextPosition;
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


//-----------------------------------------------------------------------------------------------
bool Card::IsPointInside( Vec2 const& point ) const
{
	return GetBounds().IsPointInside( point );
}


//-----------------------------------------------------------------------------------------------
void Card::SetIsSelected( bool isSelected )
{
	m_isSelected = isSelected;
}


//-----------------------------------------------------------------------------------------------
bool Card::IsSelected() const
{
	return m_isSelected;
}


//-----------------------------------------------------------------------------------------------
CardDefinition const* Card::GetDefinition() const
{
	return m_definition;
}