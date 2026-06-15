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
	for ( ChessPiece* piece : m_pieces )
	{
		if ( piece != nullptr )
		{
			piece->Update();
		}
	}

	for ( int i = 0; i < ( int ) m_pieces.size(); ++i )
	{
		ChessPiece* piece = m_pieces[i];
		if ( piece == nullptr )
		{
			continue;
		}

		if ( !piece->m_hasPendingPromotion )
		{
			continue;
		}

		if ( !piece->IsMovementAnimationComplete() )
		{
			continue;
		}

		PromotePawn( piece, *piece->m_pendingPromotionDefinition );

		break;
	}
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
				AABB2::ZERO_TO_ONE
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
		AABB2::ZERO_TO_ONE
	);

	AddVertsForQuad3D(
		boardVerts,
		Vec3( -marginSize, -marginSize, boardBottomZ ),
		Vec3( boardSize + marginSize, -marginSize, boardBottomZ ),
		Vec3( boardSize + marginSize, -marginSize, boardTopZ ),
		Vec3( -marginSize, -marginSize, boardTopZ ),
		frameColor,
		AABB2::ZERO_TO_ONE
	);

	AddVertsForQuad3D(
		boardVerts,
		Vec3( boardSize + marginSize, boardSize + marginSize, boardBottomZ ),
		Vec3( -marginSize, boardSize + marginSize, boardBottomZ ),
		Vec3( -marginSize, boardSize + marginSize, boardTopZ ),
		Vec3( boardSize + marginSize, boardSize + marginSize, boardTopZ ),
		frameColor,
		AABB2::ZERO_TO_ONE
	);

	AddVertsForQuad3D(
		boardVerts,
		Vec3( -marginSize, boardSize + marginSize, boardBottomZ ),
		Vec3( -marginSize, -marginSize, boardBottomZ ),
		Vec3( -marginSize, -marginSize, boardTopZ ),
		Vec3( -marginSize, boardSize + marginSize, boardTopZ ),
		frameColor,
		AABB2::ZERO_TO_ONE
	);

	AddVertsForQuad3D(
		boardVerts,
		Vec3( boardSize + marginSize, -marginSize, boardBottomZ ),
		Vec3( boardSize + marginSize, boardSize + marginSize, boardBottomZ ),
		Vec3( boardSize + marginSize, boardSize + marginSize, boardTopZ ),
		Vec3( boardSize + marginSize, -marginSize, boardTopZ ),
		frameColor,
		AABB2::ZERO_TO_ONE
	);

	Texture* boardDiffuseTexture = renderer->CreateOrGetTextureFromFile( "Data/Images/oak_diffuse.png" );
	Texture* boardNormalTexture = renderer->CreateOrGetTextureFromFile( "Data/Images/oak_normal.png" );

	renderer->BindTexture( 0, boardDiffuseTexture );
	renderer->BindTexture( 1, boardNormalTexture );

	renderer->BindSampler( 0, SamplerMode::BILINEAR_WRAP );
	renderer->BindSampler( 1, SamplerMode::BILINEAR_WRAP );

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
	ClearEnPassantState();

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
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal \"" + paramName + "=\" square \"" + text + "\"; must be a two-letter [Column][Rank]" );
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
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal \"" + paramName + "=\" square \"" + text + "\"; must be a two-letter [Column][Rank]" );
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
			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; path is blocked." );
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
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; no piece was provided." );
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

			if ( isCapturing )
			{
				if ( absDeltaX == 1 && deltaY == forwardDirection )
				{
					return true;
				}

				g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; pawns can only capture one square diagonally forward." );
				return false;
			}

			if ( deltaX == 0 && deltaY == forwardDirection )
			{
				if ( m_squares[to.y][to.x] == nullptr )
				{
					return true;
				}

				g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; pawn cannot move forward into an occupied square." );
				return false;
			}

			if ( deltaX == 0 && !piece->m_hasMoved && deltaY == 2 * forwardDirection )
			{
				int middleRow = from.y + forwardDirection;

				if ( m_squares[middleRow][from.x] == nullptr && m_squares[to.y][to.x] == nullptr )
				{
					return true;
				}

				g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; pawn's two-square move is blocked." );
				return false;
			}

			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; pawn movement is invalid." );
			return false;
		}

		case ChessPieceType::ROOK:
		{
			if ( deltaX == 0 || deltaY == 0 )
			{
				return IsPathClear( from, to );
			}

			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; rooks must move horizontally or vertically." );
			return false;
		}

		case ChessPieceType::KNIGHT:
		{
			if ( ( absDeltaX == 1 && absDeltaY == 2 ) || ( absDeltaX == 2 && absDeltaY == 1 ) )
			{
				return true;
			}

			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; knights must move in an L shape." );
			return false;
		}

		case ChessPieceType::BISHOP:
		{
			if ( absDeltaX == absDeltaY )
			{
				return IsPathClear( from, to );
			}

			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; bishops must move diagonally." );
			return false;
		}

		case ChessPieceType::QUEEN:
		{
			bool movesStraight = deltaX == 0 || deltaY == 0;
			bool movesDiagonally = absDeltaX == absDeltaY;

			if ( movesStraight || movesDiagonally )
			{
				return IsPathClear( from, to );
			}

			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; queens must move horizontally, vertically, or diagonally." );
			return false;
		}

		case ChessPieceType::KING:
		{
			if ( IsCastlingMove( piece, from, to ) )
			{
				return IsLegalCastlingMove( piece, from, to );
			}

			if ( absDeltaX > 1 || absDeltaY > 1 )
			{
				g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; kings can only move one square." );
				return false;
			}

			IntVec2 king2Pos = GetKingPosition( !piece->m_isWhite );
			if ( !AreKingsApart( to, king2Pos ) )
			{
				g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; kings cannot be adjacent." );
				return false;
			}

			return true;
		}

		case ChessPieceType::NUM_TYPES:
			break;

		default:
			break;
	}

	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; unknown piece type." );
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
bool ChessBoard::HasReachedEndRow( ChessPiece* piece, IntVec2 const& to ) const
{
	if ( piece == nullptr )
	{
		return false;
	}
	return ( piece->m_isWhite && to.y == 7 ) || ( !piece->m_isWhite && to.y == 0 );
}


//-----------------------------------------------------------------------------------------------
ChessPieceDefinition const* ChessBoard::GetPromotionPieceDefinition( std::string const& promoteTo ) const
{
	std::string promoteToLower = promoteTo;

	for ( int i = 0; i < ( int ) promoteToLower.size(); ++i )
	{
		if ( promoteToLower[i] >= 'A' && promoteToLower[i] <= 'Z' )
		{
			promoteToLower[i] = promoteToLower[i] - 'A' + 'a';
		}
	}

	if ( promoteToLower == "queen" )
	{
		return ChessPieceDefinition::GetDefinitionByType( ChessPieceType::QUEEN );
	}
	else if ( promoteToLower == "rook" )
	{
		return ChessPieceDefinition::GetDefinitionByType( ChessPieceType::ROOK );
	}
	else if ( promoteToLower == "bishop" )
	{
		return ChessPieceDefinition::GetDefinitionByType( ChessPieceType::BISHOP );
	}
	else if ( promoteToLower == "knight" )
	{
		return ChessPieceDefinition::GetDefinitionByType( ChessPieceType::KNIGHT );
	}

	return nullptr;
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::IsLegalPromotion( ChessPiece* piece, IntVec2 const& to, std::string const& promoteTo ) const
{
	if ( piece == nullptr )
	{
		return false;
	}

	if ( piece->m_definition->m_type != ChessPieceType::PAWN )
	{
		if ( promoteTo != "" )
		{
			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; promoteTo can only be used by pawns." );
			return false;
		}

		return true;
	}

	bool reachesEndRow = HasReachedEndRow( piece, to );

	if ( reachesEndRow )
	{
		if ( promoteTo == "" )
		{
			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; pawn promotion requires promoteTo=queen, bishop, rook, or knight." );
			return false;
		}

		if ( GetPromotionPieceDefinition( promoteTo ) == nullptr )
		{
			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; promoteTo must be queen, bishop, rook, or knight." );
			return false;
		}

		return true;
	}

	if ( promoteTo != "" )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal move; promoteTo can only be used when a pawn reaches the far end row." );
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::MovePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to, bool teleport, std::string const& promoteTo )
{
	if ( piece == nullptr )
	{
		return false;
	}

	bool isCastling = !teleport && IsCastlingMove( piece, from, to );

	if ( !teleport &&!IsLegalPieceMove( piece, from, to, false ) )
	{
		return false;
	}

	if ( !IsLegalPromotion( piece, to, promoteTo ) )
	{
		return false;
	}

	m_squares[from.y][from.x] = nullptr;
	m_squares[to.y][to.x] = piece;
	piece->m_boardCoords = to;

	piece->StartMovementAnimation( from, to );

	if ( isCastling )
	{
		if ( !MoveRookForCastling( piece, from, to ) )
		{
			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Error: Failed to move rook while castling." );
			return false;
		}
	}

	bool reachesEndRow = HasReachedEndRow( piece, to );
	if ( reachesEndRow && piece->m_definition->m_type == ChessPieceType::PAWN )
	{
		ChessPieceDefinition const* newDef = GetPromotionPieceDefinition( promoteTo );
		if ( newDef != nullptr )
		{
			piece->m_hasPendingPromotion = true;
			piece->m_pendingPromotionDefinition = newDef;
		}
	}

	if ( !piece->m_hasMoved ) 
	{
		piece->m_hasMoved = true;
	}

	UpdateEnPassantStateAfterMove( piece, from, to, teleport );

	return true;
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::CapturePiece( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to, bool teleport, std::string const& promoteTo )
{
	if ( piece == nullptr )
	{
		return false;
	}

	bool isEnPassant = !teleport && IsEnPassantCapture( piece, from, to );

	ChessPiece* capturedPiece = nullptr;
	if ( isEnPassant )
	{
		capturedPiece = m_enPassantPawn;
	}
	else
	{
		capturedPiece = m_squares[to.y][to.x];
	}

	if ( capturedPiece == nullptr )
	{
		return false;
	}

	if ( !teleport &&!IsLegalPieceMove(piece, from, to, true ) )
	{
		return false;
	}

	if ( !IsLegalPromotion( piece, to, promoteTo ) )
	{
		return false;
	}

	DestroyPiece( capturedPiece );

	m_squares[from.y][from.x] = nullptr;
	m_squares[to.y][to.x] = piece;
	piece->m_boardCoords = to;

	piece->StartMovementAnimation( from, to );

	bool reachesEndRow = HasReachedEndRow( piece, to );
	if ( reachesEndRow && piece->m_definition->m_type == ChessPieceType::PAWN )
	{
		ChessPieceDefinition const* newDef = GetPromotionPieceDefinition( promoteTo );
		if ( newDef != nullptr )
		{
			piece->m_hasPendingPromotion = true;
			piece->m_pendingPromotionDefinition = newDef;
		}
	}

	if ( !piece->m_hasMoved )
	{
		piece->m_hasMoved = true;
	}

	UpdateEnPassantStateAfterMove( piece, from, to, teleport );

	return true;
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::OverrideBoard( std::string const& boardText )
{
	if ( boardText.length() != 64 )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal board text; must be exactly 64 characters representing the pieces on the board." );
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
					g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal board text; character '" + std::string( 1, c ) + "' is not a valid piece symbol." );
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
					g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal board text; character '" + std::string( 1, c ) + "' is not a valid piece symbol." );
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


//-----------------------------------------------------------------------------------------------
ChessPiece* ChessBoard::PromotePawn( ChessPiece* pawn, ChessPieceDefinition const& newDefinition )
{
	if ( pawn == nullptr || pawn->m_definition->m_type != ChessPieceType::PAWN )
	{
		return pawn;
	}

	IntVec2 coords = pawn->m_boardCoords;
	bool isWhite = pawn->m_isWhite;

	DestroyPiece( pawn );
	CreatePiece( newDefinition, isWhite, coords );

	return m_squares[coords.y][coords.x];
}


//-----------------------------------------------------------------------------------------------
void ChessBoard::ClearEnPassantState()
{
	m_enPassantPawn = nullptr;
	m_enPassantCaptureSquare = IntVec2( -1, -1 );
}


//-----------------------------------------------------------------------------------------------
void ChessBoard::UpdateEnPassantStateAfterMove( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to, bool teleport )
{
	ClearEnPassantState();

	if ( piece == nullptr || teleport )
	{
		return;
	}

	if ( piece->m_definition->m_type != ChessPieceType::PAWN )
	{
		return;
	}

	int deltaY = to.y - from.y;
	if ( abs( deltaY ) != 2 )
	{
		return;
	}

	m_enPassantPawn = piece;
	m_enPassantCaptureSquare = IntVec2( from.x, ( from.y + to.y ) / 2 );
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::IsEnPassantCapture( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to ) const
{
	if ( piece == nullptr )
	{
		return false;
	}

	if ( piece->m_definition->m_type != ChessPieceType::PAWN )
	{
		return false;
	}

	if ( m_enPassantPawn == nullptr )
	{
		return false;
	}

	if ( m_enPassantPawn->m_definition->m_type != ChessPieceType::PAWN )
	{
		return false;
	}

	if ( m_enPassantPawn->m_isWhite == piece->m_isWhite )
	{
		return false;
	}

	if ( to != m_enPassantCaptureSquare )
	{
		return false;
	}

	if ( m_squares[to.y][to.x] != nullptr )
	{
		return false;
	}

	int forwardDirection = piece->m_isWhite ? 1 : -1;
	int deltaX = to.x - from.x;
	int deltaY = to.y - from.y;

	IntVec2 enPassantPawnPos = m_enPassantPawn->m_boardCoords;

	if ( abs( deltaX ) != 1 || deltaY != forwardDirection )
	{
		return false;
	}

	if ( enPassantPawnPos.y != from.y )
	{
		return false;
	}

	if ( enPassantPawnPos.x != to.x )
	{
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::IsCastlingMove( ChessPiece* piece, IntVec2 const& from, IntVec2 const& to ) const
{
	if ( piece == nullptr )
	{
		return false;
	}

	if ( piece->m_definition->m_type != ChessPieceType::KING )
	{
		return false;
	}

	int deltaX = to.x - from.x;
	int deltaY = to.y - from.y;

	return deltaY == 0 && abs( deltaX ) == 2;
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::IsLegalCastlingMove( ChessPiece* king, IntVec2 const& from, IntVec2 const& to ) const
{
	if ( king == nullptr )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal castle; no king was provided." );
		return false;
	}

	if ( king->m_definition->m_type != ChessPieceType::KING )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal castle; only king can castle." );
		return false;
	}

	if ( king->m_hasMoved )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal castle; the king has already moved." );
		return false;
	}

	int deltaX = to.x - from.x;
	int deltaY = to.y - from.y;

	if ( deltaY != 0 || abs( deltaX ) != 2 )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal castle; the king must move exactly two squares horizontally." );
		return false;
	}

	int rookCol = deltaX > 0 ? 7 : 0;
	ChessPiece* rook = m_squares[from.y][rookCol];

	if ( rook == nullptr )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal castle; there is no rook on that side of the board." );
		return false;
	}

	if ( rook->m_definition->m_type != ChessPieceType::ROOK )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal castle; the piece on that side is not a rook." );
		return false;
	}

	if ( rook->m_isWhite != king->m_isWhite )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal castle; the rook belongs to the opponent." );
		return false;
	}

	if ( rook->m_hasMoved )
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal castle; the rook has already moved." );
		return false;
	}

	int stepX = deltaX > 0 ? 1 : -1;
	int currentX = from.x + stepX;

	while ( currentX != rookCol )
	{
		if ( m_squares[from.y][currentX] != nullptr )
		{
			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::ERROR, "Illegal castle; there is a piece between the king and rook." );
			return false;
		}
		currentX += stepX;
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
bool ChessBoard::MoveRookForCastling( ChessPiece* king, IntVec2 const& from, IntVec2 const& to )
{
	if ( king == nullptr )
	{
		return false;
	}

	if ( !IsCastlingMove( king, from, to ) )
	{
		return false;
	}

	int row = from.y;
	int deltaX = to.x - from.x;

	int rookFromX = deltaX > 0 ? 7 : 0;
	int rookToX = deltaX > 0 ? to.x - 1 : to.x + 1;

	ChessPiece* rook = m_squares[row][rookFromX];
	if ( rook == nullptr )
	{
		return false;
	}

	m_squares[row][rookFromX] = nullptr;
	m_squares[row][rookToX] = rook;

	rook->m_boardCoords = IntVec2( rookToX, row );
	rook->m_hasMoved = true;

	IntVec2 rookFromCoords = IntVec2( rookFromX, row );
	IntVec2 rookToCoords = IntVec2( rookToX, row );
	rook->StartMovementAnimation( rookFromCoords, rookToCoords );

	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::SUCCESS, "Castling also moved rook from " + std::string( 1, ( char ) ( 'A' + rookFromX ) ) + std::to_string( row + 1 ) + " to " + std::string( 1, ( char ) ( 'A' + rookToX ) ) + std::to_string( row + 1 ) );

	return true;
}