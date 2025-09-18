#pragma once


//-----------------------------------------------------------------------------------------------
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
	Renderer* m_renderer = nullptr;
	InputSystem* m_inputSystem = nullptr;
};