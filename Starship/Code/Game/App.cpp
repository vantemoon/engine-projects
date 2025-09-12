#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
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
	// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
	g_engine->BeginFrame(); // Allow engine subsystems to do pre-frame stuff
	float deltaSeconds = 0.0167f;
	Update( deltaSeconds );
	Render(); // Draw the current state of the game
	g_engine->EndFrame(); // Allow engine subsystems to do post-frame stuff
}


//-----------------------------------------------------------------------------------------------
void CheckKeyboardInput()
{
	if ( g_app->IsKeyDown( 'Q' ) )
	{
		g_app->SetIsQuitting();
	}

	if ( g_app->WasKeyJustPressed( 'P' ) )
	{
		g_app->m_isPaused = !g_app->m_isPaused;
	}

	if ( g_app->WasKeyJustPressed( 'O' ) )
	{
		g_app->m_isPaused = false;
		g_app->m_pauseAfterNextUpdate = true;
	}

	if ( g_app->WasKeyJustPressed( 'T' ) )
	{
		g_app->m_isSlowMo = true;
	}
	else if ( g_app->WasKeyJustReleased( 'T' ) )
	{
		g_app->m_isSlowMo = false;
	}

	if ( g_app->WasKeyJustPressed( 32 ) ) // Space
	{
		g_app->m_game->SpawnBulletFromPlayerShip();
	}

	if ( g_app->WasKeyJustPressed( 'I' ) )
	{
		g_app->m_game->SpawnRandomAsteroid();
	}

	if ( g_app->WasKeyJustPressed( 'S' ) && !g_app->WasKeyJustPressed( 'F' ) )
	{
		g_app->m_game->m_playerShip->m_isTurningLeft = true;
	}

	if ( g_app->WasKeyJustReleased( 'S' ) )
	{
		g_app->m_game->m_playerShip->m_isTurningLeft = false;
	}

	if(g_app->WasKeyJustPressed( 'F' ) && !g_app->WasKeyJustPressed( 'S' ))
	{
		g_app->m_game->m_playerShip->m_isTurningRight = true;
	}

	if ( g_app->WasKeyJustReleased( 'F' ) )
	{
		g_app->m_game->m_playerShip->m_isTurningRight = false;
	}

	if ( g_app->WasKeyJustPressed( 'E' ) )
	{
		g_app->m_game->m_playerShip->m_isAccelerating = true;
	}

	if ( g_app->WasKeyJustReleased( 'E' ) )
	{
		g_app->m_game->m_playerShip->m_isAccelerating = false;
	}

	if ( g_app->WasKeyJustPressed( 'N' ) && g_app->m_game->m_playerShip->m_isDead )
	{
		g_app->m_game->m_playerShip->Respawn();
	}
}


//-----------------------------------------------------------------------------------------------
void App::Update( float deltaSeconds )
{
	CheckKeyboardInput();

	if ( !m_isPaused )
	{
		float timeScale = 1.f;

		if(m_isSlowMo)
			timeScale = 0.1f;

		m_game->Update( deltaSeconds * timeScale );
	}

	if(m_pauseAfterNextUpdate)
	{
		m_isPaused = true;
		m_pauseAfterNextUpdate = false;
	}

	for(int keyIndex = 0; keyIndex < 256; ++keyIndex)
	{
		m_wasKeyJustPressed[keyIndex] = m_isKeyDown[keyIndex];
	}
}


//-----------------------------------------------------------------------------------------------
void App::Render() const
{
	g_engine->m_renderer->ClearScreen( Rgba8( 0, 0, 0, 255 ) );
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


//-----------------------------------------------------------------------------------------------
void App::OnKeyDown( unsigned char keyCode )
{
	m_isKeyDown[keyCode] = true;
}


//-----------------------------------------------------------------------------------------------
void App::OnKeyUp( unsigned char keyCode )
{
	m_isKeyDown[keyCode] = false;
}


//-----------------------------------------------------------------------------------------------
bool App::IsKeyDown( unsigned char keyCode ) const
{
	return m_isKeyDown[keyCode];
}


//-----------------------------------------------------------------------------------------------
bool App::WasKeyJustPressed( unsigned char keyCode ) const
{
	return m_isKeyDown[keyCode] && !m_wasKeyJustPressed[keyCode];
}


//-----------------------------------------------------------------------------------------------
bool App::WasKeyJustReleased( unsigned char keyCode ) const
{
	return !m_isKeyDown[keyCode] && m_wasKeyJustPressed[keyCode];
}