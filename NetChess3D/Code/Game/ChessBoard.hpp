#pragma once
#include "Game/ChessObject.hpp"
#include "Engine/Math/IntVec2.hpp"
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
	void Reset();

	static IntVec2 ParseSquareCoords( char const* text );
	void MovePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to );

public:
	ChessPiece* m_squares[8][8] = { nullptr };
	std::vector<ChessPiece*> m_pieces;
};