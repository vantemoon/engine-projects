#pragma once


//-----------------------------------------------------------------------------------------------
class ChessBoard;
class ChessPlayer;


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

public:
	ChessBoard* m_board = nullptr;

	ChessPlayer* m_whitePlayer = nullptr;
	ChessPlayer* m_blackPlayer = nullptr;
	ChessPlayer* m_currentPlayer = nullptr;
};