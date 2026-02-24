#pragma once
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Camera.hpp"


//-----------------------------------------------------------------------------------------------
class App;        // Forward declaration
class Engine;     // Forward declaration
class Game;       // Forward declaration
class PlayerShip; // Forward declaration


//-----------------------------------------------------------------------------------------------
extern App* g_app;


//-----------------------------------------------------------------------------------------------
class App
{
public:
	App();
	~App();

	void RunMainLoop();
	void RunFrame();
	void Update();
	void UpdateFromKeyboard();
	void UpdateFromController();
	void UpdateMouse();
	void Render()									const;

	static bool Command_Quit( EventArgs& args );
	void SetIsQuitting();
	bool IsQuitting()								const;
	void HardReset();

public:
	Game*			m_game = nullptr;

	bool			m_isQuitting = false;
};