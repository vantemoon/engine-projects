#include "Engine/Core/Engine.hpp"


//-----------------------------------------------------------------------------------------------
Engine* g_engine = nullptr;


//-----------------------------------------------------------------------------------------------
Engine::Engine( EngineConfig const& config )
	: m_config( config )
{
	g_engine = this;

	m_window = new Window( config.m_windowConfig );
	m_renderer = new Renderer( config.m_rendererConfig );
	m_inputSystem = new InputSystem( config.m_inputConfig );
	m_audioSystem = new AudioSystem( config.m_audioConfig );
	m_eventSystem = new EventSystem( config.m_eventSystemConfig );
	m_devConsole = new DevConsole( config.m_devConsoleConfig );
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
	m_window->Startup();
	m_renderer->Startup();
	m_devConsole->Startup();
	m_eventSystem->Startup();
	m_inputSystem->StartUp();
	m_audioSystem->Startup();
}


//-----------------------------------------------------------------------------------------------
void Engine::Shutdown()
{
	m_audioSystem->Shutdown();
	m_inputSystem->ShutDown();
	m_devConsole->Shutdown();
	m_eventSystem->Shutdown();
	m_renderer->Shutdown();
	m_window->Shutdown();
}


//-----------------------------------------------------------------------------------------------
void Engine::BeginFrame()
{
	m_window->BeginFrame();
	m_renderer->BeginFrame();
	m_eventSystem->BeginFrame();
	m_devConsole->BeginFrame();
	m_inputSystem->BeginFrame();
	m_audioSystem->BeginFrame();
}


//-----------------------------------------------------------------------------------------------
void Engine::EndFrame()
{
	m_window->EndFrame();
	m_renderer->EndFrame();
	m_eventSystem->EndFrame();
	m_devConsole->EndFrame();
	m_inputSystem->EndFrame();
	m_audioSystem->EndFrame();
}