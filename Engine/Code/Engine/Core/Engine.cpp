#include "Engine/Core/Engine.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/Window.hpp"


//-----------------------------------------------------------------------------------------------
Engine* g_engine = nullptr;


//-----------------------------------------------------------------------------------------------
Engine::Engine()
{
	g_engine = this;

	m_window = new Window();
	m_window->Startup();

	m_renderer = new Renderer();
	m_renderer->Startup();

	m_inputSystem = new InputSystem();
	m_inputSystem->StartUp();

	m_audioSystem = new AudioSystem();
	m_audioSystem->Startup();
}


//-----------------------------------------------------------------------------------------------
Engine::~Engine()
{
	m_audioSystem->Shutdown();
	m_inputSystem->ShutDown();
	m_renderer->Shutdown();
	m_window->Shutdown();

	delete m_audioSystem;
	m_audioSystem = nullptr;

	delete m_inputSystem;
	m_inputSystem = nullptr;

	delete m_renderer;
	m_renderer = nullptr;

	delete m_window;
	m_window = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Engine::BeginFrame()
{
	m_window->BeginFrame();
	m_renderer->BeginFrame();
	m_inputSystem->BeginFrame();
	m_audioSystem->BeginFrame();
}


//-----------------------------------------------------------------------------------------------
void Engine::EndFrame()
{
	m_window->EndFrame();
	m_renderer->EndFrame();
	m_inputSystem->EndFrame();
	m_audioSystem->EndFrame();
}