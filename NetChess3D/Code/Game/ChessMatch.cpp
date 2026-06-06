#include "Game/ChessMatch.hpp"
#include "Game/App.hpp"
#include "Game/ChessBoard.hpp"
#include "Game/ChessPiece.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Game/ChessPlayer.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/DevConsole.hpp"


//-----------------------------------------------------------------------------------------------
ChessMatch::ChessMatch()
{
	ChessPieceDefinition::InitializeDefinitions();
	Startup();

	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "ChessMove", Command_MovePiece );
	g_engine->m_eventSystem->SetEventRequiredArgs( "ChessMove", { "from=<string>", "to=<string>" } );
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


//-----------------------------------------------------------------------------------------------
void ChessMatch::SwitchPlayerTurn()
{
	if ( m_currentPlayer == m_whitePlayer )
	{
		m_currentPlayer = m_blackPlayer;
	}
	else
	{
		m_currentPlayer = m_whitePlayer;
	}
}


//-----------------------------------------------------------------------------------------------
void ChessMatch::MovePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to )
{
	if ( piece == nullptr ) 
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 0, 0 ), "Error: Attempted to move a null piece!" );
		return;
	}

	m_board->MovePiece( piece, from, to );
	SwitchPlayerTurn();
}


//-----------------------------------------------------------------------------------------------
bool ChessMatch::Command_MovePiece( EventArgs& args )
{
	if ( g_app && g_app->m_game && g_app->m_game->m_chessMatch )
	{
		std::string from = args.GetValue( "from", "" );
		std::string to = args.GetValue( "to", "" );
		
		IntVec2 fromCoords = ChessBoard::ParseSquareCoords( from.c_str() );
		IntVec2 toCoords = ChessBoard::ParseSquareCoords( to.c_str() );

		ChessPiece* pieceToMove = g_app->m_game->m_chessMatch->m_board->m_squares[fromCoords.y][fromCoords.x];
		if ( pieceToMove == nullptr )
		{
			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 0, 0 ), "Error: No piece at the 'from' coordinates: " + from );
			return false;
		}

		g_app->m_game->m_chessMatch->MovePiece( pieceToMove, fromCoords, toCoords );
		g_app->m_game->PrintBoardStateToDevConsole();
		return true;
	}
	return false;
}