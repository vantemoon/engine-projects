#include "Engine/Core/Engine.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Engine* g_engine = nullptr;


//-----------------------------------------------------------------------------------------------
Engine::Engine()
{
	g_engine = this;
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
	m_renderer->Shutdown();
	delete m_renderer;
	m_renderer = nullptr;

	m_inputSystem->ShutDown();
	delete m_inputSystem;
	m_inputSystem = nullptr;

	m_audioSystem->Shutdown();
	delete m_audioSystem;
	m_audioSystem = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Engine::BeginFrame()
{
	m_renderer->BeginFrame();
	m_inputSystem->BeginFrame();
	m_audioSystem->BeginFrame();
}


//-----------------------------------------------------------------------------------------------
void Engine::EndFrame()
{
	m_renderer->EndFrame();
	m_inputSystem->EndFrame();
	m_audioSystem->EndFrame();
}