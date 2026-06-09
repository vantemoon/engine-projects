#include "Game/Board.hpp"
#include "Game/Card.hpp"
#include "Game/CardDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Board::Board()
{
	m_bottomLeft = Vec2( WORLD_SIZE_X * 0.10f, WORLD_SIZE_Y * 0.10f );
	m_bottomRight = Vec2( WORLD_SIZE_X * 0.90f, WORLD_SIZE_Y * 0.10f );
	m_topRight = Vec2( WORLD_SIZE_X * 0.825f, WORLD_SIZE_Y * 0.80f );
	m_topLeft = Vec2( WORLD_SIZE_X * 0.175f, WORLD_SIZE_Y * 0.80f );

	CardDefinition::InitializeDefinitions();

	CreateTestCards();
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
void Board::Update( float deltaSeconds, Vec2 const& mouseWorldPosition )
{
	UNUSED( deltaSeconds );

	bool wasLeftMouseJustPressed = g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_LBUTTON );
	bool isLeftMouseDown = g_engine->m_inputSystem->IsKeyDown( KEYCODE_LBUTTON );
	bool wasLeftMouseJustReleased = g_engine->m_inputSystem->WasKeyJustReleased( KEYCODE_LBUTTON );

	if ( wasLeftMouseJustPressed )
	{
		int clickedCardIndex = GetTopCardIndexAtPosition( mouseWorldPosition );

		if ( clickedCardIndex >= 0 )
		{
			BeginDraggingCardStack( clickedCardIndex, mouseWorldPosition );
		}
		else
		{
			ClearCardSelection();
			EndDraggingCardStack();
		}
	}

	if ( isLeftMouseDown && m_isDraggingCard )
	{
		UpdateDraggingCardStack( mouseWorldPosition );
	}

	if ( wasLeftMouseJustReleased )
	{
		EndDraggingCardStack();
	}

	for ( Card* card : m_cards )
	{
		card->Update();
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


//-----------------------------------------------------------------------------------------------
void Board::CreateTestCards()
{
	CardDefinition const* villagerDef = CardDefinition::GetCardDefinitionByName( "Villager" );
	GUARANTEE_OR_DIE( villagerDef != nullptr, "Could not find CardDefinition: Villager" );

	Vec2 firstCardPosition = Vec2( WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f );
	Card* firstCard = new Card( villagerDef, firstCardPosition );
	Card* secondCard = new Card( villagerDef, firstCard->GetNextPosition() );
	Card* thirdCard = new Card( villagerDef, secondCard->GetNextPosition() );
	m_cards.push_back( firstCard );
	m_cards.push_back( secondCard );
	m_cards.push_back( thirdCard );
}


//-----------------------------------------------------------------------------------------------
int Board::GetTopCardIndexAtPosition( Vec2 const& worldPosition ) const
{
	for ( int cardIndex = ( int ) m_cards.size() - 1; cardIndex >= 0; --cardIndex )
	{
		Card* card = m_cards[cardIndex];

		if ( card != nullptr && card->IsPointInside( worldPosition ) )
		{
			return cardIndex;
		}
	}

	return -1;
}


//-----------------------------------------------------------------------------------------------
void Board::BeginDraggingCardStack( int cardIndex, Vec2 const& mouseWorldPosition )
{
	m_draggedCards.clear();
	m_dragOffsets.clear();

	if ( cardIndex < 0 || cardIndex >= ( int ) m_cards.size() )
	{
		return;
	}

	ClearCardSelection();

	for ( int index = cardIndex; index < ( int ) m_cards.size(); ++index )
	{
		Card* card = m_cards[index];

		if ( card == nullptr )
		{
			continue;
		}

		card->SetIsSelected( true );

		m_draggedCards.push_back( card );
		m_dragOffsets.push_back( card->GetPosition() - mouseWorldPosition );
	}

	m_isDraggingCard = !m_draggedCards.empty();
}


//-----------------------------------------------------------------------------------------------
void Board::UpdateDraggingCardStack( Vec2 const& mouseWorldPosition )
{
	for ( int index = 0; index < ( int ) m_draggedCards.size(); ++index )
	{
		Card* card = m_draggedCards[index];

		if ( card == nullptr )
		{
			continue;
		}

		card->SetPosition( mouseWorldPosition + m_dragOffsets[index] );
	}
}


//-----------------------------------------------------------------------------------------------
void Board::EndDraggingCardStack()
{
	m_isDraggingCard = false;
	for ( Card* card : m_draggedCards )
	{
		if ( card != nullptr )
		{
			card->SetIsSelected( false );
		}
	}
	m_draggedCards.clear();
	m_dragOffsets.clear();
}


//-----------------------------------------------------------------------------------------------
void Board::ClearCardSelection()
{
	for ( Card* card : m_cards )
	{
		if ( card != nullptr )
		{
			card->SetIsSelected( false );
		}
	}
}