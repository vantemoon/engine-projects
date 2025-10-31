#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Time.hpp"
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
	engineConfig.m_windowConfig.m_windowTitle = "Math Visual Tests";

	g_engine = new Engine( engineConfig );
	g_engine->Startup();

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
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
	{
		SetIsQuitting();
	}
}


//-----------------------------------------------------------------------------------------------
void App::UpdateFromController()
{
	XboxController const& controller = g_engine->m_inputSystem->GetController( 0 );

	if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
	{
		SetIsQuitting();
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
	UpdateFromKeyboard();

	float timeScale = 1.f;

	m_game->Update( deltaSeconds * timeScale );
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