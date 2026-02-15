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
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
App* g_app = nullptr;


//-----------------------------------------------------------------------------------------------
App::App()
{
	// Do nothing
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
void App::Startup()
{
	LoadGameConfigFromFile( "Data/GameConfig.xml" );

	EngineConfig engineConfig;
	engineConfig.m_windowConfig.m_clientAspect = g_gameConfigBlackboard.GetValue( "windowAspect", 2.0f );
	engineConfig.m_windowConfig.m_windowTitle = g_gameConfigBlackboard.GetValue( "windowTitle", "Libra" );

	g_engine = new Engine( engineConfig );

	m_game = new Game();

	m_lastFrameStartTime = GetCurrentTimeSeconds();

	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "Quit", Command_Quit );
}


//-----------------------------------------------------------------------------------------------
void App::LoadGameConfigFromFile( char const* filepath )
{
	tinyxml2::XMLDocument xmlDocument;
	XmlResult loadResult = xmlDocument.LoadFile( filepath );

	if ( loadResult != tinyxml2::XML_SUCCESS )
	{
		GUARANTEE_OR_DIE( false, Stringf( "Failed to load game config XML file '%s'", filepath ) );
		return;
	}

	XmlElement* rootElement = xmlDocument.RootElement();
	if ( rootElement == nullptr )
	{
		GUARANTEE_OR_DIE( false, Stringf( "Failed to find root element in game config XML file '%s'", filepath ) );
		return;
	}

	g_gameConfigBlackboard.PopulateFromXmlElementAttributes( *rootElement );
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
		// Stop any playing music
		if ( m_game->m_isAttractMusicPlaying )
		{
			g_engine->m_audioSystem->StopSound( m_game->m_attractMusicPlaybackID );
			m_game->m_isAttractMusicPlaying = false;
		}
		if ( m_game->m_isGameplayMusicPlaying )
		{
			g_engine->m_audioSystem->StopSound( m_game->m_gameplayMusicPlaybackID );
			m_game->m_isGameplayMusicPlaying = false;
		}

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