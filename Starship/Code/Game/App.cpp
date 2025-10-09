#include "Game/App.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


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
void App::UpdateFromKeyboard()
{
	if ( !m_game->m_currentGameState == GameState::ATTRACT_MODE )
	{
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
		{
			m_isPaused = !m_isPaused;
		}

		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'O' ) )
		{
			m_isPaused = false;
			m_pauseAfterNextUpdate = true;
		}

		if ( g_engine->m_inputSystem->IsKeyDown( 'T' ) )
		{
			m_isSlowMo = true;
		}
		else
		{
			m_isSlowMo = false;
		}

		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F1 ) )
		{
			m_debugDraw = !m_debugDraw;
		}

		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F8 ) )
		{
			HardReset();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void App::UpdateFromController()
{
	// Currently no global controller actions
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
	UpdateFromKeyboard();

	float timeScale = 1.f;
	
	if( m_isPaused )
	{
		timeScale = 0.f;
	}
	else if( m_isSlowMo )
	{
		timeScale = 0.1f;
	}

	m_game->Update( deltaSeconds * timeScale );

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