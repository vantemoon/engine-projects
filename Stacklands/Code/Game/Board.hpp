#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class Card;


//-----------------------------------------------------------------------------------------------
class Board
{
public:
	Board();
	~Board();

	void Update( float deltaSeconds, Vec2 const& mouseWorldPosition );
	void Render() const;

private:
	void CreateTestCards();

	int GetTopCardIndexAtPosition( Vec2 const& worldPosition ) const;

	void BeginDraggingCardStack( int cardIndex, Vec2 const& mouseWorldPosition );
	void UpdateDraggingCardStack( Vec2 const& mouseWorldPosition );
	void EndDraggingCardStack();

	void ClearCardSelection();

private:
	Vec2 m_bottomLeft;
	Vec2 m_bottomRight;
	Vec2 m_topRight;
	Vec2 m_topLeft;

	std::vector<Card*> m_cards;

	std::vector<Card*> m_draggedCards;
	std::vector<Vec2> m_dragOffsets;

	bool m_isDraggingCard = false;
};