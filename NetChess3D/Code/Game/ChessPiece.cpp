#include "Game/ChessPiece.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Math/Mat44.hpp"


//-----------------------------------------------------------------------------------------------
ChessPiece::ChessPiece( ChessPieceDefinition const& definition, bool isWhite /*= true*/, IntVec2 boardCoords /*= IntVec2( -1, -1 )*/ )
	: m_definition( definition )
	, m_isWhite( isWhite )
	, m_boardCoords( boardCoords )
{
}


//-----------------------------------------------------------------------------------------------
ChessPiece::~ChessPiece()
{
}


//-----------------------------------------------------------------------------------------------
void ChessPiece::Update()
{
}


//-----------------------------------------------------------------------------------------------
void ChessPiece::Render() const
{
	if ( g_engine == nullptr || g_engine->m_renderer == nullptr )
	{
		return;
	}

	if ( m_isCaptured )
	{
		return;
	}

	Renderer* renderer = g_engine->m_renderer;

	VertexBuffer* vbo = m_definition.GetVertexBufferForColor( m_isWhite );
	IndexBuffer* ibo = m_definition.GetIndexBufferForColor( m_isWhite );
	unsigned int indexCount = m_definition.GetIndexCountForColor( m_isWhite );

	if ( vbo == nullptr || ibo == nullptr || indexCount == 0 )
	{
		return;
	}

	Rgba8 pieceTint;
	if ( m_isWhite )
	{
		pieceTint = Rgba8( 180, 80, 80, 255 );
	}
	else
	{
		pieceTint = Rgba8( 80, 180, 120, 255 );
	}

	Vec3 pieceWorldPos(
		static_cast<float>( m_boardCoords.x ) + 0.5f,
		static_cast<float>( m_boardCoords.y ) + 0.5f,
		0.f
	);

	Mat44 modelMatrix = Mat44::MakeTranslation3D( pieceWorldPos );
	Texture* pieceTexture = renderer->CreateOrGetTextureFromFile( "Data/Images/Marble.jpg" );

	renderer->BindShader( renderer->CreateOrGetShader( "Default", VertexType::VERTEX_PCUTBN ) );
	renderer->BindTexture( pieceTexture );
	renderer->SetModelConstants( modelMatrix, pieceTint );
	renderer->DrawIndexedVertexBuffer( vbo, ibo, indexCount );
}