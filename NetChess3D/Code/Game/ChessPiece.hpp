#pragma once
#include "Game/ChessPieceDefinition.hpp"
#include "Game/ChessObject.hpp"
#include "Engine/Math/IntVec2.hpp"


//-----------------------------------------------------------------------------------------------
class ChessPiece : public ChessObject
{
public:
	ChessPiece( ChessPieceDefinition const& definition, bool isWhite = true );
	~ChessPiece();

	void Update() override;
	void Render() const override;

public:
	ChessPieceDefinition m_definition;

	bool m_isWhite = true;
	bool m_isCaptured = false;
};