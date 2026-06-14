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
	bool AreKingsApart( IntVec2 const& king1PosToBeMoved, IntVec2 const& king2Pos ) const;
	IntVec2 GetKingPosition( bool isWhite ) const;

	bool HasReachedEndRow( ChessPiece* piece, IntVec2 const& to ) const;
	ChessPieceDefinition const* GetPromotionPieceDefinition( std::string const& promoteTo ) const;
	bool IsLegalPromotion( ChessPiece* piece, IntVec2 const& to, std::string const& promoteTo ) const;

	bool IsEnPassantCapture( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to ) const;
	void UpdateEnPassantStateAfterMove( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to, bool teleport );
	void ClearEnPassantState();

	bool IsCastlingMove( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to ) const;
	bool IsLegalCastlingMove( ChessPiece* king, IntVec2 const& from, IntVec2 const& to ) const;
	bool MoveRookForCastling( ChessPiece* king, IntVec2 const& from, IntVec2 const& to );

	bool MovePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to, bool teleport = false, std::string const& promoteTo = "" );
	bool CapturePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to, bool teleport = false, std::string const& promoteTo = "" );
	bool OverrideBoard( std::string const& boardText );

	void CreatePiece( ChessPieceDefinition const& definition, bool isWhite, IntVec2 const& boardCoords );
	void DestroyPiece( ChessPiece* piece );
	ChessPiece* PromotePawn( ChessPiece* pawn, ChessPieceDefinition const& newDefinition );

public:
	ChessPiece* m_squares[8][8] = { nullptr };
	std::vector<ChessPiece*> m_pieces;

	ChessPiece* m_enPassantPawn = nullptr;
	IntVec2 m_enPassantCaptureSquare = IntVec2( -1, -1 );
};