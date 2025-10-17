#pragma once


//-----------------------------------------------------------------------------------------------
class Engine;      // Forward declaration
class Window;      // Forward declaration
class Renderer;    // Forward declaration
class InputSystem; // Forward declaration
class AudioSystem; // Forward declaration

//-----------------------------------------------------------------------------------------------
extern Engine* g_engine; // Advertisement that this global exists, so external people can use it

//-----------------------------------------------------------------------------------------------
class Engine
{
public: 
	Engine();
	~Engine();
	void BeginFrame();
	void EndFrame();

public:
	Window*      m_window      = nullptr;
	Renderer*    m_renderer    = nullptr;
	InputSystem* m_inputSystem = nullptr;
	AudioSystem* m_audioSystem = nullptr;
};