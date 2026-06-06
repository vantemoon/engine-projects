#include "Game/ChessMatch.hpp"
#include "Game/ChessBoard.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Game/ChessPlayer.hpp"


//-----------------------------------------------------------------------------------------------
ChessMatch::ChessMatch()
{
	ChessPieceDefinition::InitializeDefinitions();
	Startup();
}


//-----------------------------------------------------------------------------------------------
ChessMatch::~ChessMatch()
{
	delete m_board;
	m_board = nullptr;

	delete m_whitePlayer;
	m_whitePlayer = nullptr;

	delete m_blackPlayer;
	m_blackPlayer = nullptr;

	ChessPieceDefinition::ClearDefinitions();
}

//-----------------------------------------------------------------------------------------------
void ChessMatch::Startup()
{
	m_board = new ChessBoard();
	m_whitePlayer = new ChessPlayer( true );
	m_blackPlayer = new ChessPlayer( false );
	m_currentPlayer = m_whitePlayer;
}


//-----------------------------------------------------------------------------------------------
void ChessMatch::Update()
{
}


//-----------------------------------------------------------------------------------------------
void ChessMatch::Render() const
{
}


//-----------------------------------------------------------------------------------------------
void ChessMatch::Reset()
{
}


//-----------------------------------------------------------------------------------------------
bool ChessMatch::IsWhitePlayerTurn() const
{
	return m_currentPlayer == m_whitePlayer;
}