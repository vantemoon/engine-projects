#pragma once
#include "Game/ChessObject.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>
#include <string>


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

	static IntVec2 ParseSquareCoords( std::string const& text, std::string const& paramName );
	void MovePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to );
	void CapturePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to );

public:
	ChessPiece* m_squares[8][8] = { nullptr };
	std::vector<ChessPiece*> m_pieces;
};