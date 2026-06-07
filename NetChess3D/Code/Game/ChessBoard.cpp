#include "Game/ChessBoard.hpp"
#include "Game/ChessPiece.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB3.hpp"


//-----------------------------------------------------------------------------------------------
ChessBoard::ChessBoard()
{
	Reset();
}


//-----------------------------------------------------------------------------------------------
ChessBoard::~ChessBoard()
{
}


//-----------------------------------------------------------------------------------------------
void ChessBoard::Update()
{
}


//-----------------------------------------------------------------------------------------------
void ChessBoard::Render() const
{
	if ( g_engine == nullptr || g_engine->m_renderer == nullptr )
	{
		return;
	}

	Renderer* renderer = g_engine->m_renderer;

	constexpr float squareSize = 1.f;
	constexpr float boardSize = 8.f * squareSize;
	constexpr float marginSize = squareSize / 3.f;
	constexpr float boardTopZ = 0.f;
	constexpr float boardBottomZ = -marginSize;

	Rgba8 const lightSquareColor = Rgba8( 190, 175, 145, 255 );
	Rgba8 const darkSquareColor = Rgba8( 95, 75, 55, 255 );
	Rgba8 const frameColor = Rgba8( 120, 80, 45, 255 );

	std::vector<Vertex> boardVerts;
	boardVerts.reserve( 64 * 6 + 10 * 6 );

	AddVertsForQuad3D(
		boardVerts,
		Vec3( -marginSize, -marginSize, boardTopZ ),
		Vec3( boardSize + marginSize, -marginSize, boardTopZ ),
		Vec3( boardSize + marginSize, 0.f, boardTopZ ),
		Vec3( -marginSize, 0.f, boardTopZ ),
		frameColor,
		AABB2( Vec2( 0.f, 0.f ), Vec2( boardSize + 2.f * marginSize, marginSize ) )
	);

	AddVertsForQuad3D(
		boardVerts,
		Vec3( -marginSize, boardSize, boardTopZ ),
		Vec3( boardSize + marginSize, boardSize, boardTopZ ),
		Vec3( boardSize + marginSize, boardSize + marginSize, boardTopZ ),
		Vec3( -marginSize, boardSize + marginSize, boardTopZ ),
		frameColor,
		AABB2( Vec2( 0.f, boardSize ), Vec2( boardSize + 2.f * marginSize, boardSize + marginSize ) )
	);

	AddVertsForQuad3D(
		boardVerts,
		Vec3( -marginSize, 0.f, boardTopZ ),
		Vec3( 0.f, 0.f, boardTopZ ),
		Vec3( 0.f, boardSize, boardTopZ ),
		Vec3( -marginSize, boardSize, boardTopZ ),
		frameColor,
		AABB2( Vec2( 0.f, 0.f ), Vec2( marginSize, boardSize ) )
	);

	AddVertsForQuad3D(
		boardVerts,
		Vec3( boardSize, 0.f, boardTopZ ),
		Vec3( boardSize + marginSize, 0.f, boardTopZ ),
		Vec3( boardSize + marginSize, boardSize, boardTopZ ),
		Vec3( boardSize, boardSize, boardTopZ ),
		frameColor,
		AABB2( Vec2( boardSize, 0.f ), Vec2( boardSize + marginSize, boardSize ) )
	);

	for ( int row = 0; row < 8; ++row )
	{
		for ( int col = 0; col < 8; ++col )
		{
			float minX = static_cast< float >( col ) * squareSize;
			float minY = static_cast< float >( row ) * squareSize;
			float maxX = minX + squareSize;
			float maxY = minY + squareSize;

			bool isLightSquare = ( ( row + col ) % 2 ) == 0;
			Rgba8 squareColor = isLightSquare ? lightSquareColor : darkSquareColor;

			AddVertsForQuad3D(
				boardVerts,
				Vec3( minX, minY, boardTopZ ),
				Vec3( maxX, minY, boardTopZ ),
				Vec3( maxX, maxY, boardTopZ ),
				Vec3( minX, maxY, boardTopZ ),
				squareColor,
				AABB2( Vec2( minX, minY ), Vec2( maxX, maxY ) )
			);
		}
	}

	AddVertsForQuad3D(
		boardVerts,
		Vec3( -marginSize, boardSize + marginSize, boardBottomZ ),
		Vec3( boardSize + marginSize, boardSize + marginSize, boardBottomZ ),
		Vec3( boardSize + marginSize, -marginSize, boardBottomZ ),
		Vec3( -marginSize, -marginSize, boardBottomZ ),
		frameColor,
		AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) )
	);

	AddVertsForQuad3D(
		boardVerts,
		Vec3( -marginSize, -marginSize, boardBottomZ ),
		Vec3( boardSize + marginSize, -marginSize, boardBottomZ ),
		Vec3( boardSize + marginSize, -marginSize, boardTopZ ),
		Vec3( -marginSize, -marginSize, boardTopZ ),
		frameColor,
		AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) )
	);

	AddVertsForQuad3D(
		boardVerts,
		Vec3( boardSize + marginSize, boardSize + marginSize, boardBottomZ ),
		Vec3( -marginSize, boardSize + marginSize, boardBottomZ ),
		Vec3( -marginSize, boardSize + marginSize, boardTopZ ),
		Vec3( boardSize + marginSize, boardSize + marginSize, boardTopZ ),
		frameColor,
		AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) )
	);

	AddVertsForQuad3D(
		boardVerts,
		Vec3( -marginSize, boardSize + marginSize, boardBottomZ ),
		Vec3( -marginSize, -marginSize, boardBottomZ ),
		Vec3( -marginSize, -marginSize, boardTopZ ),
		Vec3( -marginSize, boardSize + marginSize, boardTopZ ),
		frameColor,
		AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) )
	);

	AddVertsForQuad3D(
		boardVerts,
		Vec3( boardSize + marginSize, -marginSize, boardBottomZ ),
		Vec3( boardSize + marginSize, boardSize + marginSize, boardBottomZ ),
		Vec3( boardSize + marginSize, boardSize + marginSize, boardTopZ ),
		Vec3( boardSize + marginSize, -marginSize, boardTopZ ),
		frameColor,
		AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) )
	);

	Texture* boardTexture = renderer->CreateOrGetTextureFromFile( "Data/Images/Wood.jpg" );

	renderer->BindShader( renderer->CreateOrGetShader( "Default", VertexType::VERTEX_PCUTBN ) );
	renderer->BindTexture( boardTexture );
	renderer->SetModelConstants();
	renderer->SetBlendMode( BlendMode::OPAQUE );
	renderer->SetSamplerMode( SamplerMode::BILINEAR_WRAP );
	renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
	renderer->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );

	renderer->DrawVertexArray( boardVerts );
}


//-----------------------------------------------------------------------------------------------
void ChessBoard::Reset()
{
	for ( int row = 0; row < 8; row++ )
	{
		for ( int col = 0; col < 8; col++ )
		{
			m_squares[row][col] = nullptr;
		}
	}
	m_pieces.clear();

	ChessPieceDefinition const* pawnDef = ChessPieceDefinition::GetDefinitionByType( ChessPieceType::PAWN );
	ChessPieceDefinition const* rookDef = ChessPieceDefinition::GetDefinitionByType( ChessPieceType::ROOK );
	ChessPieceDefinition const* knightDef = ChessPieceDefinition::GetDefinitionByType( ChessPieceType::KNIGHT );
	ChessPieceDefinition const* bishopDef = ChessPieceDefinition::GetDefinitionByType( ChessPieceType::BISHOP );
	ChessPieceDefinition const* queenDef = ChessPieceDefinition::GetDefinitionByType( ChessPieceType::QUEEN );
	ChessPieceDefinition const* kingDef = ChessPieceDefinition::GetDefinitionByType( ChessPieceType::KING );

	m_squares[1][0] = new ChessPiece( *pawnDef, true );
	m_squares[1][1] = new ChessPiece( *pawnDef, true );
	m_squares[1][2] = new ChessPiece( *pawnDef, true );
	m_squares[1][3] = new ChessPiece( *pawnDef, true );
	m_squares[1][4] = new ChessPiece( *pawnDef, true );
	m_squares[1][5] = new ChessPiece( *pawnDef, true );
	m_squares[1][6] = new ChessPiece( *pawnDef, true );
	m_squares[1][7] = new ChessPiece( *pawnDef, true );
	m_pieces.push_back( m_squares[1][0] );
	m_pieces.push_back( m_squares[1][1] );
	m_pieces.push_back( m_squares[1][2] );
	m_pieces.push_back( m_squares[1][3] );
	m_pieces.push_back( m_squares[1][4] );
	m_pieces.push_back( m_squares[1][5] );
	m_pieces.push_back( m_squares[1][6] );
	m_pieces.push_back( m_squares[1][7] );

	m_squares[0][0] = new ChessPiece( *rookDef, true );
	m_squares[0][1] = new ChessPiece( *knightDef, true );
	m_squares[0][2] = new ChessPiece( *bishopDef, true );
	m_squares[0][3] = new ChessPiece( *queenDef, true );
	m_squares[0][4] = new ChessPiece( *kingDef, true );
	m_squares[0][5] = new ChessPiece( *bishopDef, true );
	m_squares[0][6] = new ChessPiece( *knightDef, true );
	m_squares[0][7] = new ChessPiece( *rookDef, true );
	m_pieces.push_back( m_squares[0][0] );
	m_pieces.push_back( m_squares[0][1] );
	m_pieces.push_back( m_squares[0][2] );
	m_pieces.push_back( m_squares[0][3] );
	m_pieces.push_back( m_squares[0][4] );
	m_pieces.push_back( m_squares[0][5] );
	m_pieces.push_back( m_squares[0][6] );
	m_pieces.push_back( m_squares[0][7] );

	m_squares[6][0] = new ChessPiece( *pawnDef, false );
	m_squares[6][1] = new ChessPiece( *pawnDef, false );
	m_squares[6][2] = new ChessPiece( *pawnDef, false );
	m_squares[6][3] = new ChessPiece( *pawnDef, false );
	m_squares[6][4] = new ChessPiece( *pawnDef, false );
	m_squares[6][5] = new ChessPiece( *pawnDef, false );
	m_squares[6][6] = new ChessPiece( *pawnDef, false );
	m_squares[6][7] = new ChessPiece( *pawnDef, false );
	m_pieces.push_back( m_squares[6][0] );
	m_pieces.push_back( m_squares[6][1] );
	m_pieces.push_back( m_squares[6][2] );
	m_pieces.push_back( m_squares[6][3] );
	m_pieces.push_back( m_squares[6][4] );
	m_pieces.push_back( m_squares[6][5] );
	m_pieces.push_back( m_squares[6][6] );
	m_pieces.push_back( m_squares[6][7] );

	m_squares[7][0] = new ChessPiece( *rookDef, false );
	m_squares[7][1] = new ChessPiece( *knightDef, false );
	m_squares[7][2] = new ChessPiece( *bishopDef, false );
	m_squares[7][3] = new ChessPiece( *queenDef, false );
	m_squares[7][4] = new ChessPiece( *kingDef, false );
	m_squares[7][5] = new ChessPiece( *bishopDef, false );
	m_squares[7][6] = new ChessPiece( *knightDef, false );
	m_squares[7][7] = new ChessPiece( *rookDef, false );
	m_pieces.push_back( m_squares[7][0] );
	m_pieces.push_back( m_squares[7][1] );
	m_pieces.push_back( m_squares[7][2] );
	m_pieces.push_back( m_squares[7][3] );
	m_pieces.push_back( m_squares[7][4] );
	m_pieces.push_back( m_squares[7][5] );
	m_pieces.push_back( m_squares[7][6] );
	m_pieces.push_back( m_squares[7][7] );
}


//-----------------------------------------------------------------------------------------------
IntVec2 ChessBoard::ParseSquareCoords( std::string const& text, std::string const& paramName )
{
	if ( text.length() != 2 )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 0, 0 ), "Illegal \"" + paramName + "=\" square \"" + text + "\"; must be a two-letter [Column][Rank]" );
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 128, 0 ), "Examples:  E2, E4; A1 is bottom left and H8 is top-right" );
		return IntVec2( -1, -1 );
	}

	char file = text[0];
	char rank = text[1];

	int col = file - 'A';
	if ( file >= 'a' && file <= 'z' )
	{
		col = file - 'a';
	}
	int row = rank - '1';

	if ( col < 0 || col > 7 || row < 0 || row > 7 )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 0, 0 ), "Illegal \"" + paramName + "=\" square \"" + text + "\"; must be a two-letter [Column][Rank]" );
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 128, 0 ), " Examples:  E2, E4; A1 is bottom left and H8 is top-right" );
		return IntVec2( -1, -1 );
	}

	return IntVec2( col, row );
}


//-----------------------------------------------------------------------------------------------
void ChessBoard::MovePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to )
{
	if ( piece == nullptr ) return;

	m_squares[from.y][from.x] = nullptr;
	m_squares[to.y][to.x] = piece;
}


//-----------------------------------------------------------------------------------------------
void ChessBoard::CapturePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to )
{
	if ( piece == nullptr ) return;

	ChessPiece* capturedPiece = m_squares[to.y][to.x];
	if ( capturedPiece )
	{
		capturedPiece->m_isCaptured = true;
	}

	m_squares[from.y][from.x] = nullptr;
	m_squares[to.y][to.x] = piece;
}