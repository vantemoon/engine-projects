#include "Game/ChessBoard.hpp"
#include "Game/ChessPiece.hpp"
#include "Game/ChessPieceDefinition.hpp"


//-----------------------------------------------------------------------------------------------
ChessBoard::ChessBoard()
{
	Reset();
}


//-----------------------------------------------------------------------------------------------
ChessBoard::~ChessBoard()
{
}


//-----------------------------------------------------------------------------------------------
void ChessBoard::Update()
{
}


//-----------------------------------------------------------------------------------------------
void ChessBoard::Render() const
{
}


//-----------------------------------------------------------------------------------------------
void ChessBoard::Reset()
{
	for ( int row = 0; row < 8; row++ )
	{
		for ( int col = 0; col < 8; col++ )
		{
			m_squares[row][col] = nullptr;
		}
	}
	m_pieces.clear();

	ChessPieceDefinition const* pawnDef = ChessPieceDefinition::GetDefinitionByType( ChessPieceType::PAWN );
	ChessPieceDefinition const* rookDef = ChessPieceDefinition::GetDefinitionByType( ChessPieceType::ROOK );
	ChessPieceDefinition const* knightDef = ChessPieceDefinition::GetDefinitionByType( ChessPieceType::KNIGHT );
	ChessPieceDefinition const* bishopDef = ChessPieceDefinition::GetDefinitionByType( ChessPieceType::BISHOP );
	ChessPieceDefinition const* queenDef = ChessPieceDefinition::GetDefinitionByType( ChessPieceType::QUEEN );
	ChessPieceDefinition const* kingDef = ChessPieceDefinition::GetDefinitionByType( ChessPieceType::KING );

	m_squares[1][0] = new ChessPiece( *pawnDef, true );
	m_squares[1][1] = new ChessPiece( *pawnDef, true );
	m_squares[1][2] = new ChessPiece( *pawnDef, true );
	m_squares[1][3] = new ChessPiece( *pawnDef, true );
	m_squares[1][4] = new ChessPiece( *pawnDef, true );
	m_squares[1][5] = new ChessPiece( *pawnDef, true );
	m_squares[1][6] = new ChessPiece( *pawnDef, true );
	m_squares[1][7] = new ChessPiece( *pawnDef, true );
	m_pieces.push_back( m_squares[1][0] );
	m_pieces.push_back( m_squares[1][1] );
	m_pieces.push_back( m_squares[1][2] );
	m_pieces.push_back( m_squares[1][3] );
	m_pieces.push_back( m_squares[1][4] );
	m_pieces.push_back( m_squares[1][5] );
	m_pieces.push_back( m_squares[1][6] );
	m_pieces.push_back( m_squares[1][7] );

	m_squares[0][0] = new ChessPiece( *rookDef, true );
	m_squares[0][1] = new ChessPiece( *knightDef, true );
	m_squares[0][2] = new ChessPiece( *bishopDef, true );
	m_squares[0][3] = new ChessPiece( *queenDef, true );
	m_squares[0][4] = new ChessPiece( *kingDef, true );
	m_squares[0][5] = new ChessPiece( *bishopDef, true );
	m_squares[0][6] = new ChessPiece( *knightDef, true );
	m_squares[0][7] = new ChessPiece( *rookDef, true );
	m_pieces.push_back( m_squares[0][0] );
	m_pieces.push_back( m_squares[0][1] );
	m_pieces.push_back( m_squares[0][2] );
	m_pieces.push_back( m_squares[0][3] );
	m_pieces.push_back( m_squares[0][4] );
	m_pieces.push_back( m_squares[0][5] );
	m_pieces.push_back( m_squares[0][6] );
	m_pieces.push_back( m_squares[0][7] );

	m_squares[6][0] = new ChessPiece( *pawnDef, false );
	m_squares[6][1] = new ChessPiece( *pawnDef, false );
	m_squares[6][2] = new ChessPiece( *pawnDef, false );
	m_squares[6][3] = new ChessPiece( *pawnDef, false );
	m_squares[6][4] = new ChessPiece( *pawnDef, false );
	m_squares[6][5] = new ChessPiece( *pawnDef, false );
	m_squares[6][6] = new ChessPiece( *pawnDef, false );
	m_squares[6][7] = new ChessPiece( *pawnDef, false );
	m_pieces.push_back( m_squares[6][0] );
	m_pieces.push_back( m_squares[6][1] );
	m_pieces.push_back( m_squares[6][2] );
	m_pieces.push_back( m_squares[6][3] );
	m_pieces.push_back( m_squares[6][4] );
	m_pieces.push_back( m_squares[6][5] );
	m_pieces.push_back( m_squares[6][6] );
	m_pieces.push_back( m_squares[6][7] );

	m_squares[7][0] = new ChessPiece( *rookDef, false );
	m_squares[7][1] = new ChessPiece( *knightDef, false );
	m_squares[7][2] = new ChessPiece( *bishopDef, false );
	m_squares[7][3] = new ChessPiece( *queenDef, false );
	m_squares[7][4] = new ChessPiece( *kingDef, false );
	m_squares[7][5] = new ChessPiece( *bishopDef, false );
	m_squares[7][6] = new ChessPiece( *knightDef, false );
	m_squares[7][7] = new ChessPiece( *rookDef, false );
	m_pieces.push_back( m_squares[7][0] );
	m_pieces.push_back( m_squares[7][1] );
	m_pieces.push_back( m_squares[7][2] );
	m_pieces.push_back( m_squares[7][3] );
	m_pieces.push_back( m_squares[7][4] );
	m_pieces.push_back( m_squares[7][5] );
	m_pieces.push_back( m_squares[7][6] );
	m_pieces.push_back( m_squares[7][7] );
}