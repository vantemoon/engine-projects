#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"


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

	if ( !m_isPaused )
	{
		float deltaSeconds = 0.0167f; // Normal speed: ~60 FPS
		if ( m_isSlowMo )
			Update( deltaSeconds * 0.1f ); // Slow motion: ~1/10th speed
		else
			Update( deltaSeconds ); // Normal speed: ~60 FPS
	}
	else 
	{
		Update( 0.f ); // If paused, update with no time having passed
	}
	Render(); // Draw the current state of the game
	g_engine->EndFrame(); // Allow engine subsystems to do post-frame stuff
}


//-----------------------------------------------------------------------------------------------
void App::Update( float deltaSeconds )
{
	// Update the game world by the given time step (deltaSeconds)
	m_game->Update( deltaSeconds );

	if ( m_pauseAfterNextUpdate )
	{
		m_isPaused = true;
		m_pauseAfterNextUpdate = false;
	}

	// Clear out "wasKeyJustPressed" state for all keys
	for ( int keyIndex = 0; keyIndex < 256; ++keyIndex )
	{
		m_wasKeyJustPressed[keyIndex] = false;
	}
}


//-----------------------------------------------------------------------------------------------
void App::Render() const
{
	Camera gameCamera = *(m_game->m_gameCamera);
	g_engine->m_renderer->BeginCamera( gameCamera );
	g_engine->m_renderer->ClearScreen( Rgba8( 100, 50, 0, 255 ) );

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