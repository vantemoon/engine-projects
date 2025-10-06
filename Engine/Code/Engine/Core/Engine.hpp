#pragma once


//-----------------------------------------------------------------------------------------------
class AudioSystem; // Forward declaration
class Engine;      // Forward declaration
class InputSystem; // Forward declaration
class Renderer;    // Forward declaration

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
	AudioSystem* m_audioSystem = nullptr;
	InputSystem* m_inputSystem = nullptr;
	Renderer* m_renderer = nullptr;
};