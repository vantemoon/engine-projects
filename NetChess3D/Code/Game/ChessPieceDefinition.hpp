#pragma once
#include "Engine/Core/Vertex.hpp"
#include <map>
#include <string>
#include <vector>

class VertexBuffer;
class IndexBuffer;

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
	~ChessPieceDefinition();

	static std::map<std::string, ChessPieceDefinition*> s_definitions;

	static void InitializeDefinitions();
	static void ClearDefinitions();
	static ChessPieceDefinition const* GetDefinitionByType( ChessPieceType type );
	static ChessPieceDefinition const* GetDefinitionBySymbol( char symbol );

	VertexBuffer* GetVertexBufferForColor( bool isWhite ) const;
	IndexBuffer* GetIndexBufferForColor( bool isWhite ) const;
	unsigned int GetIndexCountForColor( bool isWhite ) const;

private:
	static void AddVertsAndIndicesForPawn( ChessPieceDefinition& definition );
	static void AddVertsAndIndicesForRook( ChessPieceDefinition& definition );
	static void AddVertsAndIndicesForKnight( ChessPieceDefinition& definition );
	static void AddVertsAndIndicesForBishop( ChessPieceDefinition& definition );
	static void AddVertsAndIndicesForQueen( ChessPieceDefinition& definition );
	static void AddVertsAndIndicesForKing( ChessPieceDefinition& definition );

	static void CopyDefinitionToGPU( ChessPieceDefinition& definition );
	static void CopyWhiteGeometryToBlackGeometry( ChessPieceDefinition& definition );

public:
	ChessPieceType m_type = ChessPieceType::PAWN;

	std::string m_name = "Unknown";
	char m_symbol = '?';

	std::vector<Vertex> m_whiteVerts;
	std::vector<Vertex> m_blackVerts;

	std::vector<unsigned int> m_whiteIndices;
	std::vector<unsigned int> m_blackIndices;

	VertexBuffer* m_whiteVBO = nullptr;
	VertexBuffer* m_blackVBO = nullptr;

	IndexBuffer* m_whiteIBO = nullptr;
	IndexBuffer* m_blackIBO = nullptr;

	unsigned int m_whiteIndexCount = 0;
	unsigned int m_blackIndexCount = 0;
};