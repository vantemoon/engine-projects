#include "Game/Board.hpp"
#include "Game/Card.hpp"
#include "Game/CardDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Board::Board()
{
	m_bottomLeft = Vec2( WORLD_SIZE_X * 0.10f, WORLD_SIZE_Y * 0.10f );
	m_bottomRight = Vec2( WORLD_SIZE_X * 0.90f, WORLD_SIZE_Y * 0.10f );
	m_topRight = Vec2( WORLD_SIZE_X * 0.825f, WORLD_SIZE_Y * 0.80f );
	m_topLeft = Vec2( WORLD_SIZE_X * 0.175f, WORLD_SIZE_Y * 0.80f );

	CardDefinition::InitializeDefinitions();

	CardDefinition const* villagerDef = CardDefinition::GetCardDefinitionByName( "Villager" );
	Card* card = new Card( Vec2( WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f ), villagerDef );
	m_cards.push_back( card );
}


//-----------------------------------------------------------------------------------------------
Board::~Board()
{
	for ( Card* card : m_cards )
	{
		delete card;
	}
	m_cards.clear();

	CardDefinition::ClearDefinitions();
}


//-----------------------------------------------------------------------------------------------
void Board::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	for ( Card* card : m_cards )
	{
		card->Update( deltaSeconds );
	}
}


//-----------------------------------------------------------------------------------------------
void Board::Render() const
{
	std::vector<Vertex> verts;

	Rgba8 boardTint( 230, 240, 215, 180 );
	Rgba8 borderTint( 0, 0, 0, 255 );

	AddVertsForQuad2D(
		verts,
		m_bottomLeft,
		m_bottomRight,
		m_topRight,
		m_topLeft,
		boardTint
	);

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	DebugDrawLine( m_bottomLeft, m_bottomRight, 0.4f, borderTint, borderTint );
	DebugDrawLine( m_bottomRight, m_topRight, 0.4f, borderTint, borderTint );
	DebugDrawLine( m_topRight, m_topLeft, 0.4f, borderTint, borderTint );
	DebugDrawLine( m_topLeft, m_bottomLeft, 0.4f, borderTint, borderTint );

	for ( Card* card : m_cards )
	{
		card->Render();
	}
}