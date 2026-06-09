#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
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
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_worldCamera = new Camera();
	m_screenCamera = new Camera();

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	m_currentGameState = ATTRACT_MODE;

	m_gameClock = new Clock();
}


//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	delete m_worldCamera;
	m_worldCamera = nullptr;

	delete m_screenCamera;
	m_screenCamera = nullptr;
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
		return;
	};

	UpdateFromKeyboard();
	UpdateFromController();
	UpdateEntities();

	g_app->m_game->DeleteGarbageEntities();

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
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
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateEntities()
{
	// #ToDo: Update all entities in the game world
}


//-----------------------------------------------------------------------------------------------
void Game::DebugDraw() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	// #ToDo: Draw debug information about entities, collisions, etc.

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		RenderAttractMode();
		RenderDevConsole();
		return;
	};

	// Clear screen
	g_engine->m_renderer->ClearScreen( Rgba8::BLUE );

	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	RenderEntities();
	RenderHUD();

	if ( m_isDebugFeaturesOn )
	{
		DebugDraw();
	}

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	// Clear screen
	g_engine->m_renderer->ClearScreen( Rgba8(80, 80, 80) );

	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	// Render 

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderEntities() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	// #ToDo: Render all entities in the game world

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderHUD() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	// #ToDo: Render HUD elements (health, score, etc.)

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderDevConsole() const
{
	float screenWidth = g_gameConfigBlackboard.GetValue( "windowWidth", 1600.f );
	float screenHeight = g_gameConfigBlackboard.GetValue( "windowHeight", 800.f );
	AABB2 devConsoleBounds = AABB2( 0.f, 0.f, screenWidth, screenHeight );
	g_engine->m_devConsole->Render( devConsoleBounds );
}


//-----------------------------------------------------------------------------------------------
Vec3 Game::TransformWorldToScreen( Vec3 const& worldPosition ) const
{
	Vec2 worldBottomLeft = m_worldCamera->GetOrthoBottomLeft();
	Vec2 worldTopRight = m_worldCamera->GetOrthoTopRight();

	Vec2 screenBottomLeft = m_screenCamera->GetOrthoBottomLeft();
	Vec2 screenTopRight = m_screenCamera->GetOrthoTopRight();

	float worldWidth = worldTopRight.x - worldBottomLeft.x;
	float worldHeight = worldTopRight.y - worldBottomLeft.y;
	float normX = ( worldPosition.x - worldBottomLeft.x ) / worldWidth;
	float normY = ( worldPosition.y - worldBottomLeft.y ) / worldHeight;

	float screenWidth = screenTopRight.x - screenBottomLeft.x;
	float screenHeight = screenTopRight.y - screenBottomLeft.y;
	float screenX = screenBottomLeft.x + normX * screenWidth;
	float screenY = screenBottomLeft.y + normY * screenHeight;

	return Vec3( screenX, screenY, worldPosition.z );
}


//-----------------------------------------------------------------------------------------------
void Game::Reset()
{
	// #ToDo: Delete all entities and reset game state
	m_isScreenShaking = false;
	m_isDebugFeaturesOn = false;
}