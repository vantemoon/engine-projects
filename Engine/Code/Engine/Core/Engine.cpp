#include "Engine/Core/Engine.hpp"


//-----------------------------------------------------------------------------------------------
Engine* g_engine = nullptr;


//-----------------------------------------------------------------------------------------------
Engine::Engine( EngineConfig const& config )
	: m_config( config )
{
	g_engine = this;

	if ( config.m_windowConfig.m_isEnabled == true ) m_window = new Window( config.m_windowConfig );
	if ( config.m_rendererConfig.m_isEnabled == true ) m_renderer = new Renderer( config.m_rendererConfig );
	if ( config.m_inputConfig.m_isEnabled == true ) m_inputSystem = new InputSystem( config.m_inputConfig );
	if ( config.m_audioConfig.m_isEnabled == true ) m_audioSystem = new AudioSystem( config.m_audioConfig );
	if ( config.m_eventSystemConfig.m_isEnabled == true ) m_eventSystem = new EventSystem( config.m_eventSystemConfig );
	if ( config.m_devConsoleConfig.m_isEnabled == true ) m_devConsole = new DevConsole( config.m_devConsoleConfig );
}


//-----------------------------------------------------------------------------------------------
Engine::~Engine()
{
	delete m_audioSystem;
	m_audioSystem = nullptr;

	delete m_inputSystem;
	m_inputSystem = nullptr;

	delete m_eventSystem;
	m_eventSystem = nullptr;

	delete m_devConsole;
	m_devConsole = nullptr;

	delete m_renderer;
	m_renderer = nullptr;

	delete m_window;
	m_window = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Engine::Startup()
{
	if ( m_window ) m_window->Startup();
	if ( m_renderer ) m_renderer->Startup();
	if ( m_devConsole ) m_devConsole->Startup();
	if ( m_eventSystem ) m_eventSystem->Startup();
	if ( m_inputSystem ) m_inputSystem->StartUp();
	if ( m_audioSystem ) m_audioSystem->Startup();
}


//-----------------------------------------------------------------------------------------------
void Engine::Shutdown()
{
	if ( m_audioSystem ) m_audioSystem->Shutdown();
	if ( m_inputSystem ) m_inputSystem->ShutDown();
	if ( m_devConsole ) m_devConsole->Shutdown();
	if ( m_eventSystem ) m_eventSystem->Shutdown();
	if ( m_renderer ) m_renderer->Shutdown();
	if ( m_window ) m_window->Shutdown();
}


//-----------------------------------------------------------------------------------------------
void Engine::BeginFrame()
{
	if ( m_window ) m_window->BeginFrame();
	if ( m_renderer ) m_renderer->BeginFrame();
	if ( m_eventSystem ) m_eventSystem->BeginFrame();
	if ( m_devConsole ) m_devConsole->BeginFrame();
	if ( m_inputSystem ) m_inputSystem->BeginFrame();
	if ( m_audioSystem ) m_audioSystem->BeginFrame();
}


//-----------------------------------------------------------------------------------------------
void Engine::EndFrame()
{
	if ( m_window ) m_window->EndFrame();
	if ( m_renderer ) m_renderer->EndFrame();
	if ( m_eventSystem ) m_eventSystem->EndFrame();
	if ( m_devConsole ) m_devConsole->EndFrame();
	if ( m_inputSystem ) m_inputSystem->EndFrame();
	if ( m_audioSystem ) m_audioSystem->EndFrame();
}