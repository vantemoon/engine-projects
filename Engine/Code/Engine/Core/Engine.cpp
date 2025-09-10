#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Engine* g_engine = nullptr;


//-----------------------------------------------------------------------------------------------
Engine::Engine()
{
	g_engine = this;
	m_renderer = new Renderer();
	m_renderer->Startup();
}


//-----------------------------------------------------------------------------------------------
Engine::~Engine()
{
	m_renderer->Shutdown();
	delete m_renderer;
	m_renderer = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Engine::BeginFrame()
{
	m_renderer->BeginFrame();
}


//-----------------------------------------------------------------------------------------------
void Engine::EndFrame()
{
	m_renderer->EndFrame();
}