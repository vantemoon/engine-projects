#pragma once
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
class Board
{
public:
	Board();
	~Board();

	void Update( float deltaSeconds );
	void Render() const;

private:
	Vec2 m_bottomLeft;
	Vec2 m_bottomRight;
	Vec2 m_topRight;
	Vec2 m_topLeft;
};