#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class CardStack;


//-----------------------------------------------------------------------------------------------
struct CardHitResult
{
	int m_stackIndex = -1;
	int m_cardIndex = -1;

	bool IsValid() const
	{
		return m_stackIndex >= 0 && m_cardIndex >= 0;
	}
};


//-----------------------------------------------------------------------------------------------
class Board
{
public:
	Board();
	~Board();

	void Update( Vec2 const& mouseWorldPosition );
	void Render() const;

private:
	void CreateTestCards();

	CardHitResult GetTopCardHitAtPosition( Vec2 const& worldPosition ) const;

	void BeginDraggingStackFromHit( CardHitResult const& hitResult );
	void UpdateDraggedStack( Vec2 const& mouseWorldPosition );
	void EndDraggingStack();

	void ClearCardSelection();

	void RemoveEmptyStacks();

private:
	Vec2 m_bottomLeft;
	Vec2 m_bottomRight;
	Vec2 m_topRight;
	Vec2 m_topLeft;

	std::vector<CardStack*> m_cardStacks;

	CardStack* m_draggedStack = nullptr;
	Vec2 m_lastMouseWorldPosition = Vec2::ZERO;

	bool m_isDraggingCardStack = false;
};