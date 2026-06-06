#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EventSystem.hpp"


//-----------------------------------------------------------------------------------------------
class ChessBoard;
class ChessPlayer;
class ChessPiece;


//-----------------------------------------------------------------------------------------------
class ChessMatch
{
public:
	ChessMatch();
	~ChessMatch();

	void Startup();
	void Update();
	void Render() const;
	void Reset();

	bool IsWhitePlayerTurn() const;
	void SwitchPlayerTurn();
	static std::string GetPlayerName( bool isWhite );

	static bool Command_MovePiece( EventArgs& args );
	void MovePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to );

public:
	ChessBoard* m_board = nullptr;

	ChessPlayer* m_whitePlayer = nullptr;
	ChessPlayer* m_blackPlayer = nullptr;
	ChessPlayer* m_currentPlayer = nullptr;
};