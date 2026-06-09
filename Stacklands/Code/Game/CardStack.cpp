#include "Game/CardStack.hpp"
#include "Game/Card.hpp"


//-----------------------------------------------------------------------------------------------
CardStack::CardStack()
{
}


//-----------------------------------------------------------------------------------------------
CardStack::~CardStack()
{
	for ( Card* card : m_cards )
	{
		delete card;
	}

	m_cards.clear();
}


//-----------------------------------------------------------------------------------------------
void CardStack::Update()
{
	for ( Card* card : m_cards )
	{
		if ( card != nullptr )
		{
			card->Update();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void CardStack::Render() const
{
	for ( Card* card : m_cards )
	{
		if ( card != nullptr )
		{
			card->Render();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void CardStack::AddCard( Card* card )
{
	if ( card != nullptr )
	{
		m_cards.push_back( card );
	}
}


//-----------------------------------------------------------------------------------------------
void CardStack::AddCards( std::vector<Card*> const& cards )
{
	for ( Card* card : cards )
	{
		AddCard( card );
	}
}


//-----------------------------------------------------------------------------------------------
int CardStack::GetCardCount() const
{
	return ( int ) m_cards.size();
}


//-----------------------------------------------------------------------------------------------
Card* CardStack::GetCardAtIndex( int index ) const
{
	if ( index < 0 || index >= ( int ) m_cards.size() )
	{
		return nullptr;
	}

	return m_cards[index];
}


//-----------------------------------------------------------------------------------------------
Card* CardStack::GetTopCard() const
{
	if ( m_cards.empty() )
	{
		return nullptr;
	}

	return m_cards.back();
}


//-----------------------------------------------------------------------------------------------
int CardStack::GetTopCardIndexAtPosition( Vec2 const& worldPosition ) const
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
std::vector<Card*> CardStack::RemoveCardsFromIndex( int startIndex )
{
	std::vector<Card*> removedCards;

	if ( startIndex < 0 || startIndex >= ( int ) m_cards.size() )
	{
		return removedCards;
	}

	for ( int index = startIndex; index < ( int ) m_cards.size(); ++index )
	{
		removedCards.push_back( m_cards[index] );
	}

	m_cards.erase( m_cards.begin() + startIndex, m_cards.end() );

	return removedCards;
}


//-----------------------------------------------------------------------------------------------
void CardStack::SetCardsSelected( bool isSelected )
{
	for ( Card* card : m_cards )
	{
		if ( card != nullptr )
		{
			card->SetIsSelected( isSelected );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void CardStack::MoveByOffset( Vec2 const& offset )
{
	for ( Card* card : m_cards )
	{
		if ( card != nullptr )
		{
			card->SetPosition( card->GetPosition() + offset );
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool CardStack::IsEmpty() const
{
	return m_cards.empty();
}