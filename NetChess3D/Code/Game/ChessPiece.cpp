#include "Game/ChessPiece.hpp"


//-----------------------------------------------------------------------------------------------
ChessPiece::ChessPiece( ChessPieceDefinition const& definition, bool isWhite /*= true*/ )
	: m_definition( definition )
	, m_isWhite( isWhite )
{
}


//-----------------------------------------------------------------------------------------------
ChessPiece::~ChessPiece()
{
}


//-----------------------------------------------------------------------------------------------
void ChessPiece::Update()
{
}


//-----------------------------------------------------------------------------------------------
void ChessPiece::Render() const
{
}