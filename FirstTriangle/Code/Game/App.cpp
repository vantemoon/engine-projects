#include "Game/App.hpp"
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

	// Set up window config
	engineConfig.m_windowConfig.m_clientAspect = 2.0f;
	engineConfig.m_windowConfig.m_windowTitle = "First Triangle";

	// Set up renderer config
	engineConfig.m_rendererConfig.m_isEnabled = false;

	g_engine = new Engine( engineConfig );
	g_engine->Startup();

	m_lastFrameStartTime = GetCurrentTimeSeconds();

	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "Quit", Command_Quit );
}


//-----------------------------------------------------------------------------------------------
App::~App()
{
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
}


//-----------------------------------------------------------------------------------------------
void App::HardReset()
{
}


//-----------------------------------------------------------------------------------------------
void App::Update( [[maybe_unused]] float deltaSeconds )
{
	UpdateFromKeyboard();
}


//-----------------------------------------------------------------------------------------------
void App::Render() const
{
	if ( g_engine->m_renderer == nullptr )
	{
		return;
	}
	g_engine->m_renderer->ClearScreen( Rgba8( 0, 0, 0 ) );
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