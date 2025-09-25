#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/App.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"


//-----------------------------------------------------------------------------------------------
App::App()
{
	g_engine = new Engine();
	m_game = new Game();

	m_lastFrameStartTime = GetCurrentTimeSeconds();
}


//-----------------------------------------------------------------------------------------------
App::~App()
{
	delete m_game;
	m_game = nullptr;

	delete g_engine;
	g_engine = nullptr;
}


//-----------------------------------------------------------------------------------------------
void App::RunFrame()
{
	g_engine->BeginFrame();

	double currentTime = GetCurrentTimeSeconds();
	float deltaSeconds = static_cast<float> ( currentTime - m_lastFrameStartTime );
	deltaSeconds = GetClamped( deltaSeconds, 0.f, 0.1f );

	m_lastFrameStartTime = currentTime;

	Update( deltaSeconds );
	Render(); // Draw the current state of the game
	g_engine->EndFrame(); // Allow engine subsystems to do post-frame stuff
}


//-----------------------------------------------------------------------------------------------
void CheckKeyboardInput()
{
	if ( !g_app->m_game->m_isAttractMode )
	{
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
		{
			g_app->m_isPaused = !g_app->m_isPaused;
		}

		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'O' ) )
		{
			g_app->m_isPaused = false;
			g_app->m_pauseAfterNextUpdate = true;
		}

		if ( g_engine->m_inputSystem->IsKeyDown( 'T' ) )
		{
			g_app->m_isSlowMo = true;
		}
		else
		{
			g_app->m_isSlowMo = false;
		}

		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) )
		{
			g_app->m_game->SpawnBulletFromPlayerShip();
		}

		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'I' ) )
		{
			g_app->m_game->SpawnRandomAsteroids( 1 );
		}

		if ( g_engine->m_inputSystem->IsKeyDown( 'S' ) && !g_engine->m_inputSystem->IsKeyDown( 'F' ) )
		{
			g_app->m_game->m_playerShip->m_isTurningLeft = true;
		}
		else if ( g_engine->m_inputSystem->IsKeyDown( 'F' ) && !g_engine->m_inputSystem->IsKeyDown( 'S' ) )
		{
			g_app->m_game->m_playerShip->m_isTurningRight = true;
		}
		else
		{
			g_app->m_game->m_playerShip->m_isTurningLeft = false;
			g_app->m_game->m_playerShip->m_isTurningRight = false;
		}

		if ( g_engine->m_inputSystem->IsKeyDown( 'E' ) )
		{
			g_app->m_game->m_playerShip->m_isAccelerating = true;
		}
		else
		{
			g_app->m_game->m_playerShip->m_isAccelerating = false;
		}

		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'N' ) && g_app->m_game->m_playerShip->m_isDead )
		{
			if ( g_app->m_game->m_playerSpareLives > 0 )
			{
				g_app->m_game->m_playerShip->Respawn();
				-- g_app->m_game->m_playerSpareLives;
			}
		}

		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F1 ) )
		{
			g_app->m_debugDraw = !g_app->m_debugDraw;
		}

		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F8 ) )
		{
			g_app->HardReset();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void App::HardReset()
{
	if ( m_game != nullptr )
	{
		delete m_game;
		m_game = nullptr;
	}

	m_game = new Game();
}


//-----------------------------------------------------------------------------------------------
void App::Update( float deltaSeconds )
{
	CheckKeyboardInput();

	if ( !m_isPaused )
	{
		float timeScale = 1.f;

		if( m_isSlowMo )
			timeScale = 0.1f;

		m_game->Update( deltaSeconds * timeScale );
	}

	if( m_pauseAfterNextUpdate )
	{
		m_isPaused = true;
		m_pauseAfterNextUpdate = false;
	}
}


//-----------------------------------------------------------------------------------------------
void App::Render() const
{
	g_engine->m_renderer->ClearScreen( Rgba8( 0, 0, 0 ) );
	m_game->Render();
}


//-----------------------------------------------------------------------------------------------
void App::SetIsQuitting()
{
	m_isQuitting = true;
}


//-----------------------------------------------------------------------------------------------
bool App::IsQuitting() const
{
	return m_isQuitting;
}