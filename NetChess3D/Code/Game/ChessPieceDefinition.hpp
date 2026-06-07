#pragma once
#include "Engine/Core/Vertex.hpp"
#include <map>
#include <string>
#include <vector>


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

private:
	static void AddVertsAndIndicesForPawn();
	static void AddVertsAndIndicesForRook();
	static void AddVertsAndIndicesForKnight();
	static void AddVertsAndIndicesForBishop();
	static void AddVertsAndIndicesForQueen();
	static void AddVertsAndIndicesForKing();

public:
	ChessPieceType m_type = ChessPieceType::PAWN;

	std::string m_name = "Unknown";
	char m_symbol = '?';

	std::vector<Vertex> m_whiteVerts;
	std::vector<Vertex> m_blackVerts;

	std::vector<unsigned int> m_whiteIndices;
	std::vector<unsigned int> m_blackIndices;
};