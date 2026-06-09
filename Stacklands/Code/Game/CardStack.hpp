#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class Card;


//-----------------------------------------------------------------------------------------------
class CardStack
{
public:
	CardStack();
	~CardStack();

	void Update();
	void Render() const;

	void AddCard( Card* card );
	void AddCards( std::vector<Card*> const& cards );

	int GetCardCount() const;
	Card* GetCardAtIndex( int index ) const;
	Card* GetTopCard() const;

	int GetTopCardIndexAtPosition( Vec2 const& worldPosition ) const;

	std::vector<Card*> RemoveCardsFromIndex( int startIndex );

	void SetCardsSelected( bool isSelected );
	void MoveByOffset( Vec2 const& offset );

	bool IsEmpty() const;

private:
	std::vector<Card*> m_cards;
};