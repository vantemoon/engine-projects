#pragma once
#include "Game/ChessPiece.hpp"


//-----------------------------------------------------------------------------------------------
class ChessPlayer
{
public:
	ChessPlayer();
	~ChessPlayer();

	void Update();
	void Render() const;

public:
	bool m_isWhite = true;
};