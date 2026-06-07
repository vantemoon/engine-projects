#pragma once
#include "Game/ChessPieceDefinition.hpp"
#include "Game/ChessObject.hpp"
#include "Engine/Math/IntVec2.hpp"


//-----------------------------------------------------------------------------------------------
class ChessPiece : public ChessObject
{
public:
	ChessPiece( ChessPieceDefinition const& definition, bool isWhite = true, IntVec2 boardCoord = IntVec2( -1, -1 ) );
	~ChessPiece();

	void Update() override;
	void Render() const override;

public:
	ChessPieceDefinition m_definition;

	bool m_isWhite = true;
	bool m_isCaptured = false;

	IntVec2 m_boardCoords = IntVec2( -1, -1 );
};