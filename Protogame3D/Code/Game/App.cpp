#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
App* g_app = nullptr;


//-----------------------------------------------------------------------------------------------
App::App()
{
	EngineConfig engineConfig;
	engineConfig.m_windowConfig.m_clientAspect = 2.0f;
	engineConfig.m_windowConfig.m_windowTitle = "Protogame3D";

	g_engine = new Engine( engineConfig );

	m_game = new Game();

	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "Quit", Command_Quit );
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
void App::RunMainLoop()
{
	while ( !IsQuitting() )
	{
		RunFrame();
	}
}


//-----------------------------------------------------------------------------------------------
void App::RunFrame()
{
	Clock::TickSystemClock();

	g_engine->BeginFrame();

	Update();
	Render(); // Draw the current state of the game

	g_engine->EndFrame(); // Allow engine subsystems to do post-frame stuff
}


//-----------------------------------------------------------------------------------------------
void App::UpdateFromKeyboard()
{
	if ( !m_game->m_currentGameState == GameState::ATTRACT_MODE )
	{
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
void App::Update()
{
	UpdateMouse();
	UpdateFromKeyboard();

	m_game->Update();
}


//-----------------------------------------------------------------------------------------------
void App::UpdateMouse()
{
	bool isWindowFocused = g_engine->m_window->IsFocused();
	bool isDevConsoleOpen = g_engine->m_devConsole->GetMode() != DevConsoleMode::HIDDEN;
	bool isGameInAttractMode = m_game->m_currentGameState == GameState::ATTRACT_MODE;

	if ( !isWindowFocused || isDevConsoleOpen || isGameInAttractMode )
	{
		g_engine->m_inputSystem->SetCursorMode( CursorMode::POINTER );
	}
	else
	{
		g_engine->m_inputSystem->SetCursorMode( CursorMode::FPS );
	}

}


//-----------------------------------------------------------------------------------------------
void App::Render() const
{
	m_game->Render();
	m_game->RenderDevConsole();
}


//-----------------------------------------------------------------------------------------------
bool App::Command_Quit( EventArgs& args )
{
	UNUSED( args );
	if ( g_app && !g_app->m_isQuitting )
	{
		g_app->SetIsQuitting();
	}
	return false;
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