#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/ChessBoard.hpp"
#include "Game/ChessMatch.hpp"
#include "Game/ChessPiece.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#define _USE_MATH_DEFINES
#include <math.h>


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	// Cameras
	m_screenCamera = new Camera();
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	// Clock and game state
	m_gameClock = new Clock();
	m_currentGameState = ATTRACT_MODE;

	// Renderer defaults
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );

	// Register console commands
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "Controls", Command_Controls );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "PrintBoard", Command_PrintBoard );

	Startup();
}


//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	m_entities.clear();

	delete m_screenCamera;
	m_screenCamera = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
	Player* player = new Player( this );
	player->m_position = Vec3( -2.f, 0.f, 1.f );
	m_entities.push_back( player );
	m_player = player;

	m_chessMatch = new ChessMatch();
}


//-----------------------------------------------------------------------------------------------
void Game::DeleteGarbageEntities()
{
	if ( g_app->m_game == nullptr )
		return;

	// #ToDo: Delete entities that are marked as garbage
}


//-----------------------------------------------------------------------------------------------
void Game::Update()
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		UpdateAttractMode();
		return;
	};

	if ( m_currentGameState == GameState::PAUSED )
	{
		UpdateFromKeyboard();
		UpdateFromController();

		m_player->UpdateCamera();
		return;
	};

	UpdateFromKeyboard();
	UpdateFromController();
	UpdateEntities();

	g_app->m_game->DeleteGarbageEntities();

	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateAttractMode()
{
	UpdateFromKeyboard();
	UpdateFromController();
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateFromKeyboard()
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		// Start the game
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) || g_engine->m_inputSystem->WasKeyJustPressed( 'N' ) || g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) )
		{
			Reset();
			m_currentGameState = GameState::PLAYING;
		};

		// Quit the game
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
		{
			g_app->SetIsQuitting();
		}
	}
	else
	{
		// Return to attract mode
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
		}

		// Pause and resume the game
		if ( m_currentGameState != GameState::PAUSED && g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
		{
			m_currentGameState = GameState::PAUSED;
			m_gameClock->Pause();
		}

		else if ( m_currentGameState == GameState::PAUSED && g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
		{
			m_currentGameState = GameState::PLAYING;
			m_gameClock->Unpause();
		}

		// Slow the time scale to 1/10
		if ( g_engine->m_inputSystem->IsKeyDown( 'T' ) )
		{
			m_gameClock->SetTimeScale( 0.1 );
		}
		if ( g_engine->m_inputSystem->WasKeyJustReleased( 'T' ) )
		{
			m_gameClock->SetTimeScale( 1.0 );
		}

		// Toggle debug features
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F1 ) )
		{
			m_isDebugFeaturesOn = !m_isDebugFeaturesOn;
		}

		// Pause the game after the next update (for debugging)
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'O' ) )
		{
			m_currentGameState = GameState::PLAYING;
			m_gameClock->StepSingleFrame();
			m_currentGameState = GameState::PAUSED;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateFromController()
{
	XboxController const& controller = g_engine->m_inputSystem->GetController( 0 );

	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		// Start the game
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_A ) || controller.WasButtonJustPressed( XBOX_BUTTON_START ) )
		{
			// Reset();
			m_currentGameState = GameState::PLAYING;
		};
		// Quit the game
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
		{
			g_app->SetIsQuitting();
		}
	}
	else
	{
		// Return to attract mode
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateEntities()
{
	for ( Entity* entity : m_entities )
	{
		entity->Update( ( float ) m_gameClock->GetDeltaSeconds() );
	}
}


//-----------------------------------------------------------------------------------------------
void Game::DebugDraw() const
{
	if (m_player == nullptr || m_player->m_playerCamera == nullptr)
	{
		return;
	}

	g_engine->m_renderer->BeginCamera( *m_player->m_playerCamera );

	// #ToDo: Draw debug information about entities, collisions, etc.

	g_engine->m_renderer->EndCamera( *m_player->m_playerCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		RenderAttractMode();
		return;
	};

	AABB2 screenBounds( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
	std::string clockText = Stringf(
		"Time: %.2f FPS: %.1f Scale: %.2f",
		m_gameClock->GetTotalSeconds(),
		m_gameClock->GetFrameRate(),
		m_gameClock->GetTimeScale() );

	DebugAddScreenText(
		clockText,
		screenBounds,
		15.f,
		Vec2( 1.f, 1.f ),
		0.f,
		Rgba8::WHITE,
		Rgba8::WHITE );

	if ( m_player != nullptr )
	{
		std::string playerPosText = Stringf(
			"Player Position: (%.2f, %.2f, %.2f)",
			m_player->m_position.x,
			m_player->m_position.y,
			m_player->m_position.z );

		DebugAddMessage( playerPosText, 0.f );
	}

	g_engine->m_renderer->ClearScreen( Rgba8( 50, 50, 50 ) );

	if ( m_player == nullptr || m_player->m_playerCamera == nullptr )
	{
		return;
	}

	g_engine->m_renderer->BeginCamera( *m_player->m_playerCamera );

	g_engine->m_renderer->SetLightingConstants( Vec3( 3.f, 1.f, -2.f ), 1.f, 0.2f );

	Shader* defaultShader = g_engine->m_renderer->CreateOrGetShader( "Data/Shaders/Default", VertexType::VERTEX_PCUTBN );
	g_engine->m_renderer->BindShader( defaultShader );
	g_engine->m_renderer->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_renderer->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_renderer->SetSamplerMode( SamplerMode::BILINEAR_WRAP );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );

	RenderEntities();
	m_chessMatch->Render();

	DebugRenderWorld( *m_player->m_playerCamera );
	DebugRenderScreen( *m_screenCamera );

	if ( m_isDebugFeaturesOn )
	{
		DebugDraw();
	}

	g_engine->m_renderer->EndCamera( *m_player->m_playerCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	// Clear screen
	g_engine->m_renderer->ClearScreen( Rgba8( 150, 150, 150 ) );

	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	// RenderHUD();

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderEntities() const
{
	g_engine->m_renderer->BeginCamera( *m_player->m_playerCamera );

	for ( Entity* entity : m_entities )
	{
		entity->Render();
	}

	g_engine->m_renderer->EndCamera( *m_player->m_playerCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderHUD() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	// Text
	std::vector<Vertex> verts;
	
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		AddVertsForTextTriangles2D( verts, "ATTRACT MODE", Vec2( 10.f, SCREEN_SIZE_Y - 30.f ), 24.f, Rgba8( 255, 255, 255 ) );
	}
	else if ( m_currentGameState == GameState::PAUSED )
	{
		AddVertsForTextTriangles2D( verts, "PAUSED", Vec2( 10.f, SCREEN_SIZE_Y - 30.f ), 24.f, Rgba8( 255, 255, 255 ) );
	}
	else if ( m_currentGameState == GameState::PLAYING )
	{
		AddVertsForTextTriangles2D( verts, "GAME MODE", Vec2( 10.f, SCREEN_SIZE_Y - 30.f ), 24.f, Rgba8( 255, 255, 255 ) );
	}
	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
Vec3 Game::TransformWorldToScreen( Vec3 const& worldPosition ) const
{
	Mat44 transform;
	transform.Append( m_player->m_playerCamera->GetWorldToCameraTransform() );
	transform.Append( m_player->m_playerCamera->GetCameraToRenderTransform() );
	Vec3 screenPosition = transform.TransformPosition3D( worldPosition );
	return screenPosition;
}


//-----------------------------------------------------------------------------------------------
bool Game::IsOnScreen( Vec2 const& worldPosition, float cosmeticRadius ) const
{
	Vec3 screenPosition = TransformWorldToScreen( Vec3( worldPosition.x, worldPosition.y, 0.f ) );
	if ( screenPosition.z < 0.f ) return false;
	if ( screenPosition.x < -cosmeticRadius || screenPosition.x > SCREEN_SIZE_X + cosmeticRadius ) return false;
	if ( screenPosition.y < -cosmeticRadius || screenPosition.y > SCREEN_SIZE_Y + cosmeticRadius ) return false;
	return true;
}


//-----------------------------------------------------------------------------------------------
void Game::Reset()
{
	g_engine->m_eventSystem->FireEvent( "Clear" );
	m_entities.clear();
	Startup();
	m_isDebugFeaturesOn = false;
	PrintBoardStateToDevConsole();
}


//-----------------------------------------------------------------------------------------------
bool Game::Command_Controls( EventArgs& args )
{
	UNUSED( args );
	if ( g_engine && g_engine->m_devConsole )
	{
		g_app->m_game->PrintInstructionsToDevConsole();
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
void Game::PrintInstructionsToDevConsole() const
{
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::MAGENTA, "Controls" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "" );

	// KEYBOARD + MOUSE
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MAJOR, "Keyboard & Mouse" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "" );

	// Attract Mode
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::MAGENTA, "[Attract Mode]" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Space / P:                 Switch to Game Mode" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Esc:                       Exit the game" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "" );

	// Game Mode — General
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::MAGENTA, "[Game Mode: General]" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "P:                         Pause / Resume" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "O:                         Advance one frame, then pause" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "T (Hold):                  Slow game time to 1/10 speed" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Esc:                       Switch to Attract Mode" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "H:                         Reset camera position and orientation" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "" );

	// Game Mode — Movement & Rotation
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::MAGENTA, "[Game Mode: Movement & Rotation]" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Mouse X:                   Yaw" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Mouse Y:                   Pitch" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Q / E:                     Roll" );

	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "A / D:                     Move camera left / right" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "W / S:                     Move camera forward / back" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Z / C:                     Move camera down / up" );

	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Shift (Hold):              Increase camera movement speed x10" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "" );

	// CONTROLLER
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MAJOR, "Controller" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "" );

	// Game Mode — General
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::MAGENTA, "[Game Mode: General]" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Start:                     Reset camera position and orientation" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "" );

	// Game Mode — Movement & Rotation
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::MAGENTA, "[Game Mode: Movement & Rotation]" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Right Stick X:             Yaw" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Right Stick Y:             Pitch" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "LT / RT:                   Roll" );

	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Left Stick X:              Move camera left / right" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Left Stick Y:              Move camera forward / back" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "LB / RB:                   Move camera down / up" );

	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "A Button (Hold):           Increase camera movement speed x10" );
}


//-----------------------------------------------------------------------------------------------
void Game::AddVertsForCube( std::vector<Vertex>& verts, float size ) const
{
	float halfSize = size * 0.5f;

	AddVertsForQuad3D( verts,
		Vec3( halfSize, -halfSize, -halfSize ),
		Vec3( halfSize, halfSize, -halfSize ),
		Vec3( halfSize, halfSize, halfSize ),
		Vec3( halfSize, -halfSize, halfSize ),
		Rgba8::RED );

	AddVertsForQuad3D( verts,
		Vec3( -halfSize, halfSize, -halfSize ),
		Vec3( -halfSize, -halfSize, -halfSize ),
		Vec3( -halfSize, -halfSize, halfSize ),
		Vec3( -halfSize, halfSize, halfSize ),
		Rgba8::CYAN );

	AddVertsForQuad3D( verts,
		Vec3( halfSize, halfSize, -halfSize ),
		Vec3( -halfSize, halfSize, -halfSize ),
		Vec3( -halfSize, halfSize, halfSize ),
		Vec3( halfSize, halfSize, halfSize ),
		Rgba8::GREEN );

	AddVertsForQuad3D( verts,
		Vec3( -halfSize, -halfSize, -halfSize ),
		Vec3( halfSize, -halfSize, -halfSize ),
		Vec3( halfSize, -halfSize, halfSize ),
		Vec3( -halfSize, -halfSize, halfSize ),
		Rgba8::MAGENTA );

	AddVertsForQuad3D( verts,
		Vec3( -halfSize, -halfSize, halfSize ),
		Vec3( halfSize, -halfSize, halfSize ),
		Vec3( halfSize, halfSize, halfSize ),
		Vec3( -halfSize, halfSize, halfSize ),
		Rgba8::BLUE );

	AddVertsForQuad3D( verts,
		Vec3( halfSize, -halfSize, -halfSize ),
		Vec3( -halfSize, -halfSize, -halfSize ),
		Vec3( -halfSize, halfSize, -halfSize ),
		Vec3( halfSize, halfSize, -halfSize ),
		Rgba8::YELLOW );
}


//-----------------------------------------------------------------------------------------------
bool Game::Command_PrintBoard( EventArgs& args )
{
	UNUSED( args );
	if ( g_engine && g_engine->m_devConsole && g_app->m_game )
	{
		g_app->m_game->PrintBoardStateToDevConsole();
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
void Game::PrintBoardStateToDevConsole() const
{
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 128, 0 ), "============================================" );

	if ( m_chessMatch && m_chessMatch->m_board )
	{
		switch ( m_chessMatch->m_gameState )
		{	
			case ChessGameState::WHITE_PLAYER_TURN:
				g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 128, 0 ), "Player #1 (White) is your turn!" );
				g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 100, 150, 255 ), "Game state is: First Player's Turn" );
				break;
			case ChessGameState::BLACK_PLAYER_TURN:
				g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 255, 128, 0 ), "Player #2 (Black) is your turn!" );
				g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 100, 150, 255 ), "Game state is: Second Player's Turn" );
				break;
			default:
				break;
		}

		ChessBoard* board = m_chessMatch->m_board;

		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 100, 150, 255 ), "  ABCDEFGH" );
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 100, 150, 255 ), " +--------+" );

		for ( int row = 7; row >= 0; --row )
		{
			std::string line = Stringf( "%d|", row + 1 );
			for ( int col = 0; col < 8; ++col )
			{
				ChessPiece* piece = board->m_squares[row][col];
				if ( piece )
				{
					char symbol = piece->m_definition.m_symbol;
					if ( piece->m_isWhite && symbol >= 'a' && symbol <= 'z' )
					{
						symbol = symbol - 'a' + 'A';
					}
					line += symbol;
				}
				else
				{
					line += '.';
				}
			}
			line += Stringf( "|%d", row + 1 );
			g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 100, 150, 255 ), line );
		}

		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 100, 150, 255 ), " +--------+" );
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8( 100, 150, 255 ), "  ABCDEFGH" );
	}
	else
	{
		g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "No active chess board currently initialized." );
	}
}