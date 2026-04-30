#include "Game/Game.hpp"
#include "Game/Actor.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Player.hpp"
#include "Game/TileDefinition.hpp"
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
	// Initialize game config variables
	LoadGameConfigFromFile( "Data/GameConfig.xml" );
	InitializeGameAudio();

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

	/*if ( m_currentMap != nullptr && m_player != nullptr )
	{
		m_currentMap->SpawnPlayer( m_player );
	}*/
}


//-----------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	DestroyMaps();

	// Delete players
	for ( int playerIndex = 0; playerIndex < ( int ) m_players.size(); ++playerIndex )
	{
		Player* player = m_players[playerIndex];
		delete player;
	}
	m_players.clear();

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
void Game::InitializeGameAudio()
{
	m_musicVolume = g_gameConfigBlackboard.GetValue( "musicVolume", 0.1f );
	std::string mainMenuMusicFilePath = g_gameConfigBlackboard.GetValue( "mainMenuMusic", "Data/Audio/Music/MainMenu_InTheDark.mp2" );
	std::string gameMusicFilePath = g_gameConfigBlackboard.GetValue( "gameMusic", "Data/Audio/Music/E1M1_AtDoomsGate.mp2" );
	std::string buttonClickSoundFilePath = g_gameConfigBlackboard.GetValue( "buttonClickSound", "Data/Audio/Click.mp3" );

	m_mainMenuMusicID = g_engine->m_audioSystem->CreateOrGetSound( mainMenuMusicFilePath );
	m_gameMusicID = g_engine->m_audioSystem->CreateOrGetSound( gameMusicFilePath );
	m_buttonClickSoundID = g_engine->m_audioSystem->CreateOrGetSound( buttonClickSoundFilePath );
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
		// g_engine->m_devConsole->ToggleMode( DevConsoleMode::OPEN_FULL );
		AddInstructionsToDevConsole();
		m_hasControlsBeenShown = true;
	}

	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		if ( !m_isMainMenuMusicPlaying )
		{
			m_mainMenuMusicPlaybackID = g_engine->m_audioSystem->StartSound( m_mainMenuMusicID, true, m_musicVolume );
			m_isMainMenuMusicPlaying = true;
		}
		if ( m_isGameMusicPlaying )
		{
			g_engine->m_audioSystem->StopSound( m_gameMusicPlaybackID );
			m_isGameMusicPlaying = false;
		}

		UpdateAttractMode();
		return;
	}
	if ( m_currentGameState == GameState::LOBBY )
	{
		// No change in music from attract mode to lobby
	}
	else
	{
		if ( m_isMainMenuMusicPlaying )
		{
			g_engine->m_audioSystem->StopSound( m_mainMenuMusicPlaybackID );
			m_isMainMenuMusicPlaying = false;
		}
		if ( !m_isGameMusicPlaying )
		{
			m_gameMusicPlaybackID = g_engine->m_audioSystem->StartSound( m_gameMusicID, true, m_musicVolume );
			m_isGameMusicPlaying = true;
		}
	}

	float deltaSeconds = 0.f;
	if ( m_gameClock != nullptr )
	{
		deltaSeconds = ( float ) m_gameClock->GetDeltaSeconds();
	}

	if ( m_currentGameState == GameState::PAUSED )
	{
		UpdateFromKeyboard();
		UpdateFromController();

		UpdatePlayers( deltaSeconds );
		return;
	};

	UpdateFromKeyboard();
	UpdateFromController();
	UpdatePlayers( deltaSeconds );
	UpdateCurrentMap( deltaSeconds );
	UpdateAudioListeners();

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
		// Start the game and join player 1 with keyboard controls
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) )
		{
			g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

			Reset();
			m_currentGameState = GameState::LOBBY;
			Player* newPlayer = new Player( this, ControlMode::KEYBOARD );
			m_players.push_back( newPlayer );
			return;
		};

		// Quit the game
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
		{
			g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

			g_app->SetIsQuitting();
			return;
		}
	}
	else if ( m_currentGameState == GameState::LOBBY )
	{
		int playerCount = ( int ) m_players.size();
		if ( playerCount == 1 )
		{
			ControlMode player1ControlMode = m_players[0]->GetControlMode();
			if ( player1ControlMode == ControlMode::KEYBOARD )
			{
				// Start the game
				if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) )
				{
					g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

					StartGameFromLobby();
					return;
				};
			}
			else if ( player1ControlMode == ControlMode::CONTROLLER )
			{
				// Join player 2 with keyboard controls
				if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) )
				{
					g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

					Player* newPlayer = new Player( this, ControlMode::KEYBOARD );
					m_players.push_back( newPlayer );
				};
			}
			// Remove player 1 and return to attract mode
			if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
			{
				g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

				Player* player1 = m_players[0];
				m_players.clear();
				delete player1;
				m_currentGameState = GameState::ATTRACT_MODE;
			}
		}
		else if ( playerCount == 2 )
		{
			// Start the game
			if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) )
			{
				g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

				StartGameFromLobby();
				return;
			};
			// Remove player with keyboard controls
			if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
			{
				g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

				Player* playerToRemove = nullptr;
				for ( int playerIndex = 0; playerIndex < playerCount; ++playerIndex )
				{
					Player* player = m_players[playerIndex];
					if ( player->GetControlMode() == ControlMode::KEYBOARD )
					{
						playerToRemove = player;
						break;
					}
				}
				if ( playerToRemove != nullptr )
				{
					m_players.erase( std::remove( m_players.begin(), m_players.end(), playerToRemove ), m_players.end() );
					delete playerToRemove;
				}
			}
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

			for ( int playerIndex = 0; playerIndex < ( int ) m_players.size(); ++playerIndex )
			{
				Player* player = m_players[playerIndex];
				delete player;
			}
			m_players.clear();
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
		// Start the game and join player 1
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_START ) )
		{
			g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

			Reset();
			m_currentGameState = GameState::LOBBY;
			Player* newPlayer = new Player( this, ControlMode::CONTROLLER );
			m_players.push_back( newPlayer );
			return;
		};

		// Quit the game
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
		{
			g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

			g_app->SetIsQuitting();
			return;
		}
	}
	else if ( m_currentGameState == GameState::LOBBY )
	{
		int playerCount = ( int ) m_players.size();
		if ( playerCount == 1 )
		{
			ControlMode player1ControlMode = m_players[0]->GetControlMode();
			if ( player1ControlMode == ControlMode::CONTROLLER )
			{
				// Start the game
				if ( g_engine->m_inputSystem->GetController( 0 ).WasButtonJustPressed( XBOX_BUTTON_START ) )
				{
					g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

					StartGameFromLobby();
					return;
				};
			}
			else if ( player1ControlMode == ControlMode::KEYBOARD )
			{
				// Join player 2 with controller controls
				if ( controller.WasButtonJustPressed( XBOX_BUTTON_START ) )
				{
					g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

					Player* newPlayer = new Player( this, ControlMode::CONTROLLER );
					m_players.push_back( newPlayer );
				};
			}
			// Remove player 1 and return to attract mode
			if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
			{
				g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

				Player* player1 = m_players[0];
				m_players.clear();
				delete player1;
				m_currentGameState = GameState::ATTRACT_MODE;
			}
		}
		else if ( playerCount == 2 )
		{
			// Start the game
			if ( controller.WasButtonJustPressed( XBOX_BUTTON_START ) )
			{
				g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

				StartGameFromLobby();
				return;
			}
			// Remove player with controller controls
			if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
			{
				g_engine->m_audioSystem->StartSound( m_buttonClickSoundID, false );

				Player* playerToRemove = nullptr;
				for ( int playerIndex = 0; playerIndex < playerCount; ++playerIndex )
				{
					Player* player = m_players[playerIndex];
					if ( player->GetControlMode() == ControlMode::CONTROLLER )
					{
						playerToRemove = player;
						break;
					}
				}
				if ( playerToRemove != nullptr )
				{
					m_players.erase( std::remove( m_players.begin(), m_players.end(), playerToRemove ), m_players.end() );
					delete playerToRemove;
				}
			}
		}
	}
	else
	{
		// Return to attract mode
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
			DestroyMaps();

			for ( int playerIndex = 0; playerIndex < ( int ) m_players.size(); ++playerIndex )
			{
				Player* player = m_players[playerIndex];
				delete player;
			}
			m_players.clear();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::UpdatePlayers( float deltaSeconds )
{
	for ( int playerIndex = 0; playerIndex < ( int ) m_players.size(); ++playerIndex )
	{
		Player* player = m_players[playerIndex];
		player->Update( deltaSeconds );
	}
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
void Game::Render()
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		RenderAttractMode();
		return;
	}

	if ( m_currentGameState == GameState::LOBBY )
	{
		RenderLobby();
		return;
	}

	g_engine->m_renderer->ClearScreen( Rgba8( 50, 50, 50 ) );

	int playerCount = ( int ) m_players.size();

	for ( int playerIndex = 0; playerIndex < playerCount; ++playerIndex )
	{
		Player* player = m_players[playerIndex];
		if ( player == nullptr || player->m_playerWorldCamera == nullptr )
		{
			continue;
		}

		AABB2 viewport;
		if ( playerCount == 1 )
		{
			player->m_playerWorldCamera->SetViewport( AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) ) );
			viewport = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
		}
		else if ( playerCount == 2 )
		{
			if ( playerIndex == 0 )
			{
				// Player 1
				player->m_playerWorldCamera->SetViewport( AABB2( Vec2( 0.f, 0.5f ), Vec2( 1.f, 1.f ) ) );
				viewport = AABB2( Vec2( 0.f, 0.5f ), Vec2( 1.f, 1.f ) );
			}
			else
			{
				// Player 2
				player->m_playerWorldCamera->SetViewport( AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 0.5f ) ) );
				viewport = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 0.5f ) );
			}
		}

		player->SetPlayerIndex( playerIndex );
		player->SetViewport( viewport );

		float screenWidth = SCREEN_SIZE_X;
		float screenHeight = SCREEN_SIZE_Y * viewport.GetDimensions().y;

		player->m_playerScreenCamera->SetOrthoView(
			Vec2( 0.f, 0.f ),
			Vec2( screenWidth, screenHeight ) );

		m_currentRenderingPlayer = player;

		g_engine->m_renderer->BeginCamera( *player->m_playerWorldCamera );

		RenderMap();
		RenderEntities();
		RenderHUD();

		g_engine->m_renderer->EndCamera( *player->m_playerWorldCamera );

		DebugRenderWorld( *player->m_playerWorldCamera );
	}

	m_currentRenderingPlayer = nullptr;

	m_screenCamera->SetViewport( AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	DebugRenderScreen( *m_screenCamera );
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
	g_engine->m_renderer->ClearScreen( Rgba8( 100, 100, 100 ) );

	m_screenCamera->SetViewport( AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	BitmapFont* font = g_engine->m_renderer->CreateOrGetBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" );
	if ( font == nullptr ) return;

	std::string attractModeText1 = "Press SPACE to join with mouse and keyboard";
	std::string attractModeText2 = "Press START to join with a controller";
	std::string attractModeText = "Press ESCAPE or BACK to exit";
	std::vector<Vertex> textVerts;
	AABB2 text3Bounds = AABB2( Vec2( 0.f, 30.f ), Vec2( SCREEN_SIZE_X, 60.f ) );
	AABB2 text2Bounds = AABB2( Vec2( 0.f, 60.f ), Vec2( SCREEN_SIZE_X, 90.f ) );
	AABB2 text1Bounds = AABB2( Vec2( 0.f, 90.f ), Vec2( SCREEN_SIZE_X, 120.f ) );
	font->AddVertsForTextInBox2D( textVerts, attractModeText1, text1Bounds, 20.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.5f ) );
	font->AddVertsForTextInBox2D( textVerts, attractModeText2, text2Bounds, 20.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.5f ) );
	font->AddVertsForTextInBox2D( textVerts, attractModeText, text3Bounds, 20.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.5f ) );

	g_engine->m_renderer->BindTexture( &font->GetTexture() );
	g_engine->m_renderer->SetModelConstants();
	g_engine->m_renderer->DrawVertexArray( textVerts );
	g_engine->m_renderer->BindTexture( nullptr );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderLobby() const
{
	g_engine->m_renderer->ClearScreen( Rgba8( 100, 100, 100 ) );

	m_screenCamera->SetViewport( AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	BitmapFont* font = g_engine->m_renderer->CreateOrGetBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" );
	if ( font == nullptr ) return;
	std::vector<Vertex> textVerts;
	AABB2 textBounds = AABB2( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	if ( m_players.size() == 1 )
	{
		std::string playerTag = "Player 1";
		font->AddVertsForTextInBox2D( textVerts, playerTag, textBounds, 40.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.5f ) );

		ControlMode controlType = m_players[0]->GetControlMode();
		std::string controlTypeText = "";
		std::string startText = "";
		std::string leaveText = "";
		std::string joinText = "";
		if ( controlType == ControlMode::KEYBOARD )
		{
			controlTypeText = "Mouse and Keyboard";
			startText = "Press SPACE to start the game";
			leaveText = "Press ESCAPE to leave the game";
			joinText = "Press START to join with a controller";
		}
		else if ( controlType == ControlMode::CONTROLLER )
		{
			controlTypeText = "Controller";
			startText = "Press START to start the game";
			leaveText = "Press BACK to leave the game";
			joinText = "Press SPACE to join with mouse and keyboard";
		}
		font->AddVertsForTextInBox2D( textVerts, controlTypeText, textBounds, 25.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.425f ) );
		font->AddVertsForTextInBox2D( textVerts, startText, textBounds, 15.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.3f ) );
		font->AddVertsForTextInBox2D( textVerts, leaveText, textBounds, 15.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.275f ) );
		font->AddVertsForTextInBox2D( textVerts, joinText, textBounds, 15.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.25f ) );
	}
	else if ( m_players.size() == 2 )
	{
		std::string playerTag1 = "Player 1";
		std::string playerTag2 = "Player 2";
		font->AddVertsForTextInBox2D( textVerts, playerTag1, textBounds, 40.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.75f ) );
		font->AddVertsForTextInBox2D( textVerts, playerTag2, textBounds, 40.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.35f ) );

		ControlMode player1ControlType = m_players[0]->GetControlMode();
		ControlMode player2ControlType = m_players[1]->GetControlMode();
		std::string player1ControlTypeText = "";
		std::string player2ControlTypeText = "";
		std::string player1StartText = "";
		std::string player2StartText = "";
		std::string player1LeaveText = "";
		std::string player2LeaveText = "";
		if ( player1ControlType == ControlMode::KEYBOARD )
		{
			player1ControlTypeText = "Mouse and Keyboard";
			player1StartText = "Press SPACE to start the game";
			player1LeaveText = "Press ESCAPE to leave the game";
		}
		else if ( player1ControlType == ControlMode::CONTROLLER )
		{
			player1ControlTypeText = "Controller";
			player1StartText = "Press START to start the game";
			player1LeaveText = "Press BACK to leave the game";
		}
		if ( player2ControlType == ControlMode::KEYBOARD )
		{
			player2ControlTypeText = "Mouse and Keyboard";
			player2StartText = "Press SPACE to start the game";
			player2LeaveText = "Press ESCAPE to leave the game";
		}
		else if ( player2ControlType == ControlMode::CONTROLLER )
		{
			player2ControlTypeText = "Controller";
			player2StartText = "Press START to start the game";
			player2LeaveText = "Press BACK to leave the game";
		}
		font->AddVertsForTextInBox2D( textVerts, player1ControlTypeText, textBounds, 25.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.625f ) );
		font->AddVertsForTextInBox2D( textVerts, player1StartText, textBounds, 15.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.55f ) );
		font->AddVertsForTextInBox2D( textVerts, player1LeaveText, textBounds, 15.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.525f ) );
		font->AddVertsForTextInBox2D( textVerts, player2ControlTypeText, textBounds, 25.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.225f ) );
		font->AddVertsForTextInBox2D( textVerts, player2StartText, textBounds, 15.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.15f ) );
		font->AddVertsForTextInBox2D( textVerts, player2LeaveText, textBounds, 15.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.125f ) );
	}

	g_engine->m_renderer->BindTexture( &font->GetTexture() );
	g_engine->m_renderer->SetModelConstants();
	g_engine->m_renderer->DrawVertexArray( textVerts );
	g_engine->m_renderer->BindTexture( nullptr );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderEntities() const
{
	if ( m_currentRenderingPlayer == nullptr )
	{
		return;
	}

	for ( int playerIndex = 0; playerIndex < ( int ) m_players.size(); ++playerIndex )
	{
		Player* player = m_players[playerIndex];
		if ( player != nullptr )
		{
			player->Render();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::RenderHUD() const
{
	if ( m_currentRenderingPlayer == nullptr )
	{
		return;
	}

	m_screenCamera->SetViewport( m_currentRenderingPlayer->m_playerWorldCamera->GetViewport() );

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

	Player* player = m_currentRenderingPlayer;
	Actor* playerActor = player->GetActor();

	if ( playerActor != nullptr && playerActor->m_isDead )
	{
		std::vector<Vertex> overlayVerts;
		AddVertsForAABB2D(
			overlayVerts,
			AABB2( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) ),
			Rgba8( 20, 20, 20, 150 ) );

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
	transform.Append( m_currentRenderingPlayer->m_playerWorldCamera->GetWorldToCameraTransform() );
	transform.Append( m_currentRenderingPlayer->m_playerWorldCamera->GetCameraToRenderTransform() );
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
void Game::StartGameFromLobby()
{
	if ( m_currentGameState != GameState::LOBBY )
	{
		return;
	}

	if ( m_currentMap == nullptr )
	{
		return;
	}

	m_currentGameState = GameState::PLAYING;

	for ( int playerIndex = 0; playerIndex < ( int ) m_players.size(); ++playerIndex )
	{
		Player* player = m_players[playerIndex];
		if ( player == nullptr )
		{
			continue;
		}

		player->SetPlayerIndex( playerIndex );
		player->m_possessedActor = ActorHandle::INVALID;
		player->m_map = m_currentMap;

		m_currentMap->SpawnPlayer( player );
	}

	g_engine->m_audioSystem->SetNumListeners( ( int ) m_players.size() );
}


//-----------------------------------------------------------------------------------------------
void Game::Reset()
{
	DestroyMaps();

	for ( int playerIndex = 0; playerIndex < ( int ) m_players.size(); ++playerIndex )
	{
		Player* player = m_players[playerIndex];
		delete player;
	}
	m_players.clear();

	LoadMaps();
}


//-----------------------------------------------------------------------------------------------
Player* Game::GetPlayerFromActor( Actor const* actor ) const
{
	if ( actor == nullptr )
	{
		return nullptr;
	}

	for ( Player* player : m_players )
	{
		if ( player != nullptr && player->GetActor() == actor )
		{
			return player;
		}
	}

	return nullptr;
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


//-----------------------------------------------------------------------------------------------
void Game::UpdateAudioListeners()
{
	if ( g_engine == nullptr || g_engine->m_audioSystem == nullptr )
	{
		return;
	}

	for ( int playerIndex = 0; playerIndex < ( int ) m_players.size(); ++playerIndex )
	{
		Player* player = m_players[playerIndex];
		if ( player == nullptr || player->m_playerWorldCamera == nullptr )
		{
			continue;
		}

		g_engine->m_audioSystem->UpdateListener(
			playerIndex,
			player->m_playerWorldCamera->GetPosition(),
			player->m_playerWorldCamera->GetForwardDir(),
			Vec3( 0.f, 0.f, 1.f ) );
	}
}