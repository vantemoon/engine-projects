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
	m_board->Render();
}


//-----------------------------------------------------------------------------------------------
void ChessMatch::Reset()
{
}


//-----------------------------------------------------------------------------------------------
bool ChessMatch::IsWhitePlayerTurn() const
{
	return m_gameState == ChessGameState::WHITE_PLAYER_TURN;
}


//-----------------------------------------------------------------------------------------------
void ChessMatch::SwitchPlayerTurn()
{
	if ( m_gameState == ChessGameState::WHITE_PLAYER_TURN )
	{
		m_gameState = ChessGameState::BLACK_PLAYER_TURN;
		m_currentPlayer = m_blackPlayer;
	}
	else if ( m_gameState == ChessGameState::BLACK_PLAYER_TURN )
	{
		m_gameState = ChessGameState::WHITE_PLAYER_TURN;
		m_currentPlayer = m_whitePlayer;
	}
}


//-----------------------------------------------------------------------------------------------
std::string ChessMatch::GetPlayerName( bool isWhite )
{
	if ( isWhite )
	{
		return "Player #0 (White)";
	}

	return "Player #1 (Black)";
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
}


//-----------------------------------------------------------------------------------------------
bool ChessMatch::Command_MovePiece( EventArgs& args )
{
	if ( g_app == nullptr || g_app->m_game == nullptr || g_app->m_game->m_chessMatch == nullptr )
	{
		return false;
	}

	Game* game = g_app->m_game;
	ChessMatch* match = game->m_chessMatch;
	ChessBoard* board = match->m_board;

	if ( board == nullptr )
	{
		return false;
	}

	if ( match->m_gameState == ChessGameState::VICTORY )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MAJOR, "The match is already over; no more moves can be made." );
		return true;
	}

	std::string from = args.GetValue( "from", "" );
	std::string to = args.GetValue( "to", "" );

	IntVec2 fromCoords = ChessBoard::ParseSquareCoords( from, "from" );
	if ( fromCoords == IntVec2( -1, -1 ) )
	{
		return true;
	}

	IntVec2 toCoords = ChessBoard::ParseSquareCoords( to, "to" );
	if ( toCoords == IntVec2( -1, -1 ) )
	{
		return true;
	}

	if ( fromCoords == toCoords )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Error: The to=" + to + " square must be different from the from=" + from + " square." );
		return true;
	}

	ChessPiece* pieceToMove = board->m_squares[fromCoords.y][fromCoords.x];
	if ( pieceToMove == nullptr )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Error: The from=" + from + " square is empty; there is no piece to move." );
		return true;
	}

	bool isWhiteTurn = match->IsWhitePlayerTurn();
	if ( pieceToMove->m_isWhite != isWhiteTurn )
	{
		std::string currentPlayerName = ChessMatch::GetPlayerName( isWhiteTurn );

		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Error: Attempted to move a piece not belonging to the current player (" + currentPlayerName + ")." );
		return true;
	}

	ChessPiece* targetPiece = board->m_squares[toCoords.y][toCoords.x];
	if ( targetPiece != nullptr && targetPiece->m_isWhite == pieceToMove->m_isWhite )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Error: The to=" + to + " square is occupied by a piece belonging to the current player." );
		return true;
	}

	std::string movingPlayerName = ChessMatch::GetPlayerName( pieceToMove->m_isWhite );
	std::string movingPieceName = pieceToMove->m_definition.m_name;

	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 100, 150, 255 ), "Moved " + movingPlayerName + "'s " + movingPieceName + " from " + from + " to " + to );

	if ( targetPiece != nullptr )
	{
		std::string targetPlayerName = ChessMatch::GetPlayerName( targetPiece->m_isWhite );
		std::string targetPieceName = targetPiece->m_definition.m_name;

		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 128, 0 ), movingPlayerName + " captured " + targetPlayerName + "'s " + targetPieceName + " at " + to );

		board->CapturePiece( pieceToMove, fromCoords, toCoords );

		if ( targetPiece->m_definition.m_type == ChessPieceType::KING )
		{
			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 128, 0 ), "######################################################################" );
			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 128, 0 ), movingPlayerName + " has won the match!" );
			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 128, 0 ), "######################################################################" );

			match->m_gameState = ChessGameState::VICTORY;
			return true;
		}

		match->SwitchPlayerTurn();
		game->PrintBoardStateToDevConsole();
		return true;
	}

	match->MovePiece( pieceToMove, fromCoords, toCoords );
	match->SwitchPlayerTurn();
	game->PrintBoardStateToDevConsole();

	return true;
}