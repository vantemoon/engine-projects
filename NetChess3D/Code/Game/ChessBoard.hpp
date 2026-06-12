#pragma once
#include "Game/ChessObject.hpp"
#include "Game/ChessPieceDefinition.hpp"
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
	bool IsPathClear( IntVec2 const& from, IntVec2 const& to ) const;
	bool IsLegalPieceMove( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to, bool isCapturing ) const;

	bool MovePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to, bool teleport = false );
	bool CapturePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to, bool teleport = false );
	bool OverrideBoard( std::string const& boardText );

	void CreatePiece( ChessPieceDefinition const& definition, bool isWhite, IntVec2 const& boardCoords );
	void DestroyPiece( ChessPiece* piece );

public:
	ChessPiece* m_squares[8][8] = { nullptr };
	std::vector<ChessPiece*> m_pieces;
};