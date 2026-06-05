#pragma once
#include "Game/ChessObject.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class ChessPiece;


//-----------------------------------------------------------------------------------------------
class ChessBoard : public ChessObject
{
public:
	ChessBoard();
	~ChessBoard();

	void Update() override;
	void Render() const override;

public:
	ChessPiece* m_squares[8][8] = { nullptr };
	std::vector<ChessPiece*> m_pieces;
};