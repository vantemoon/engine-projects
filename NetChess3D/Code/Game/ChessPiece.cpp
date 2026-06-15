#include "Game/ChessPiece.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Math/Easing.hpp"
#include "Engine/Math/Mat44.hpp"


//-----------------------------------------------------------------------------------------------
ChessPiece::ChessPiece( ChessPieceDefinition const& definition, bool isWhite /*= true*/, IntVec2 boardCoords /*= IntVec2( -1, -1 )*/ )
	: m_definition( &definition )
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
	if ( !m_isMoving )
	{
		return;
	}

	float deltaSeconds = ( float ) g_app->m_game->m_gameClock->GetDeltaSeconds();

	m_moveAgeSeconds += deltaSeconds;

	if ( m_moveAgeSeconds >= m_moveDurationSeconds )
	{
		m_moveAgeSeconds = m_moveDurationSeconds;
		m_isMoving = false;
	}
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

	VertexBuffer* vbo = m_definition->GetVertexBufferForColor( m_isWhite );
	IndexBuffer* ibo = m_definition->GetIndexBufferForColor( m_isWhite );
	unsigned int indexCount = m_definition->GetIndexCountForColor( m_isWhite );

	if ( vbo == nullptr || ibo == nullptr || indexCount == 0 )
	{
		return;
	}

	Rgba8 pieceTint;
	if ( m_isWhite )
	{
		pieceTint = Rgba8( 80, 180, 120, 255 );
	}
	else
	{
		pieceTint = Rgba8( 180, 80, 80, 255 );
	}

	Vec3 pieceWorldPos = GetCurrentPosition();

	Mat44 modelMatrix = Mat44::MakeTranslation3D( pieceWorldPos );
	Texture* pieceTexture = renderer->CreateOrGetTextureFromFile( "Data/Images/Marble.jpg" );

	renderer->BindTexture( pieceTexture );
	renderer->SetModelConstants( modelMatrix, pieceTint );

	renderer->DrawIndexedVertexBuffer( vbo, ibo, indexCount );
}


//-----------------------------------------------------------------------------------------------
void ChessPiece::StartMovementAnimation( IntVec2 const& from, IntVec2 const& to )
{
	m_isMoving = true;
	m_moveAgeSeconds = 0.f;
	m_moveDurationSeconds = 0.35f;

	m_moveStartCoords = from;
	m_moveEndCoords = to;
}


//-----------------------------------------------------------------------------------------------
bool ChessPiece::IsMovementAnimationComplete() const
{
	return !m_isMoving;
}


//-----------------------------------------------------------------------------------------------
Vec3 ChessPiece::GetCurrentPosition() const
{
	IntVec2 coords = m_boardCoords;

	if ( !m_isMoving )
	{
		return Vec3(
			static_cast<float>( coords.x ) + 0.5f,
			static_cast<float>( coords.y ) + 0.5f,
			0.f
		);
	}

	float t = m_moveAgeSeconds / m_moveDurationSeconds;
	float easedT = SmoothStep3( t );

	Vec3 startPos(
		static_cast<float>( m_moveStartCoords.x ) + 0.5f,
		static_cast<float>( m_moveStartCoords.y ) + 0.5f,
		0.f
	);

	Vec3 endPos(
		static_cast<float>( m_moveEndCoords.x ) + 0.5f,
		static_cast<float>( m_moveEndCoords.y ) + 0.5f,
		0.f
	);

	Vec3 currentPos = startPos + ( endPos - startPos ) * easedT;

	if ( m_definition != nullptr && m_definition->m_type == ChessPieceType::KNIGHT )
	{
		float hopHeight = 0.75f;
		currentPos.z += sinf( t * 3.1415926f ) * hopHeight;
	}

	return currentPos;
}