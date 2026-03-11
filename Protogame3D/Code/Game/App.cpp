#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DebugRender.hpp"
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
	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	m_systemClock = new Clock();

	EngineConfig engineConfig;
	engineConfig.m_windowConfig.m_clientAspect = 2.0f;
	engineConfig.m_windowConfig.m_windowTitle = "Protogame3D";
	engineConfig.m_devConsoleConfig.m_camera = m_devConsoleCamera;

	g_engine = new Engine( engineConfig );

	DebugRenderConfig debugRenderConfig;
	debugRenderConfig.m_fontPath = "Data/Fonts/";
	debugRenderConfig.m_fontName = "SquirrelFixedFont";
	DebugRenderSystemStartup( debugRenderConfig );

	m_game = new Game();

	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "Quit", Command_Quit );
}


//-----------------------------------------------------------------------------------------------
App::~App()
{
	DebugRenderSystemShutdown();

	delete m_devConsoleCamera;
	m_devConsoleCamera = nullptr;

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
	DebugRenderBeginFrame();

	Update();
	Render(); // Draw the current state of the game

	DebugRenderEndFrame();
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

	m_devConsoleCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
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
	RenderDevConsole();
}


//-----------------------------------------------------------------------------------------------
void App::RenderDevConsole() const
{
	AABB2 devConsoleBounds = AABB2( 0.f, 0.f, SCREEN_SIZE_X, SCREEN_SIZE_Y );
	g_engine->m_devConsole->Render( devConsoleBounds );
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