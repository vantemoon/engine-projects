#pragma once


//-----------------------------------------------------------------------------------------------
enum class ChessPieceType
{
	PAWN,
	ROOK,
	KNIGHT,
	BISHOP,
	QUEEN,
	KING
};


//-----------------------------------------------------------------------------------------------
class ChessPieceDefinition
{
public:
	ChessPieceDefinition();
	~ChessPieceDefinition();

public:
	ChessPieceType m_type;
};