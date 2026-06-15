#include "Game/ChessPieceDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"


//-----------------------------------------------------------------------------------------------
std::map<std::string, ChessPieceDefinition*> ChessPieceDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
ChessPieceDefinition::~ChessPieceDefinition()
{
	delete m_whiteVBO;
	m_whiteVBO = nullptr;

	delete m_blackVBO;
	m_blackVBO = nullptr;

	delete m_whiteIBO;
	m_whiteIBO = nullptr;

	delete m_blackIBO;
	m_blackIBO = nullptr;
}


//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::InitializeDefinitions()
{
	s_definitions["PAWN"] = new ChessPieceDefinition();
	s_definitions["PAWN"]->m_type = ChessPieceType::PAWN;
	s_definitions["PAWN"]->m_name = "Pawn";
	s_definitions["PAWN"]->m_symbol = 'p';
	AddVertsAndIndicesForPawn( *s_definitions["PAWN"] );
	CopyDefinitionToGPU( *s_definitions["PAWN"] );

	s_definitions["ROOK"] = new ChessPieceDefinition();
	s_definitions["ROOK"]->m_type = ChessPieceType::ROOK;
	s_definitions["ROOK"]->m_name = "Rook";
	s_definitions["ROOK"]->m_symbol = 'r';
	AddVertsAndIndicesForRook( *s_definitions["ROOK"] );
	CopyDefinitionToGPU( *s_definitions["ROOK"] );

	s_definitions["KNIGHT"] = new ChessPieceDefinition();
	s_definitions["KNIGHT"]->m_type = ChessPieceType::KNIGHT;
	s_definitions["KNIGHT"]->m_name = "Knight";
	s_definitions["KNIGHT"]->m_symbol = 'n';
	AddVertsAndIndicesForKnight( *s_definitions["KNIGHT"] );
	CopyDefinitionToGPU( *s_definitions["KNIGHT"] );

	s_definitions["BISHOP"] = new ChessPieceDefinition();
	s_definitions["BISHOP"]->m_type = ChessPieceType::BISHOP;
	s_definitions["BISHOP"]->m_name = "Bishop";
	s_definitions["BISHOP"]->m_symbol = 'b';
	AddVertsAndIndicesForBishop( *s_definitions["BISHOP"] );
	CopyDefinitionToGPU( *s_definitions["BISHOP"] );

	s_definitions["QUEEN"] = new ChessPieceDefinition();
	s_definitions["QUEEN"]->m_type = ChessPieceType::QUEEN;
	s_definitions["QUEEN"]->m_name = "Queen";
	s_definitions["QUEEN"]->m_symbol = 'q';
	AddVertsAndIndicesForQueen( *s_definitions["QUEEN"] );
	CopyDefinitionToGPU( *s_definitions["QUEEN"] );

	s_definitions["KING"] = new ChessPieceDefinition();
	s_definitions["KING"]->m_type = ChessPieceType::KING;
	s_definitions["KING"]->m_name = "King";
	s_definitions["KING"]->m_symbol = 'k';
	AddVertsAndIndicesForKing( *s_definitions["KING"] );
	CopyDefinitionToGPU( *s_definitions["KING"] );
}


//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::ClearDefinitions()
{
	for ( std::map<std::string, ChessPieceDefinition*>::iterator it = s_definitions.begin(); it != s_definitions.end(); ++it )
	{
		delete it->second;
	}

	s_definitions.clear();
}


//-----------------------------------------------------------------------------------------------
ChessPieceDefinition const* ChessPieceDefinition::GetDefinitionByType( ChessPieceType type )
{
	for ( std::map<std::string, ChessPieceDefinition*>::iterator it = s_definitions.begin(); it != s_definitions.end(); ++it )
	{
		if ( it->second->m_type == type )
		{
			return it->second;
		}
	}
	return nullptr;
}


//-----------------------------------------------------------------------------------------------
ChessPieceDefinition const* ChessPieceDefinition::GetDefinitionBySymbol( char symbol )
{
	for ( std::map<std::string, ChessPieceDefinition*>::iterator it = s_definitions.begin(); it != s_definitions.end(); ++it )
	{
		symbol = static_cast<char>( tolower( symbol ) );
		if ( it->second->m_symbol == symbol )
		{
			return it->second;
		}
	}
	return nullptr;
}


//-----------------------------------------------------------------------------------------------
VertexBuffer* ChessPieceDefinition::GetVertexBufferForColor( bool isWhite ) const
{
	if ( isWhite )
	{
		return m_whiteVBO;
	}

	return m_blackVBO;
}


//-----------------------------------------------------------------------------------------------
IndexBuffer* ChessPieceDefinition::GetIndexBufferForColor( bool isWhite ) const
{
	if ( isWhite )
	{
		return m_whiteIBO;
	}

	return m_blackIBO;
}


//-----------------------------------------------------------------------------------------------
unsigned int ChessPieceDefinition::GetIndexCountForColor( bool isWhite ) const
{
	if ( isWhite )
	{
		return m_whiteIndexCount;
	}

	return m_blackIndexCount;
}


//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::CopyWhiteGeometryToBlackGeometry( ChessPieceDefinition& definition )
{
	definition.m_blackVerts = definition.m_whiteVerts;
	definition.m_blackIndices = definition.m_whiteIndices;

	for ( Vertex& vert : definition.m_blackVerts )
	{
		vert.m_position.x = -vert.m_position.x;
		vert.m_position.y = -vert.m_position.y;

		vert.m_tangent.x = -vert.m_tangent.x;
		vert.m_tangent.y = -vert.m_tangent.y;

		vert.m_bitangent.x = -vert.m_bitangent.x;
		vert.m_bitangent.y = -vert.m_bitangent.y;

		vert.m_normal.x = -vert.m_normal.x;
		vert.m_normal.y = -vert.m_normal.y;
	}
}


//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::CopyDefinitionToGPU( ChessPieceDefinition& definition )
{
	if ( g_engine == nullptr || g_engine->m_renderer == nullptr )
	{
		return;
	}

	CopyWhiteGeometryToBlackGeometry( definition );

	Renderer* renderer = g_engine->m_renderer;

	definition.m_whiteIndexCount = static_cast<unsigned int>( definition.m_whiteIndices.size() );
	definition.m_blackIndexCount = static_cast<unsigned int>( definition.m_blackIndices.size() );

	unsigned int whiteVertexBufferSize = static_cast<unsigned int>( definition.m_whiteVerts.size() * sizeof( Vertex ) );
	unsigned int blackVertexBufferSize = static_cast<unsigned int>( definition.m_blackVerts.size() * sizeof( Vertex ) );

	unsigned int whiteIndexBufferSize = static_cast<unsigned int>( definition.m_whiteIndices.size() * sizeof( unsigned int ) );
	unsigned int blackIndexBufferSize = static_cast<unsigned int>( definition.m_blackIndices.size() * sizeof( unsigned int ) );

	definition.m_whiteVBO = renderer->CreateVertexBuffer( whiteVertexBufferSize, sizeof( Vertex ) );
	definition.m_blackVBO = renderer->CreateVertexBuffer( blackVertexBufferSize, sizeof( Vertex ) );

	definition.m_whiteIBO = renderer->CreateIndexBuffer( whiteIndexBufferSize );
	definition.m_blackIBO = renderer->CreateIndexBuffer( blackIndexBufferSize );

	renderer->CopyCPUToGPU( definition.m_whiteVerts.data(), whiteVertexBufferSize, definition.m_whiteVBO );
	renderer->CopyCPUToGPU( definition.m_blackVerts.data(), blackVertexBufferSize, definition.m_blackVBO );

	renderer->CopyCPUToGPU( definition.m_whiteIndices.data(), whiteIndexBufferSize, definition.m_whiteIBO );
	renderer->CopyCPUToGPU( definition.m_blackIndices.data(), blackIndexBufferSize, definition.m_blackIBO );
}


//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::AddVertsAndIndicesForPawn( ChessPieceDefinition& definition )
{
	AddVertsForIndexedCylinderZ3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.f, 0.f ), Vec3( 0.f, 0.f, 0.12f ), 0.30f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64 );
	AddVertsForIndexedCylinderZ3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.f, 0.12f ), Vec3( 0.f, 0.f, 0.55f ), 0.18f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64 );
	AddVertsForIndexedSphere3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.f, 0.72f ), 0.22f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64, 32 );
}


//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::AddVertsAndIndicesForRook( ChessPieceDefinition& definition )
{
	AddVertsForIndexedAABB3D( definition.m_whiteVerts, definition.m_whiteIndices, AABB3( Vec3( -0.32f, -0.32f, 0.f ), Vec3( 0.32f, 0.32f, 0.15f ) ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	AddVertsForIndexedAABB3D( definition.m_whiteVerts, definition.m_whiteIndices, AABB3( Vec3( -0.23f, -0.23f, 0.15f ), Vec3( 0.23f, 0.23f, 0.78f ) ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	AddVertsForIndexedAABB3D( definition.m_whiteVerts, definition.m_whiteIndices, AABB3( Vec3( -0.34f, -0.34f, 0.78f ), Vec3( 0.34f, 0.34f, 0.95f ) ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
}


//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::AddVertsAndIndicesForKnight( ChessPieceDefinition& definition )
{
	AddVertsForIndexedAABB3D( definition.m_whiteVerts, definition.m_whiteIndices, AABB3( Vec3( -0.30f, -0.30f, 0.f ), Vec3( 0.30f, 0.30f, 0.14f ) ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	AddVertsForIndexedAABB3D( definition.m_whiteVerts, definition.m_whiteIndices, AABB3( Vec3( -0.18f, -0.20f, 0.14f ), Vec3( 0.18f, 0.20f, 0.65f ) ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	AddVertsForIndexedAABB3D( definition.m_whiteVerts, definition.m_whiteIndices, AABB3( Vec3( -0.12f, -0.10f, 0.60f ), Vec3( 0.34f, 0.14f, 0.90f ) ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	AddVertsForIndexedAABB3D( definition.m_whiteVerts, definition.m_whiteIndices, AABB3( Vec3( 0.16f, -0.08f, 0.48f ), Vec3( 0.38f, 0.12f, 0.68f ) ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
}


//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::AddVertsAndIndicesForBishop( ChessPieceDefinition& definition )
{
	AddVertsForIndexedCylinderZ3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.f, 0.f ), Vec3( 0.f, 0.f, 0.12f ), 0.30f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64 );
	AddVertsForIndexedCylinderZ3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.f, 0.12f ), Vec3( 0.f, 0.f, 0.70f ), 0.16f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64 );
	AddVertsForIndexedSphere3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.f, 0.86f ), 0.21f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64, 32 );
	AddVertsForIndexedAABB3D( definition.m_whiteVerts, definition.m_whiteIndices, AABB3( Vec3( -0.04f, -0.26f, 0.72f ), Vec3( 0.04f, 0.26f, 0.98f ) ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
}


//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::AddVertsAndIndicesForQueen( ChessPieceDefinition& definition )
{
	AddVertsForIndexedCylinderZ3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.f, 0.f ), Vec3( 0.f, 0.f, 0.14f ), 0.34f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64 );
	AddVertsForIndexedCylinderZ3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.f, 0.14f ), Vec3( 0.f, 0.f, 0.82f ), 0.20f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64 );
	AddVertsForIndexedSphere3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.f, 1.00f ), 0.18f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64, 32 );

	AddVertsForIndexedSphere3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( -0.20f, 0.f, 0.92f ), 0.09f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64, 32 );
	AddVertsForIndexedSphere3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.20f, 0.f, 0.92f ), 0.09f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64, 32 );
	AddVertsForIndexedSphere3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, -0.20f, 0.92f ), 0.09f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64, 32 );
	AddVertsForIndexedSphere3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.20f, 0.92f ), 0.09f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64, 32 );
}


//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::AddVertsAndIndicesForKing( ChessPieceDefinition& definition )
{
	AddVertsForIndexedCylinderZ3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.f, 0.f ), Vec3( 0.f, 0.f, 0.14f ), 0.34f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64 );
	AddVertsForIndexedCylinderZ3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.f, 0.14f ), Vec3( 0.f, 0.f, 0.85f ), 0.19f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64 );
	AddVertsForIndexedSphere3D( definition.m_whiteVerts, definition.m_whiteIndices, Vec3( 0.f, 0.f, 0.96f ), 0.15f, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 64, 32 );

	AddVertsForIndexedAABB3D( definition.m_whiteVerts, definition.m_whiteIndices, AABB3( Vec3( -0.05f, -0.05f, 1.00f ), Vec3( 0.05f, 0.05f, 1.28f ) ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
	AddVertsForIndexedAABB3D( definition.m_whiteVerts, definition.m_whiteIndices, AABB3( Vec3( -0.17f, -0.04f, 1.10f ), Vec3( 0.17f, 0.04f, 1.20f ) ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
}