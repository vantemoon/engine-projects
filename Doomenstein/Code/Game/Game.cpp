#include "Game/Game.hpp"
#include "Game/Actor.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Player.hpp"
#include "Game/TileDefinition.hpp"
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
#include "Engine/Math/Mat44.hpp"
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

	LoadGameConfigFromFile( "Data/GameConfig.xml" );
	Startup();
}


//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	Shutdown();
}


//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
	// Load all definitions
	MapDefinition::InitializeDefinitions();
	TileDefinition::InitializeDefinitions();
	ActorDefinition::InitializeDefinitions();

	LoadMaps();
}


//-----------------------------------------------------------------------------------------------
void Game::LoadGameConfigFromFile( char const* filepath )
{
	tinyxml2::XMLDocument xmlDocument;
	XmlResult loadResult = xmlDocument.LoadFile( filepath );

	if ( loadResult != tinyxml2::XML_SUCCESS )
	{
		GUARANTEE_OR_DIE( false, Stringf( "Failed to load game config XML file '%s'", filepath ) );
		return;
	}

	XmlElement* rootElement = xmlDocument.RootElement();
	if ( rootElement == nullptr )
	{
		GUARANTEE_OR_DIE( false, Stringf( "Failed to find root element in game config XML file '%s'", filepath ) );
		return;
	}

	g_gameConfigBlackboard.PopulateFromXmlElementAttributes( *rootElement );
}


//-----------------------------------------------------------------------------------------------
void Game::LoadMaps()
{
	int numMapsToLoad = g_gameConfigBlackboard.GetValue( "numOfMaps", 2 );
	std::string defaultMapName = g_gameConfigBlackboard.GetValue( "defaultMapName", "TestMap" );

	for ( int mapIndex = 0; mapIndex < numMapsToLoad; ++mapIndex )
	{
		MapDefinition const* mapDef = MapDefinition::GetMapDefinitionByIndex( mapIndex );
		if ( mapDef == nullptr )
		{
			continue;
		}

		Map* newMap = new Map( this, *mapDef );
		m_maps.push_back( newMap );

		if ( mapDef->m_name == defaultMapName )
		{
			m_currentMap = newMap;
		}
	}

	if ( m_currentMap == nullptr && !m_maps.empty() )
	{
		m_currentMap = m_maps[0];
	}

	if ( m_currentMap != nullptr && m_player != nullptr )
	{
		m_currentMap->SpawnPlayer( m_player );
	}
}


//-----------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	DestroyMaps();

	if ( m_player != nullptr )
	{
		delete m_player;
		m_player = nullptr;
	}

	// Clear definitions
	MapDefinition::ClearDefinitions();
	TileDefinition::ClearDefinitions();
	ActorDefinition::ClearDefinitions();

	delete m_screenCamera;
	m_screenCamera = nullptr;

	delete m_gameClock;
	m_gameClock = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Game::DestroyMaps()
{
	for ( Map* map : m_maps )
	{
		delete map;
	}
	m_maps.clear();
	m_currentMap = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Game::DeleteGarbageEntities()
{
	if ( g_app->m_game == nullptr )
		return;

	// Delete entities that are marked as garbage
}


//-----------------------------------------------------------------------------------------------
void Game::Update()
{
	if ( !m_hasControlsBeenShown )
	{
		g_engine->m_devConsole->ToggleMode( DevConsoleMode::OPEN_FULL );
		AddInstructionsToDevConsole();
		m_hasControlsBeenShown = true;
	}

	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		UpdateAttractMode();
		return;
	};

	float deltaSeconds = 0.f;
	if ( m_gameClock != nullptr )
	{
		deltaSeconds = ( float ) m_gameClock->GetDeltaSeconds();
	}

	if ( m_currentGameState == GameState::PAUSED )
	{
		UpdateFromKeyboard();
		UpdateFromController();

		UpdatePlayer( deltaSeconds );
		return;
	};

	UpdateFromKeyboard();
	UpdateFromController();
	UpdatePlayer( deltaSeconds );
	UpdateCurrentMap( deltaSeconds );

	g_app->m_game->DeleteGarbageEntities();

	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
}


//-----------------------------------------------------------------------------------------------
void Game::ScreenShake( float intensity )
{
	if ( m_player->m_playerCamera == nullptr || intensity <= 0.f )
		return;

	RandomNumberGenerator rng;
	float offsetX = rng.RollRandomFloatInRange( -intensity, intensity );
	float offsetY = rng.RollRandomFloatInRange( -intensity, intensity );
	m_player->m_playerCamera->Translate2D( Vec2( offsetX, offsetY ) );
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
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) || g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) )
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
		// F2
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F2 ) )
		{
			if ( m_currentMap != nullptr )
			{
				m_currentMap->AddToSunDirectionX( -1.f );
				float sunDirectionX = m_currentMap->GetSunDirection().x;
				std::string debugText = Stringf( "Sun Direction X: %.2f", sunDirectionX );
				DebugAddMessage( debugText, 2.f );
			}
		}

		// F3
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F3 ) )
		{
			if ( m_currentMap != nullptr )
			{
				m_currentMap->AddToSunDirectionX( 1.f );
				float sunDirectionX = m_currentMap->GetSunDirection().x;
				std::string debugText = Stringf( "Sun Direction X: %.2f", sunDirectionX );
				DebugAddMessage( debugText, 2.f );
			}
		}

		// F4
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F4 ) )
		{
			if ( m_currentMap != nullptr )
			{
				m_currentMap->AddToSunDirectionY( -1.f );
				float sunDirectionY = m_currentMap->GetSunDirection().y;
				std::string debugText = Stringf( "Sun Direction Y: %.2f", sunDirectionY );
				DebugAddMessage( debugText, 2.f );
			}
		}

		// F5
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F5 ) )
		{
			if ( m_currentMap != nullptr )
			{
				m_currentMap->AddToSunDirectionY( 1.f );
				float sunDirectionY = m_currentMap->GetSunDirection().y;
				std::string debugText = Stringf( "Sun Direction Y: %.2f", sunDirectionY );
				DebugAddMessage( debugText, 2.f );
			}
		}

		// F6
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F6 ) )
		{
			if ( m_currentMap != nullptr )
			{
				m_currentMap->AddToSunIntensity( -0.05f );
				float sunIntensity = m_currentMap->GetSunIntensity();
				std::string debugText = Stringf( "Sun Intensity: %.2f", sunIntensity );
				DebugAddMessage( debugText, 2.f );
			}
		}

		// F7
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F7 ) )
		{
			if ( m_currentMap != nullptr )
			{
				m_currentMap->AddToSunIntensity( 0.05f );
				float sunIntensity = m_currentMap->GetSunIntensity();
				std::string debugText = Stringf( "Sun Intensity: %.2f", sunIntensity );
				DebugAddMessage( debugText, 2.f );
			}
		}

		// F8
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F8 ) )
		{
			if ( m_currentMap != nullptr )
			{
				m_currentMap->AddToAmbientIntensity( -0.05f );
				float ambientIntensity = m_currentMap->GetAmbientIntensity();
				std::string debugText = Stringf( "Ambient Intensity: %.2f", ambientIntensity );
				DebugAddMessage( debugText, 2.f );
			}
		}

		// F9
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F9 ) )
		{
			if ( m_currentMap != nullptr )
			{
				m_currentMap->AddToAmbientIntensity( 0.05f );
				float ambientIntensity = m_currentMap->GetAmbientIntensity();
				std::string debugText = Stringf( "Ambient Intensity: %.2f", ambientIntensity );
				DebugAddMessage( debugText, 2.f );
			}
		}

		// Return to attract mode
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
			DestroyMaps();

			if ( m_player != nullptr )
			{
				delete m_player;
				m_player = nullptr;
			}
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
			Reset();
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
			DestroyMaps();

			if ( m_player != nullptr )
			{
				delete m_player;
				m_player = nullptr;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::UpdatePlayer( float deltaSeconds )
{
	if ( m_player == nullptr )
	{
		return;
	}

	m_player->Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateCurrentMap( float deltaSeconds )
{
	if ( m_currentMap != nullptr )
	{
		m_currentMap->Update( deltaSeconds );
	}
}


//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		RenderAttractMode();
		return;
	};

	// Clear screen
	g_engine->m_renderer->ClearScreen( Rgba8( 50, 50, 50 ) );

	if ( m_player == nullptr || m_player->m_playerCamera == nullptr )
	{
		return;
	}

	g_engine->m_renderer->BeginCamera( *m_player->m_playerCamera );

	RenderMap();
	RenderEntities();
	RenderHUD();

	DebugRenderWorld( *m_player->m_playerCamera );
	DebugRenderScreen( *m_screenCamera );

	g_engine->m_renderer->EndCamera( *m_player->m_playerCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderMap() const
{
	if ( m_currentMap != nullptr )
	{
		m_currentMap->Render();
	}
}


//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	// Clear screen
	g_engine->m_renderer->ClearScreen( Rgba8( 150, 150, 150 ) );

	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderEntities() const
{
	g_engine->m_renderer->BeginCamera( *m_player->m_playerCamera );

	m_player->Render();

	g_engine->m_renderer->EndCamera( *m_player->m_playerCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderHUD() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	g_engine->m_renderer->SetBlendMode( BlendMode::ALPHA );
	g_engine->m_renderer->SetDepthMode( DepthMode::DISABLED );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );
	g_engine->m_renderer->BindTexture( nullptr );

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

	Actor* playerActor = nullptr;
	if ( m_player != nullptr )
	{
		playerActor = m_player->GetActor();

		std::string playerPosText = Stringf(
			"Player Position: (%.2f, %.2f, %.2f)",
			m_player->m_position.x,
			m_player->m_position.y,
			m_player->m_position.z );

		DebugAddMessage( playerPosText, 0.f );
	}

	if ( playerActor != nullptr )
	{
		std::string healthText = Stringf( "Health: %d / %d", playerActor->m_currentHealth, playerActor->m_maxHealth );
		AABB2 healthTextBounds( Vec2( 0.f, 8.f ), Vec2( SCREEN_SIZE_X, 48.f ) );
		DebugAddScreenText(
			healthText,
			healthTextBounds,
			20.f,
			Vec2( 0.5f, 0.f ),
			0.f,
			Rgba8::WHITE,
			Rgba8::WHITE );
	}

	if ( m_currentMap != nullptr )
	{
		Vec3 sunDirection = m_currentMap->GetSunDirection();
		float sunDirectionX = sunDirection.x;
		float sunDirectionY = sunDirection.y;
		float sunIntensity = m_currentMap->GetSunIntensity();
		float ambientIntensity = m_currentMap->GetAmbientIntensity();

		std::string lightingText = Stringf(
			"\n\nSun Direction X: %.2f [F2/F3 to change]\nSun Direction Y: %.2f [F4/F5 to change]\nSun Intensity: %.2f [F6/F7 to change]\nAmbient Intensity: %.2f [F8/F9 to change]",
			sunDirectionX,
			sunDirectionY,
			sunIntensity,
			ambientIntensity );

		DebugAddScreenText(
			lightingText,
			screenBounds,
			15.f,
			Vec2( 1.f, 1.f ),
			0.f,
			Rgba8::WHITE,
			Rgba8::WHITE );
	}

	if ( playerActor != nullptr && playerActor->m_isDead )
	{
		std::vector<Vertex> overlayVerts;
		AddVertsForAABB2D( overlayVerts, screenBounds, Rgba8( 96, 96, 96, 150 ) );
		g_engine->m_renderer->BindTexture( nullptr );
		g_engine->m_renderer->SetModelConstants();
		g_engine->m_renderer->DrawVertexArray( overlayVerts );
	}

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
	DestroyMaps();

	if ( m_player != nullptr )
	{
		delete m_player;
		m_player = nullptr;
	}

	m_player = new Player( this );

	LoadMaps();
}


//-----------------------------------------------------------------------------------------------
bool Game::Command_Controls( EventArgs& args )
{
	UNUSED( args );
	if ( g_engine && g_engine->m_devConsole )
	{
		g_app->m_game->AddInstructionsToDevConsole();
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
void Game::AddInstructionsToDevConsole() const
{
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::MAGENTA, "Controls" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "" );

	// Attract Mode
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::MAGENTA, "[Attract Mode]" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Space      | P:           Start game" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Esc:                      Exit" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "" );

	// First-Person Camera
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::MAGENTA, "[First-Person Camera]" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Mouse      | Right Stick: Look (yaw / pitch)" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "WASD       | Left Stick:  Move (relative to camera)" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Shift      | A (Hold):    Sprint" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "LMB        | RT (Hold):   Fire weapon" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Arrow keys | D-Pad L/R:   Switch weapon" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "1          | X:           Weapon 1" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "2          | Y:           Weapon 2" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "F:                        Toggle free-fly" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "N:                        Possess next actor" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "P          | Start:       Pause / Resume" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Esc:                      Back to attract" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "" );

	// Free-Fly Camera
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::MAGENTA, "[Free-Fly Camera]" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Mouse      | Right Stick: Look (yaw / pitch)" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "WASD       | Left Stick:  Move (relative to camera)" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Z/C        | LB/RB:       Move up / down" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Shift      | A (Hold):    Fast move" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "F:                        Toggle free-fly" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "P          | Start:       Pause / Resume" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "H          | Back:        Reset camera" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( Rgba8::INFO_MINOR, "Esc:                      Back to attract" );
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