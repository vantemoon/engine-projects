#include "Game/ChessBoard.hpp"
#include "Game/ChessPiece.hpp"
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

	Rgba8 const lightSquareColor = Rgba8( 225, 215, 185, 255 );
	Rgba8 const darkSquareColor = Rgba8( 55, 45, 38, 255 );
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

	Texture* boardTexture = renderer->CreateOrGetTextureFromFile( "Data/Images/Wood-Light.jpg" );

	renderer->BindTexture( boardTexture );
	renderer->SetModelConstants();

	renderer->DrawVertexArray( boardVerts );

	for ( ChessPiece* piece : m_pieces )
	{
		piece->Render();
	}
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

	m_squares[1][0] = new ChessPiece( *pawnDef, true, IntVec2( 0, 1 ) );
	m_squares[1][1] = new ChessPiece( *pawnDef, true, IntVec2( 1, 1 ) );
	m_squares[1][2] = new ChessPiece( *pawnDef, true, IntVec2( 2, 1 ) );
	m_squares[1][3] = new ChessPiece( *pawnDef, true, IntVec2( 3, 1 ) );
	m_squares[1][4] = new ChessPiece( *pawnDef, true, IntVec2( 4, 1 ) );
	m_squares[1][5] = new ChessPiece( *pawnDef, true, IntVec2( 5, 1 ) );
	m_squares[1][6] = new ChessPiece( *pawnDef, true, IntVec2( 6, 1 ) );
	m_squares[1][7] = new ChessPiece( *pawnDef, true, IntVec2( 7, 1 ) );
	m_pieces.push_back( m_squares[1][0] );
	m_pieces.push_back( m_squares[1][1] );
	m_pieces.push_back( m_squares[1][2] );
	m_pieces.push_back( m_squares[1][3] );
	m_pieces.push_back( m_squares[1][4] );
	m_pieces.push_back( m_squares[1][5] );
	m_pieces.push_back( m_squares[1][6] );
	m_pieces.push_back( m_squares[1][7] );

	m_squares[0][0] = new ChessPiece( *rookDef, true, IntVec2( 0, 0 ) );
	m_squares[0][1] = new ChessPiece( *knightDef, true, IntVec2( 1, 0 ) );
	m_squares[0][2] = new ChessPiece( *bishopDef, true, IntVec2( 2, 0 ) );
	m_squares[0][3] = new ChessPiece( *queenDef, true, IntVec2( 3, 0 ) );
	m_squares[0][4] = new ChessPiece( *kingDef, true, IntVec2( 4, 0 ) );
	m_squares[0][5] = new ChessPiece( *bishopDef, true, IntVec2( 5, 0 ) );
	m_squares[0][6] = new ChessPiece( *knightDef, true, IntVec2( 6, 0 ) );
	m_squares[0][7] = new ChessPiece( *rookDef, true, IntVec2( 7, 0 ) );
	m_pieces.push_back( m_squares[0][0] );
	m_pieces.push_back( m_squares[0][1] );
	m_pieces.push_back( m_squares[0][2] );
	m_pieces.push_back( m_squares[0][3] );
	m_pieces.push_back( m_squares[0][4] );
	m_pieces.push_back( m_squares[0][5] );
	m_pieces.push_back( m_squares[0][6] );
	m_pieces.push_back( m_squares[0][7] );

	m_squares[6][0] = new ChessPiece( *pawnDef, false, IntVec2( 0, 6 ) );
	m_squares[6][1] = new ChessPiece( *pawnDef, false, IntVec2( 1, 6 ) );
	m_squares[6][2] = new ChessPiece( *pawnDef, false, IntVec2( 2, 6 ) );
	m_squares[6][3] = new ChessPiece( *pawnDef, false, IntVec2( 3, 6 ) );
	m_squares[6][4] = new ChessPiece( *pawnDef, false, IntVec2( 4, 6 ) );
	m_squares[6][5] = new ChessPiece( *pawnDef, false, IntVec2( 5, 6 ) );
	m_squares[6][6] = new ChessPiece( *pawnDef, false, IntVec2( 6, 6 ) );
	m_squares[6][7] = new ChessPiece( *pawnDef, false, IntVec2( 7, 6 ) );
	m_pieces.push_back( m_squares[6][0] );
	m_pieces.push_back( m_squares[6][1] );
	m_pieces.push_back( m_squares[6][2] );
	m_pieces.push_back( m_squares[6][3] );
	m_pieces.push_back( m_squares[6][4] );
	m_pieces.push_back( m_squares[6][5] );
	m_pieces.push_back( m_squares[6][6] );
	m_pieces.push_back( m_squares[6][7] );

	m_squares[7][0] = new ChessPiece( *rookDef, false, IntVec2( 0, 7 ) );
	m_squares[7][1] = new ChessPiece( *knightDef, false, IntVec2( 1, 7 ) );
	m_squares[7][2] = new ChessPiece( *bishopDef, false, IntVec2( 2, 7 ) );
	m_squares[7][3] = new ChessPiece( *queenDef, false, IntVec2( 3, 7 ) );
	m_squares[7][4] = new ChessPiece( *kingDef, false, IntVec2( 4, 7 ) );
	m_squares[7][5] = new ChessPiece( *bishopDef, false, IntVec2( 5, 7 ) );
	m_squares[7][6] = new ChessPiece( *knightDef, false, IntVec2( 6, 7 ) );
	m_squares[7][7] = new ChessPiece( *rookDef, false, IntVec2( 7, 7 ) );
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
bool ChessBoard::IsPathClear( IntVec2 const& from, IntVec2 const& to ) const
{
	int deltaX = to.x - from.x;
	int deltaY = to.y - from.y;

	int stepX = 0;
	int stepY = 0;

	if ( deltaX > 0 ) stepX = 1;
	if ( deltaX < 0 ) stepX = -1;
	if ( deltaY > 0 ) stepY = 1;
	if ( deltaY < 0 ) stepY = -1;

	IntVec2 currentSquare = from + IntVec2( stepX, stepY );

	while ( currentSquare != to )
	{
		if ( m_squares[currentSquare.y][currentSquare.x] != nullptr )
		{
			return false;
		}

		currentSquare = currentSquare + IntVec2( stepX, stepY );
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::IsLegalPieceMove( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to, bool isCapturing ) const
{
	if ( piece == nullptr )
	{
		return false;
	}

	int deltaX = to.x - from.x;
	int deltaY = to.y - from.y;

	int absDeltaX = abs( deltaX );
	int absDeltaY = abs( deltaY );

	ChessPieceType pieceType = piece->m_definition->m_type;
	switch ( pieceType )
	{
		case ChessPieceType::PAWN:
		{
			int forwardDirection = piece->m_isWhite ? 1 : -1;
			int startRow = piece->m_isWhite ? 1 : 6;

			if ( isCapturing )
			{
				return absDeltaX == 1 && deltaY == forwardDirection;
			}

			if ( deltaX == 0 && deltaY == forwardDirection )
			{
				return m_squares[to.y][to.x] == nullptr;
			}

			if ( deltaX == 0 && from.y == startRow && deltaY == 2 * forwardDirection )
			{
				int middleRow = from.y + forwardDirection;
				return m_squares[middleRow][from.x] == nullptr && m_squares[to.y][to.x] == nullptr;
			}

			break;
		}

		case ChessPieceType::ROOK:
		{
			if ( deltaX == 0 || deltaY == 0 )
			{
				return IsPathClear( from, to );
			}

			break;
		}

		case ChessPieceType::KNIGHT:
		{
			return ( absDeltaX == 1 && absDeltaY == 2 ) || ( absDeltaX == 2 && absDeltaY == 1 );
		}

		case ChessPieceType::BISHOP:
		{
			if ( absDeltaX == absDeltaY )
			{
				return IsPathClear( from, to );
			}

			break;
		}

		case ChessPieceType::QUEEN:
		{
			bool movesStraight = deltaX == 0 || deltaY == 0;
			bool movesDiagonally = absDeltaX == absDeltaY;

			if ( movesStraight || movesDiagonally )
			{
				return IsPathClear( from, to );
			}

			break;
		}

		case ChessPieceType::KING:
		{
			IntVec2 king2Pos = GetKingPosition( !piece->m_isWhite );
			bool kingsApart = AreKingsApart( to, king2Pos );
			return kingsApart && absDeltaX <= 1 && absDeltaY <= 1;
		}

		case ChessPieceType::NUM_TYPES:
			break;

		default:
			break;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::AreKingsApart( IntVec2 const& king1PosToBeMoved, IntVec2 const& king2Pos ) const
{
	for ( int deltaY = -1; deltaY <= 1; ++deltaY )
	{
		for ( int deltaX = -1; deltaX <= 1; ++deltaX )
		{
			if ( deltaX == 0 && deltaY == 0 )
			{
				continue;
			}

			IntVec2 adjacentSquare = king1PosToBeMoved + IntVec2( deltaX, deltaY );
			if ( adjacentSquare == king2Pos )
			{
				return false;
			}
		}
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
IntVec2 ChessBoard::GetKingPosition( bool isWhite ) const
{
	for ( int row = 0; row < 8; ++row )
	{
		for ( int col = 0; col < 8; ++col )
		{
			ChessPiece* piece = m_squares[row][col];
			if ( piece != nullptr && piece->m_definition->m_type == ChessPieceType::KING && piece->m_isWhite == isWhite )
			{
				return IntVec2( col, row );
			}
		}
	}

	return IntVec2( -1, -1 );
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::MovePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to, bool teleport )
{
	if ( piece == nullptr )
	{
		return false;
	}

	if ( !teleport && !IsLegalPieceMove( piece, from, to, false ) )
	{
		return false;
	}

	m_squares[from.y][from.x] = nullptr;
	m_squares[to.y][to.x] = piece;
	piece->m_boardCoords = to;

	return true;
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::CapturePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to, bool teleport )
{
	if ( piece == nullptr )
	{
		return false;
	}

	ChessPiece* capturedPiece = m_squares[to.y][to.x];
	if ( capturedPiece == nullptr )
	{
		return false;
	}

	if ( !teleport && !IsLegalPieceMove( piece, from, to, true ) )
	{
		return false;
	}

	DestroyPiece( capturedPiece );

	m_squares[from.y][from.x] = nullptr;
	m_squares[to.y][to.x] = piece;
	piece->m_boardCoords = to;

	return true;
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::OverrideBoard( std::string const& boardText )
{
	if ( boardText.length() != 64 )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 0, 0 ), "Illegal board text; must be exactly 64 characters representing the pieces on the board." );
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 128, 0 ), "Example: board=RNBQKBNRPPPPPPPP................................pppppppprnbqkbnr" );
		return false;
	}

	for ( int i = 0; i < 64; ++i )
	{
		char c = boardText[i];
		bool isEmpty = c == '.';
		
		bool isCurrentlyEmpty = m_squares[i / 8][i % 8] == nullptr;
		if ( isCurrentlyEmpty )
		{
			if ( !isEmpty )
			{
				ChessPieceDefinition const* def = ChessPieceDefinition::GetDefinitionBySymbol( c );
				if ( def == nullptr )
				{
					g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 0, 0 ), "Illegal board text; character '" + std::string( 1, c ) + "' is not a valid piece symbol." );
					g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 128, 0 ), "Example: board=RNBQKBNRPPPPPPPP................................pppppppprnbqkbnr" );
					return false;
				}

				CreatePiece( *def, isupper( c ), IntVec2( i % 8, i / 8 ) );
			}
		}

		else
		{
			ChessPiece* existingPiece = m_squares[i / 8][i % 8];
			if ( isEmpty )
			{
				DestroyPiece( existingPiece );
				continue;
			}

			char existingSymbol = existingPiece->m_definition->m_symbol;
			existingSymbol = existingPiece->m_isWhite ? ( char ) toupper( existingSymbol ) : ( char ) tolower( existingSymbol );
			if ( existingSymbol != c )
			{
				ChessPieceDefinition const* def = ChessPieceDefinition::GetDefinitionBySymbol( c );
				if ( def == nullptr )
				{
					g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 0, 0 ), "Illegal board text; character '" + std::string( 1, c ) + "' is not a valid piece symbol." );
					g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 128, 0 ), "Example: board=RNBQKBNRPPPPPPPP................................pppppppprnbqkbnr" );
					return false;
				}
				
				DestroyPiece( existingPiece );
				CreatePiece( *def, isupper( c ), IntVec2( i % 8, i / 8 ) );
			}
		}
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
void ChessBoard::CreatePiece( ChessPieceDefinition const& definition, bool isWhite, IntVec2 const& boardCoords )
{
	if ( boardCoords.x < 0 || boardCoords.x >= 8 || boardCoords.y < 0 || boardCoords.y >= 8 )
	{
		return;
	}

	if ( m_squares[boardCoords.y][boardCoords.x] != nullptr )
	{
		DestroyPiece( m_squares[boardCoords.y][boardCoords.x] );
	}

	ChessPiece* newPiece = new ChessPiece( definition, isWhite, boardCoords );
	m_squares[boardCoords.y][boardCoords.x] = newPiece;
	m_pieces.push_back( newPiece );
}


//-----------------------------------------------------------------------------------------------
void ChessBoard::DestroyPiece( ChessPiece* piece )
{
	if ( piece == nullptr )
	{
		return;
	}

	IntVec2 coords = piece->m_boardCoords;

	if ( coords.x >= 0 && coords.x < 8 && coords.y >= 0 && coords.y < 8 )
	{
		if ( m_squares[coords.y][coords.x] == piece )
		{
			m_squares[coords.y][coords.x] = nullptr;
		}
	}

	for ( int i = 0; i < ( int ) m_pieces.size(); ++i )
	{
		if ( m_pieces[i] == piece )
		{
			m_pieces.erase( m_pieces.begin() + i );
			break;
		}
	}

	delete piece;
}