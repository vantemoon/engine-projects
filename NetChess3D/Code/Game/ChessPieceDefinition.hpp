#pragma once
#include <map>
#include <string>


//-----------------------------------------------------------------------------------------------
enum class ChessPieceType
{
	PAWN,
	ROOK,
	KNIGHT,
	BISHOP,
	QUEEN,
	KING,
	NUM_TYPES
};


//-----------------------------------------------------------------------------------------------
class ChessPieceDefinition
{
public:
	static std::map<std::string, ChessPieceDefinition*> s_definitions;

	static void InitializeDefinitions();
	static void ClearDefinitions();
	static ChessPieceDefinition const* GetDefinitionByType( ChessPieceType type );

public:
	ChessPieceType m_type = ChessPieceType::PAWN;
	char m_symbol = '?';
};