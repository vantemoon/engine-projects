#include "Game/Board.hpp"
#include "Game/Card.hpp"
#include "Game/CardDefinition.hpp"
#include "Game/CardStack.hpp"
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
	for ( CardStack* stack : m_cardStacks )
	{
		delete stack;
	}

	m_cardStacks.clear();
	m_draggedStack = nullptr;

	CardDefinition::ClearDefinitions();
}


//-----------------------------------------------------------------------------------------------
void Board::CreateTestCards()
{
	CardDefinition const* villagerDef = CardDefinition::GetCardDefinitionByName( "Villager" );
	GUARANTEE_OR_DIE( villagerDef != nullptr, "Could not find CardDefinition: Villager" );

	CardStack* testStack = new CardStack();

	Vec2 firstCardPos = Vec2( WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f );
	Card* firstCard = new Card( villagerDef, firstCardPos );
	Card* secondCard = new Card( villagerDef, firstCard->GetNextPosition() );
	Card* thirdCard = new Card( villagerDef, secondCard->GetNextPosition() );

	testStack->AddCard( firstCard );
	testStack->AddCard( secondCard );
	testStack->AddCard( thirdCard );

	m_cardStacks.push_back( testStack );
}


//-----------------------------------------------------------------------------------------------
void Board::Update( Vec2 const& mouseWorldPosition )
{
	for ( CardStack* stack : m_cardStacks )
	{
		if ( stack != nullptr )
		{
			stack->Update();
		}
	}

	bool wasLeftMouseJustPressed = g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_LBUTTON );
	bool isLeftMouseDown = g_engine->m_inputSystem->IsKeyDown( KEYCODE_LBUTTON );
	bool wasLeftMouseJustReleased = g_engine->m_inputSystem->WasKeyJustReleased( KEYCODE_LBUTTON );

	if ( wasLeftMouseJustPressed )
	{
		CardHitResult hitResult = GetTopCardHitAtPosition( mouseWorldPosition );

		if ( hitResult.IsValid() )
		{
			BeginDraggingStackFromHit( hitResult );
			m_lastMouseWorldPosition = mouseWorldPosition;
		}
		else
		{
			ClearCardSelection();
			EndDraggingStack();
		}
	}

	if ( isLeftMouseDown && m_isDraggingCardStack && m_draggedStack != nullptr )
	{
		UpdateDraggedStack( mouseWorldPosition );
	}

	if ( wasLeftMouseJustReleased )
	{
		EndDraggingStack();
	}

	RemoveEmptyStacks();
}


//-----------------------------------------------------------------------------------------------
void Board::Render() const
{
	std::vector<Vertex> boardVerts;

	Rgba8 boardTint( 230, 240, 215, 180 );
	Rgba8 borderTint( 0, 0, 0, 255 );

	AddVertsForQuad2D(
		boardVerts,
		m_bottomLeft,
		m_bottomRight,
		m_topRight,
		m_topLeft,
		boardTint
	);

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( boardVerts );

	DebugDrawLine( m_bottomLeft, m_bottomRight, 0.4f, borderTint, borderTint );
	DebugDrawLine( m_bottomRight, m_topRight, 0.4f, borderTint, borderTint );
	DebugDrawLine( m_topRight, m_topLeft, 0.4f, borderTint, borderTint );
	DebugDrawLine( m_topLeft, m_bottomLeft, 0.4f, borderTint, borderTint );

	for ( CardStack* stack : m_cardStacks )
	{
		if ( stack != nullptr )
		{
			stack->Render();
		}
	}
}


//-----------------------------------------------------------------------------------------------
CardHitResult Board::GetTopCardHitAtPosition( Vec2 const& worldPosition ) const
{
	for ( int stackIndex = ( int ) m_cardStacks.size() - 1; stackIndex >= 0; --stackIndex )
	{
		CardStack* stack = m_cardStacks[stackIndex];

		if ( stack == nullptr )
		{
			continue;
		}

		int cardIndex = stack->GetTopCardIndexAtPosition( worldPosition );

		if ( cardIndex >= 0 )
		{
			CardHitResult result;
			result.m_stackIndex = stackIndex;
			result.m_cardIndex = cardIndex;
			return result;
		}
	}

	return CardHitResult();
}


//-----------------------------------------------------------------------------------------------
void Board::BeginDraggingStackFromHit( CardHitResult const& hitResult )
{
	if ( !hitResult.IsValid() )
	{
		return;
	}

	CardStack* sourceStack = m_cardStacks[hitResult.m_stackIndex];
	if ( sourceStack == nullptr )
	{
		return;
	}

	ClearCardSelection();

	std::vector<Card*> draggedCards = sourceStack->RemoveCardsFromIndex( hitResult.m_cardIndex );

	CardStack* newDraggedStack = new CardStack();
	newDraggedStack->AddCards( draggedCards );
	newDraggedStack->SetCardsSelected( true );

	m_cardStacks.push_back( newDraggedStack );

	m_draggedStack = newDraggedStack;
	m_isDraggingCardStack = true;
}


//-----------------------------------------------------------------------------------------------
void Board::UpdateDraggedStack( Vec2 const& mouseWorldPosition )
{
	Vec2 mouseDelta = mouseWorldPosition - m_lastMouseWorldPosition;

	if ( m_draggedStack != nullptr )
	{
		m_draggedStack->MoveByOffset( mouseDelta );
	}

	m_lastMouseWorldPosition = mouseWorldPosition;
}


//-----------------------------------------------------------------------------------------------
void Board::EndDraggingStack()
{
	m_isDraggingCardStack = false;
	m_draggedStack = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Board::ClearCardSelection()
{
	for ( CardStack* stack : m_cardStacks )
	{
		if ( stack != nullptr )
		{
			stack->SetCardsSelected( false );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Board::RemoveEmptyStacks()
{
	for ( int stackIndex = ( int ) m_cardStacks.size() - 1; stackIndex >= 0; --stackIndex )
	{
		CardStack* stack = m_cardStacks[stackIndex];

		if ( stack != nullptr && stack->IsEmpty() )
		{
			delete stack;
			m_cardStacks.erase( m_cardStacks.begin() + stackIndex );
		}
	}
}