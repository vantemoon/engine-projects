#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Engine.hpp"
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
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//----------------------------------------------------------------------------------------------
Game* g_game = nullptr;


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	g_game = this;

	m_currentMap = new Map( IntVec2( 32, 64 ) );

	m_worldCamera = new Camera();
	m_screenCamera = new Camera();
	m_debugCamera = new Camera();

	m_numTilesInViewVertically = NUM_TILES_VISIBLE_VERTICALLY;
	m_numTilesInViewHorizontally = NUM_TILES_VISIBLE_HORIZONTALLY;

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	m_attractMusicID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/AttractMusic.mp3" );
	m_gameplayMusicID = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/GameplayMusic.mp3" );

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
		UpdateFromKeyboard();
		UpdateFromController();
		return;
	};

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

		m_debugCamera->SetOrthoView(Vec2( 0.f, 0.f ), Vec2( ( float ) m_numTilesInViewHorizontally * TILE_SIZE, ( float ) m_numTilesInViewVertically * TILE_SIZE ) );
	}
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

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
			m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
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
void Game::UpdateFromKeyboard()
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		// Start the game
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
		{
			Reset();
			m_currentGameState = GameState::PLAYING;
			UpdateMusic();
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
		if ( m_currentGameState == GameState::PAUSED && g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
			UpdateMusic();
		}

		// Pause and resume the game
		if ( m_currentGameState == GameState::PLAYING && ( g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) || 
														   g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) ) )
		{
			m_currentGameState = GameState::PAUSED;
			UpdateMusic();
		}

		else if ( m_currentGameState == GameState::PAUSED && g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
		{
			m_currentGameState = GameState::PLAYING;
			UpdateMusic();
		}

		// Toggle debug features
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F1 ) )
		{
			m_isDebugOn = !m_isDebugOn;
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
				m_numTilesInViewVertically = NUM_TILES_VISIBLE_VERTICALLY;
				m_numTilesInViewHorizontally = NUM_TILES_VISIBLE_HORIZONTALLY;
			}
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
			UpdateMusic();
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
		if ( m_currentGameState == GameState::PAUSED && controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
			UpdateMusic();
		}

		// Pause and resume the game
		if ( m_currentGameState == GameState::PLAYING && ( controller.WasButtonJustPressed( XBOX_BUTTON_START ) ||
														   controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) ) )
		{
			m_currentGameState = GameState::PAUSED;
			UpdateMusic();
		}
		else if ( m_currentGameState == GameState::PAUSED && controller.WasButtonJustPressed( XBOX_BUTTON_START ) )
		{
			m_currentGameState = GameState::PLAYING;
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
		return;
	}

	if ( !m_isDebugCameraActive )
	{
		m_currentMap->UpdateWorldCameraView();
	}

	Camera* cameraToUse = m_isDebugCameraActive ? m_debugCamera : m_worldCamera;
	g_engine->m_renderer->BeginCamera( *cameraToUse );

	m_currentMap->Render();

	if ( m_isDebugOn )
	{
		m_currentMap->DebugRender();
	}

	if ( m_currentGameState == GameState::PAUSED )
	{
		RenderPauseScreenOverlay();
	};
	RenderHUD();

	g_engine->m_renderer->EndCamera( *cameraToUse );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	// Colors
	const Rgba8 brightYellow = Rgba8( 253, 239, 3 );
	const Rgba8 brightCyan = Rgba8( 87, 231, 239 );
	const Rgba8 dimCyan = Rgba8( 87, 231, 239, 80 );
	const Rgba8 dimYellow = Rgba8( 253, 239, 3, 80 );
	const Rgba8 neonPink = Rgba8( 248, 21, 98 );

	float flicker = 0.9f + 0.1f * SinDegrees( ( float ) GetCurrentTimeSeconds() * 720.f );
	Rgba8 flickerCol = Rgba8(
		( unsigned char ) ( brightCyan.r * flicker ),
		( unsigned char ) ( brightCyan.g * flicker ),
		( unsigned char ) ( brightCyan.b * flicker )
	);

	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	const float currentTime = ( float ) GetCurrentTimeSeconds();
	const float centerX = SCREEN_SIZE_X * 0.5f;
	const float centerY = SCREEN_SIZE_Y * 0.5f;

	// Play triangle
	float triBase = SCREEN_SIZE_X * 0.05f;
	float triPulse = 1.f + 0.06f * SinDegrees( currentTime * 180.f );
	float triSize = triBase * triPulse;

	Vertex triangleVertexArray[3];
	triangleVertexArray[0].m_position = Vec3( centerX - triSize, centerY - triSize, 0.f );
	triangleVertexArray[1].m_position = Vec3( centerX - triSize, centerY + triSize, 0.f );
	triangleVertexArray[2].m_position = Vec3( centerX + triSize, centerY, 0.f );

	float triAlpha = 0.55f + 0.45f * SinDegrees( currentTime * 180.f );
	Rgba8 triangleColor = Rgba8( 0, 153, 0, ( unsigned char ) ( triAlpha * 255.f ) );
	triangleVertexArray[0].m_color = triangleColor;
	triangleVertexArray[1].m_color = triangleColor;
	triangleVertexArray[2].m_color = triangleColor;

	// Text
	std::vector<Vertex> verts;
	AddVertsForTextTriangles2D( verts, "LIBRA", Vec2( 10.f, SCREEN_SIZE_Y - 30.f ), 24.f, Rgba8( 255, 255, 255 ) );

	// Ring
	std::vector<Vertex> ringVerts;
	AddVertsForRing2D( ringVerts, Vec2( SCREEN_CENTER_X, SCREEN_CENTER_Y ), 350.f, 20.f, Rgba8( 255, 0, 255 ), 64 );

	// Texture
	Texture* testTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	std::vector<Vertex> testTextureVerts;
	AABB2 texturedAABB2( 300.f, 100.f, 800.f, 600.f );
	AddVertsForAABB2D( testTextureVerts, texturedAABB2, Rgba8( 255, 255, 255, 255 ) );

	// Render 
	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( ringVerts );
	g_engine->m_renderer->BindTexture( testTexture );
	g_engine->m_renderer->DrawVertexArray( testTextureVerts );
	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( 3, triangleVertexArray );
	g_engine->m_renderer->DrawVertexArray( ( int ) verts.size(), verts.data() );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderHUD() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	// #ToDo: Render HUD elements (health, score, etc.)

	if ( m_currentGameState == GameState::PLAYING )
	{
		std::vector<Vertex> verts;
		AddVertsForTextTriangles2D( verts, "GAME MODE", Vec2( 10.f, SCREEN_SIZE_Y - 30.f ), 24.f, Rgba8( 255, 255, 255 ) );
		g_engine->m_renderer->DrawVertexArray( ( int ) verts.size(), verts.data() );
	}
	else if ( m_currentGameState == GameState::PAUSED )
	{
		std::vector<Vertex> verts;
		AddVertsForTextTriangles2D( verts, "PAUSED", Vec2( 10.f, SCREEN_SIZE_Y - 30.f ), 24.f, Rgba8( 255, 255, 255 ) );
		g_engine->m_renderer->DrawVertexArray( ( int ) verts.size(), verts.data() );
	}

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderPauseScreenOverlay() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	std::vector<Vertex> quadVerts;
	AABB2 fullScreenAABB2( 0.f, 0.f, SCREEN_SIZE_X, SCREEN_SIZE_Y );
	AddVertsForAABB2D( quadVerts, fullScreenAABB2, Rgba8( 0, 0, 0, 100 ) );
	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( quadVerts );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::KillAllEnemies()
{
	// #ToDo: Kill all enemy entities in the game world
}


//-----------------------------------------------------------------------------------------------
void Game::Reset()
{
	// #ToDo: Delete all entities and reset game state
	m_isScreenShaking = false;
	m_isDebugOn = false;
}