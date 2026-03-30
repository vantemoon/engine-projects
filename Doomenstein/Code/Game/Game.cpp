#include "Game/Game.hpp"
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
	// Clear definitions
	MapDefinition::ClearDefinitions();
	TileDefinition::ClearDefinitions();

	delete m_screenCamera;
	m_screenCamera = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
	Player* player = new Player( this );
	m_player = player;

	// Load all definitions
	MapDefinition::InitializeDefinitions();
	TileDefinition::InitializeDefinitions();

	int numMapsToLoad = g_gameConfigBlackboard.GetValue( "numOfMaps", 2 );
	std::string defaultMapName = g_gameConfigBlackboard.GetValue( "defaultMapName", "TestMap" );
	for ( int mapIndex = 0; mapIndex < numMapsToLoad; ++mapIndex )
	{
		MapDefinition const* mapDef = MapDefinition::GetMapDefinitionByIndex( mapIndex );
		if ( mapDef != nullptr )
		{
			Map* newMap = new Map( this, *mapDef );
			m_maps.push_back( newMap );

			std::string mapName = mapDef->m_name;
			if ( mapName == defaultMapName )
			{
				m_currentMap = newMap;
			}
		}
	}

	if ( m_currentMap == nullptr && !m_maps.empty() )
	{
		m_currentMap = m_maps[0];
	}
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

	if ( m_isScreenShaking )
	{
		float shakeElapsedTime = ( float ) m_gameClock->GetTotalSeconds() - m_screenShakeStartTime;
		if ( shakeElapsedTime < m_screenShakeDuration )
		{
			float t = shakeElapsedTime / m_screenShakeDuration;
			float currentIntensity = m_screenShakeIntensity * ( 1.f - t );
			ScreenShake( currentIntensity );
		}
		else
		{
			m_isScreenShaking = false;
			m_screenShakeIntensity = 0.f;
			m_screenShakeDuration = 0.f;
			m_screenShakeStartTime = 0.f;
			m_player->m_playerCamera->SetPerspectiveView( g_engine->m_window->m_config.m_clientAspect, 60.f, 0.1f, 100.f );
		}
	}
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
		// 1
		if ( g_engine->m_inputSystem->WasKeyJustPressed( '1' ) )
		{
			Vec3 start = m_player->m_position;
			Vec3 forward = m_player->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
			Vec3 end = start + forward * 20.f;
			DebugAddWorldCylinder( start, end, 0.0625f, 10.f, Rgba8::YELLOW, Rgba8::YELLOW, DebugRenderMode::X_RAY );
		}

		// 2
		if ( g_engine->m_inputSystem->IsKeyDown( '2' ) )
		{
			Vec3 center = Vec3( m_player->m_position.x, m_player->m_position.y, 0.f );
			DebugAddWorldSphere( center, 0.25f, 60.f, Rgba8( 150, 75, 0 ), Rgba8( 150, 75, 0 ), DebugRenderMode::USE_DEPTH );
		}

		// 3
		if ( g_engine->m_inputSystem->WasKeyJustPressed( '3' ) )
		{
			Vec3 center = m_player->m_position;
			Vec3 forward = m_player->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
			center += forward * 2.f;
			DebugAddWorldWireSphere( center, 1.f, 5.f, Rgba8::GREEN, Rgba8::RED, DebugRenderMode::USE_DEPTH );
		}

		// 4
		if ( g_engine->m_inputSystem->WasKeyJustPressed( '4' ) )
		{
			DebugAddBasis( m_player->GetModelToWorldTransform(), 20.f, 1.f, 0.08f, 1.f, 1.f, DebugRenderMode::USE_DEPTH );
		}

		// 5
		if ( g_engine->m_inputSystem->WasKeyJustPressed( '5' ) )
		{
			std::string debugText = Stringf(
				"Position: %.2f, %.2f, %.2f\nOrientation: %.1f, %.1f, %.1f",
				m_player->m_position.x, m_player->m_position.y, m_player->m_position.z,
				m_player->m_orientation.m_yawDegrees, m_player->m_orientation.m_pitchDegrees, m_player->m_orientation.m_rollDegrees );
			Vec3 textPosition = m_player->m_position + m_player->m_orientation.GetForwardDir_IFwd_JLeft_KUp() * 2.f;

			DebugAddWorldBillboardText( debugText, textPosition, 0.125f, Vec2( 0.5f, 0.5f ), 10.f, Rgba8::WHITE, Rgba8::RED );
		}

		// 6
		if ( g_engine->m_inputSystem->WasKeyJustPressed( '6' ) )
		{
			Vec3 start = m_player->m_position;
			Vec3 end = start + Vec3( 0.f, 0.f, 1.f );
			DebugAddWorldWireCylinder( start, end, 0.5f, 10.f, Rgba8::WHITE, Rgba8::RED, DebugRenderMode::USE_DEPTH );
		}

		// 7 
		if ( g_engine->m_inputSystem->WasKeyJustPressed( '7' ) )
		{
			EulerAngles cameraOrientation = m_player->m_playerCamera->GetOrientation();
			float yaw = cameraOrientation.m_yawDegrees;
			float pitch = cameraOrientation.m_pitchDegrees;
			float roll = cameraOrientation.m_rollDegrees;

			std::string debugText = Stringf(
				"Camera Orientation: %.2f, %.2f, %.2f",
				yaw, pitch, roll );

			DebugAddMessage( debugText, 5.f );
		}

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
	m_player->Update( ( float ) m_gameClock->GetDeltaSeconds() );
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

	if ( m_isDebugFeaturesOn )
	{
		DebugDraw();
	}

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
	Startup();
	m_isScreenShaking = false;
	m_isDebugFeaturesOn = false;
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