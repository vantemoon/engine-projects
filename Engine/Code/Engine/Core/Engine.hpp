#pragma once
#include "Engine/Window/Window.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"


//-----------------------------------------------------------------------------------------------
class Engine;      // Forward declaration
class Window;      // Forward declaration
class Renderer;    // Forward declaration
class InputSystem; // Forward declaration
class AudioSystem; // Forward declaration

//-----------------------------------------------------------------------------------------------
extern Engine* g_engine; // Advertisement that this global exists, so external people can use it


//-----------------------------------------------------------------------------------------------
struct EngineConfig
{
	WindowConfig  m_windowConfig;
	RenderConfig  m_rendererConfig;
	InputConfig   m_inputConfig;
	AudioConfig   m_audioConfig;
};


//-----------------------------------------------------------------------------------------------
class Engine
{
public: 
	Engine( EngineConfig const& config );
	~Engine();
	void BeginFrame();
	void EndFrame();

public:
	Window*      m_window      = nullptr;
	Renderer*    m_renderer    = nullptr;
	InputSystem* m_inputSystem = nullptr;
	AudioSystem* m_audioSystem = nullptr;

	EngineConfig m_config;
};