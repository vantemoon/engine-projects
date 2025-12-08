#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"


//----------------------------------------------------------------------------------------------
Game* g_game = nullptr;


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	LoadTextures();
	LoadSounds();

	g_game = this;

	static bool isInitialized = false;
	if ( !isInitialized )
	{
		MapDefinition::InitializeMapDefinitions();
		isInitialized = true;
	}

	Map* map1 = new Map( MapDefinition::s_definitions["Level1"], 0 );
	Map* map2 = new Map( MapDefinition::s_definitions["Level2"], 1 );
	Map* map3 = new Map( MapDefinition::s_definitions["Level3"], 2 );
	Map* map4 = new Map( MapDefinition::s_definitions["Level4"], 3 );
	Map* map5 = new Map( MapDefinition::s_definitions["Level5"], 4 );
	Map* map6 = new Map( MapDefinition::s_definitions["Level6"], 5 );
	m_currentMap = map1;
	m_maps.push_back( map1 );
	m_maps.push_back( map2 );
	m_maps.push_back( map3 );
	m_maps.push_back( map4 );
	m_maps.push_back( map5 );
	m_maps.push_back( map6 );

	IntVec2 playerStartTileCoords = IntVec2( 1, 1 );
	Vec2 playerStartPos = m_currentMap->GetWorldPositionForTileCoords( playerStartTileCoords );
	Player* player = static_cast<Player*>( m_currentMap->SpawnNewEntity( ENTITY_TYPE_GOOD_PLAYER, playerStartPos, 45.f ) );
	g_game->m_player = player;
	
	m_worldCamera = new Camera();
	m_screenCamera = new Camera();
	m_debugCamera = new Camera();

	m_numTilesInViewVertically = g_gameConfigBlackboard.GetValue( "numTilesVisibleVertically", 8 );
	m_numTilesInViewHorizontally = g_gameConfigBlackboard.GetValue( "numTilesVisibleHorizontally", 16 );

	float worldWidth = g_gameConfigBlackboard.GetValue( "worldWidth", 200.f );
	float worldHeight = g_gameConfigBlackboard.GetValue( "worldHeight", 100.f );

	float screenWidth = g_gameConfigBlackboard.GetValue( "windowWidth", 1600.f );
	float screenHeight = g_gameConfigBlackboard.GetValue( "windowHeight", 800.f );

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( worldWidth, worldHeight ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( screenWidth, screenHeight ) );

	m_isAttractMusicPlaying = false;
	m_isGameplayMusicPlaying = false;

	m_currentGameState = ATTRACT_MODE;
	UpdateMusic();
}


//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	delete m_player;
	m_player = nullptr;

	delete m_worldCamera;
	m_worldCamera = nullptr;

	delete m_screenCamera;
	m_screenCamera = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Game::Update( float deltaSeconds )
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		UpdateAttractMode( deltaSeconds );
		return;
	};

	if ( m_currentGameState == GameState::PAUSED )
	{
		UpdatePausedMode( deltaSeconds );
		return;
	};

	if ( m_currentGameState == GameState::VICTORY )
	{
		UpdateVicoryMode( deltaSeconds );
		return;
	}

	if ( m_currentGameState == GameState::GAME_OVER )
	{
		UpdateGameOverMode( deltaSeconds );
		return;
	}

	UpdateFromKeyboard();
	UpdateFromController();

	if ( m_isSlowMo )
	{
		deltaSeconds *= 0.1f;
	}
	else if ( m_isFastMo )
	{
		deltaSeconds *= 4.f;
	}
	m_currentMap->Update( deltaSeconds );

	static float s_timeSincePlayerDeath = 0.f;
	static bool s_playerDeathHandled = false;


	if ( m_player && m_player->m_isDead ) 
	{
		if ( !s_playerDeathHandled ) 
		{
			s_timeSincePlayerDeath = 0.f;
			s_playerDeathHandled = true;
		}
		else 
		{
			s_timeSincePlayerDeath += deltaSeconds;
			if ( s_timeSincePlayerDeath >= g_gameConfigBlackboard.GetValue( "deathDelay", 3.0f ) )
			{
				g_engine->m_audioSystem->StartSound( m_gameOverSoundID );
				m_currentGameState = GameState::GAME_OVER;
				s_playerDeathHandled = false;
			}
		}
	}
	else 
	{
		s_playerDeathHandled = false;
		s_timeSincePlayerDeath = 0.f;
	}

	if ( m_isDebugCameraActive )
	{
		IntVec2 mapDims = m_currentMap->m_dimensions;
		int mapWidthInTiles = mapDims.x;
		int mapHeightInTiles = mapDims.y;

		if ( mapWidthInTiles > mapHeightInTiles )
		{
			m_numTilesInViewHorizontally = mapWidthInTiles;
			float aspect = g_engine->m_window->m_config.m_clientAspect;
			m_numTilesInViewVertically = static_cast<int>( ( float ) m_numTilesInViewHorizontally / aspect );
		}
		else
		{
			m_numTilesInViewVertically = mapHeightInTiles;
			float aspect = g_engine->m_window->m_config.m_clientAspect;
			m_numTilesInViewHorizontally = static_cast<int>( ( float ) m_numTilesInViewVertically * aspect );
		}

		float tileSize = g_gameConfigBlackboard.GetValue( "tileSize", 12.5f );
		m_debugCamera->SetOrthoView(Vec2( 0.f, 0.f ), Vec2( ( float ) m_numTilesInViewHorizontally * tileSize, ( float ) m_numTilesInViewVertically * tileSize ) );
	}

	float worldWidth = g_gameConfigBlackboard.GetValue( "worldWidth", 200.f );
	float worldHeight = g_gameConfigBlackboard.GetValue( "worldHeight", 100.f );

	float screenWidth = g_gameConfigBlackboard.GetValue( "windowWidth", 1600.f );
	float screenHeight = g_gameConfigBlackboard.GetValue( "windowHeight", 800.f );

	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( screenWidth, screenHeight ) );

	if ( m_isScreenShaking )
	{
		float shakeElapsedTime = ( float ) GetCurrentTimeSeconds() - m_screenShakeStartTime;
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
			m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( worldWidth, worldHeight ) );
		}
	}

	if ( m_isPausedAfterNextUpdate )
	{
		m_currentGameState = GameState::PAUSED;
		m_isPausedAfterNextUpdate = false;
	}
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateMusic()
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		if ( m_isGameplayMusicPlaying)
		{
			g_engine->m_audioSystem->StopSound( m_gameplayMusicPlaybackID );
			m_isGameplayMusicPlaying = false;
		}
		if ( !m_isAttractMusicPlaying )
		{
			m_attractMusicPlaybackID = g_engine->m_audioSystem->StartSound( m_attractMusicID, true );
			m_isAttractMusicPlaying = true;
		}
	}
	else if ( m_currentGameState == GameState::PLAYING )
	{
		if ( m_isAttractMusicPlaying)
		{
			g_engine->m_audioSystem->StopSound( m_attractMusicPlaybackID );
			m_isAttractMusicPlaying = false;
		}
		if ( !m_isGameplayMusicPlaying )
		{
			m_gameplayMusicPlaybackID = g_engine->m_audioSystem->StartSound( m_gameplayMusicID, true );
			m_isGameplayMusicPlaying = true;
		}
		if ( m_isGameplayMusicPlaying )
		{
			if ( m_isSlowMo )
			{
				g_engine->m_audioSystem->SetSoundPlaybackSpeed( m_gameplayMusicPlaybackID, 0.5f );
			}
			else if ( m_isFastMo )
			{
				g_engine->m_audioSystem->SetSoundPlaybackSpeed( m_gameplayMusicPlaybackID, 1.5f );
			}
			else
			{
				g_engine->m_audioSystem->SetSoundPlaybackSpeed( m_gameplayMusicPlaybackID, 1.f );
			}
		}
	}
	else if ( m_currentGameState == GameState::PAUSED )
	{
		if ( m_isGameplayMusicPlaying )
		{
			g_engine->m_audioSystem->SetSoundPlaybackSpeed( m_gameplayMusicPlaybackID, 0.f );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::ScreenShake( float intensity )
{
	if ( m_worldCamera == nullptr || intensity <= 0.f )
		return;

	RandomNumberGenerator rng;
	float offsetX = rng.RollRandomFloatInRange( -intensity, intensity );
	float offsetY = rng.RollRandomFloatInRange( -intensity, intensity );
	m_worldCamera->Translate2D( Vec2( offsetX, offsetY ) );
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateAttractMode( [[maybe_unused]] float deltaSeconds )
{
	UpdateFromKeyboard();
	UpdateFromController();
}


//-----------------------------------------------------------------------------------------------
void Game::UpdatePausedMode( [[maybe_unused]] float deltaSeconds )
{
	UpdateFromKeyboard();
	UpdateFromController();
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateVicoryMode( [[maybe_unused]] float deltaSeconds )
{
	UpdateFromKeyboard();
	UpdateFromController();
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateGameOverMode( [[maybe_unused]] float deltaSeconds )
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
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
		{
			Reset();
			m_currentGameState = GameState::PLAYING;
			g_engine->m_audioSystem->StartSound( m_startGameSoundID, false );
			UpdateMusic();
		};

		// Quit the game
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
		{
			g_app->SetIsQuitting();
		}
	}
	else if ( m_currentGameState == GameState::VICTORY )
	{
		// Return to attract mode
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) || g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) || 
			g_engine->m_inputSystem->WasKeyJustPressed( 'N' ) )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
			g_engine->m_audioSystem->StartSound( m_startGameSoundID, false );
			UpdateMusic();
		}
	}
	else if ( m_currentGameState == GameState::GAME_OVER )
	{
		// Respawn the player
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'N' ) )
		{
			m_player->Respawn( m_player->m_position );
			m_currentGameState = GameState::PLAYING;
			UpdateMusic();
		}

		// Return to attract mode
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
			g_engine->m_audioSystem->StartSound( m_startGameSoundID, false );
			UpdateMusic();
		}
	}
	else
	{
		// Return to attract mode
		if ( m_currentGameState == GameState::PAUSED && g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
			g_engine->m_audioSystem->StartSound( m_startGameSoundID, false );
			UpdateMusic();
		}

		// Pause and resume the game
		if ( m_currentGameState == GameState::PLAYING && ( g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) || 
														   g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) ) )
		{
			m_currentGameState = GameState::PAUSED;
			g_engine->m_audioSystem->StartSound( m_pauseSoundID, false );
			UpdateMusic();
		}
		else if ( m_currentGameState == GameState::PAUSED && g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
		{
			m_currentGameState = GameState::PLAYING;
			g_engine->m_audioSystem->StartSound( m_unpauseSoundID, false );
			UpdateMusic();
		}

		// Toggle debug features
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F1 ) )
		{
			m_isDebugOn = !m_isDebugOn;
		}

		// Toggle map render mode
		if ( m_isDebugOn && g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F6 ) )
		{
			m_mapRenderMode = ( m_mapRenderMode + 1 ) % 4;
		}

		// Toggle slow motion
		if ( g_engine->m_inputSystem->IsKeyDown( 'T' ) && !g_engine->m_inputSystem->IsKeyDown( 'Y' ) )
		{
			m_isSlowMo = true;
			UpdateMusic();
		}
		else
		{
			m_isSlowMo = false;
			UpdateMusic();
		}

		// Toggle fast motion
		if ( g_engine->m_inputSystem->IsKeyDown( 'Y' ) && !g_engine->m_inputSystem->IsKeyDown( 'T' ) )
		{
			m_isFastMo = true;
			UpdateMusic();
		}
		else
		{
			m_isFastMo = false;
			UpdateMusic();
		}

		// Toggle player invincibility (for debugging)
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F2 ) )
		{
			m_player->m_isInvincible = !m_player->m_isInvincible;
		}

		// Toggle no-clip (for debugging)
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F3 ) )
		{
			m_player->m_noClip = !m_player->m_noClip;
		}

		// Toggle debug camera (for debugging)
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F4 ) )
		{
			m_isDebugCameraActive = !m_isDebugCameraActive;

			if ( !m_isDebugCameraActive )
			{
				m_numTilesInViewVertically = g_gameConfigBlackboard.GetValue( "numTilesVisibleVertically", 8 );
				m_numTilesInViewHorizontally = g_gameConfigBlackboard.GetValue( "numTilesVisibleHorizontally", 16 );
			}
		}

		// Load next map (for debugging)
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F9 ) )
		{
			LoadNextMap();
		}
	}

	// Toggle developer console
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_TILDE ) )
	{
		g_engine->m_devConsole->ToggleMode( DevConsoleMode::OPEN_FULL );
	}

	// If the console is open, press enter to print a new line
	if ( g_engine->m_devConsole->GetMode() == DevConsoleMode::OPEN_FULL )
	{
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ENTER ) )
		{
			// g_engine->m_devConsole->AddLine( g_engine->m_devConsole->INFO_MINOR, "" );
			g_engine->m_devConsole->Execute( g_gameConfigBlackboard.GetValue( "testCommand", "" ) );
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
			g_engine->m_audioSystem->StartSound( m_startGameSoundID, false );
			UpdateMusic();
		};
		// Quit the game
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
		{
			g_app->SetIsQuitting();
		}
	}
	else if ( m_currentGameState == GameState::VICTORY )
	{
		// Return to attract mode
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) || controller.WasButtonJustPressed( XBOX_BUTTON_A ) ||
			controller.WasButtonJustPressed( XBOX_BUTTON_START ) )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
			g_engine->m_audioSystem->StartSound( m_startGameSoundID, false );
			UpdateMusic();
		}
	}
	else if ( m_currentGameState == GameState::GAME_OVER )
	{
		// Respawn the player
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_START ) )
		{
			m_player->Respawn( m_player->m_position );
			m_currentGameState = GameState::PLAYING;
			UpdateMusic();
		}

		// Return to attract mode
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
			g_engine->m_audioSystem->StartSound( m_startGameSoundID, false );
			UpdateMusic();
		}
	}
	else
	{
		// Return to attract mode
		if ( m_currentGameState == GameState::PAUSED && controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
			g_engine->m_audioSystem->StartSound( m_startGameSoundID, false );
			UpdateMusic();
		}

		// Pause and resume the game
		if ( m_currentGameState == GameState::PLAYING && ( controller.WasButtonJustPressed( XBOX_BUTTON_START ) ||
														   controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) ) )
		{
			m_currentGameState = GameState::PAUSED;
			g_engine->m_audioSystem->StartSound( m_pauseSoundID, false );
			UpdateMusic();
		}
		else if ( m_currentGameState == GameState::PAUSED && controller.WasButtonJustPressed( XBOX_BUTTON_START ) )
		{
			m_currentGameState = GameState::PLAYING;
			g_engine->m_audioSystem->StartSound( m_unpauseSoundID, false );
			UpdateMusic();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		RenderAttractMode();
		RenderDevConsole();
		return;
	}

	if ( m_currentGameState == GameState::VICTORY )
	{
		RenderVictoryMode();
		RenderDevConsole();
		return;
	}

	if ( !m_isDebugCameraActive )
	{
		m_currentMap->UpdateWorldCameraView();
	}

	Camera* cameraToUse;
	if ( m_isDebugCameraActive )
	{
		cameraToUse = m_debugCamera;
	}
	else
	{
		cameraToUse = m_worldCamera;
	}

	g_engine->m_renderer->BeginCamera( *cameraToUse );

	m_currentMap->Render();

	if ( m_currentGameState == GameState::PAUSED )
	{
		RenderPausedMode();
	};

	if ( m_currentGameState == GameState::GAME_OVER )
	{
		RenderGameOverMode();
	};

	// RenderHUD();

	RenderDevConsole();

	g_engine->m_renderer->EndCamera( *cameraToUse );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	float screenWidth = g_gameConfigBlackboard.GetValue( "windowWidth", 1600.f );
	float screenHeight = g_gameConfigBlackboard.GetValue( "windowHeight", 800.f );

	// Texture
	AABB2 fullScreenAABB2( 0.f, 0.f, screenWidth, screenHeight );
	std::vector<Vertex> backgroundVerts;
	AddVertsForAABB2D( backgroundVerts, fullScreenAABB2, Rgba8::WHITE );

	// Ring
	float time = ( float ) GetCurrentTimeSeconds();
	float pulse = ( SinDegrees( time * 180.f ) * 0.5f ) + 0.5f; // oscillates 0 → 1

	float baseRadius = 150.f;
	float pulseRadiusRange = 40.f;
	float currentRadius = baseRadius + pulseRadiusRange * pulse;

	float baseThickness = 20.f;
	float pulseThicknessRange = 20.f;
	float currentThickness = baseThickness + pulseThicknessRange * pulse;

	std::vector<Vertex> ringVerts;
	AddVertsForRing2D( ringVerts, Vec2( screenWidth / 2, screenHeight / 2 ), currentRadius, currentThickness, Rgba8( 255, 165, 0 ), 64 );

	// Render 
	g_engine->m_renderer->BindTexture( m_attractModeBackgroundTexture );
	g_engine->m_renderer->DrawVertexArray( ( int ) backgroundVerts.size(), backgroundVerts.data() );
	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( ( int ) ringVerts.size(), ringVerts.data() );

	g_engine->m_renderer->BindTexture( nullptr );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderHUD() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	// float screenWidth = g_gameConfigBlackboard.GetValue( "windowWidth", 1600.f );
	float screenHeight = g_gameConfigBlackboard.GetValue( "windowHeight", 800.f );

	if ( m_currentGameState == GameState::PLAYING )
	{
		std::vector<Vertex> verts;
		AddVertsForTextTriangles2D( verts, "GAME MODE", Vec2( 10.f, screenHeight - 30.f ), 24.f, Rgba8( 255, 255, 255 ) );
		g_engine->m_renderer->DrawVertexArray( ( int ) verts.size(), verts.data() );
	}
	else if ( m_currentGameState == GameState::PAUSED )
	{
		std::vector<Vertex> verts;
		AddVertsForTextTriangles2D( verts, "PAUSED", Vec2( 10.f, screenHeight - 30.f ), 24.f, Rgba8( 255, 255, 255 ) );
		g_engine->m_renderer->DrawVertexArray( ( int ) verts.size(), verts.data() );
	}
	else if ( m_currentGameState == GameState::GAME_OVER )
	{
		std::vector<Vertex> verts;
		AddVertsForTextTriangles2D( verts, "GAME OVER", Vec2( 10.f, screenHeight - 30.f ), 24.f, Rgba8( 255, 0, 0 ) );
		g_engine->m_renderer->DrawVertexArray( ( int ) verts.size(), verts.data() );
	}

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderDevConsole() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	float screenWidth = g_gameConfigBlackboard.GetValue( "windowWidth", 1600.f );
	float screenHeight = g_gameConfigBlackboard.GetValue( "windowHeight", 800.f );
	AABB2 devConsoleBounds = AABB2( 0.f, 0.f, screenWidth, screenHeight );
	g_engine->m_devConsole->Render( devConsoleBounds );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderPausedMode() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	float screenWidth = g_gameConfigBlackboard.GetValue( "windowWidth", 1600.f );
	float screenHeight = g_gameConfigBlackboard.GetValue( "windowHeight", 800.f );

	std::vector<Vertex> quadVerts;
	AABB2 fullScreenAABB2( 0.f, 0.f, screenWidth, screenHeight );
	AddVertsForAABB2D( quadVerts, fullScreenAABB2, Rgba8( 0, 0, 0, 100 ) );
	g_engine->m_renderer->DrawVertexArray( quadVerts );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderVictoryMode() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	float screenWidth = g_gameConfigBlackboard.GetValue( "windowWidth", 1600.f );
	float screenHeight = g_gameConfigBlackboard.GetValue( "windowHeight", 800.f );

	std::vector<Vertex> verts;
	AABB2 fullScreenAABB2( 0.f, 0.f, screenWidth, screenHeight );
	AddVertsForAABB2D( verts, fullScreenAABB2, Rgba8( 255, 255, 255, 255 ) );
	g_engine->m_renderer->BindTexture( m_victoryScreenTexture );
	g_engine->m_renderer->DrawVertexArray( ( int ) verts.size(), verts.data() );
	g_engine->m_renderer->BindTexture( nullptr );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderGameOverMode() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	float screenWidth = g_gameConfigBlackboard.GetValue( "windowWidth", 1600.f );
	float screenHeight = g_gameConfigBlackboard.GetValue( "windowHeight", 800.f );
	
	std::vector<Vertex> verts;
	AABB2 fullScreenAABB2( 0.f, 0.f, screenWidth, screenHeight );
	AddVertsForAABB2D( verts, fullScreenAABB2, Rgba8( 255, 255, 255, 255 ) );
	g_engine->m_renderer->BindTexture( m_gameOverScreenTexture );
	g_engine->m_renderer->DrawVertexArray( ( int ) verts.size(), verts.data() );
	g_engine->m_renderer->BindTexture( nullptr );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::KillAllEnemies()
{
	// #ToDo: Kill all enemy entities in the game world
}


//-----------------------------------------------------------------------------------------------
void Game::LoadNextMap()
{
	int currentMapIndex = m_currentMap->m_index;
	if ( currentMapIndex == g_gameConfigBlackboard.GetValue( "numOfMaps", 6 ) - 1 )
	{
		g_engine->m_audioSystem->StartSound( m_victorySoundID, false );
		m_currentGameState = GameState::VICTORY;
		return;
	}
	g_engine->m_audioSystem->StartSound( m_nextLevelSoundID, false );
	int nextMapIndex = ( currentMapIndex + 1 ) % static_cast<int>( m_maps.size() );
	m_currentMap = m_maps[nextMapIndex];
	IntVec2 playerStartTileCoords = IntVec2( 1, 1 );
	m_player->Reset();
}


//-----------------------------------------------------------------------------------------------
void Game::Reset()
{
	// Reset current map
	delete m_currentMap;

	// Recreate all maps
	m_maps.clear();
	Map* map1 = new Map( MapDefinition::s_definitions["Level1"], 0 );
	Map* map2 = new Map( MapDefinition::s_definitions["Level2"], 1 );
	Map* map3 = new Map( MapDefinition::s_definitions["Level3"], 2 );
	Map* map4 = new Map( MapDefinition::s_definitions["Level4"], 3 );
	Map* map5 = new Map( MapDefinition::s_definitions["Level5"], 4 );
	Map* map6 = new Map( MapDefinition::s_definitions["Level6"], 5 );
	m_currentMap = map1;
	m_maps.push_back( map1 );
	m_maps.push_back( map2 );
	m_maps.push_back( map3 );
	m_maps.push_back( map4 );
	m_maps.push_back( map5 );
	m_maps.push_back( map6 );

	// Reset player
	m_player->Reset();

	m_isScreenShaking = false;
	m_isDebugOn = false;
}


//-----------------------------------------------------------------------------------------------
void Game::LoadTextures()
{
	// Tile SpriteSheet
	Texture* tileTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	m_tileSpriteSheet = new SpriteSheet( *tileTexture, IntVec2( 8, 8 ) );

	// Textures
	m_attractModeBackgroundTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/AttractScreen.png" );
	m_victoryScreenTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/VictoryScreen.jpg" );
	m_gameOverScreenTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/YouDiedScreen.png" );
	m_ariesTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/EnemyAries.png" );
	m_leoTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTank4.png" );
	m_scorpioBaseTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTurretBase.png" );
	m_scorpioTurretTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/EnemyCannon.png" );
	m_playerTankBaseTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankBase.png" );
	m_playerTankTurretTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankTop.png" );
	m_goodBulletTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/FriendlyBullet.png" );
	m_goodBoltTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/FriendlyBolt.png" );
	m_evilBulletTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/EnemyBullet.png" );
	m_evilBoltTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/EnemyBolt.png" );
	m_explosionTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/Explosion_5x5.png" );
}


//-----------------------------------------------------------------------------------------------
void Game::LoadSounds()
{
	// Background Music
	m_attractMusicID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/AttractMusic.mp3" );
	m_gameplayMusicID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/GameplayMusic.mp3" );

	// Sound Effects
	m_startGameSoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/Click.mp3" );
	m_pauseSoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/Pause.mp3" );
	m_unpauseSoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/Unpause.mp3" );
	m_playerShootSoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/PlayerShootNormal.ogg" );
	m_enemyShootSoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/EnemyShoot.wav" );
	m_playerHitSoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/PlayerHit.wav" );
	m_enemyHitSoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/EnemyHit.wav" );
	m_playerDeathSoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/EnemyDied.wav" ); // Using same sound as enemy death for now
	m_enemyDeathSoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/EnemyDied.wav" );
	m_nextLevelSoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/ExitMap.wav" );
	m_gameOverSoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/GameOver.mp3" );
	m_victorySoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/Victory.mp3" );
	m_discoverySoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/Discovery.wav" );
}