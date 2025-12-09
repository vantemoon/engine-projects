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

	void Startup();
	void LoadGameConfigFromFile( char const* filepath );
	void RunMainLoop();
	void RunFrame();
	void Update( float deltaSeconds );
	void UpdateFromKeyboard();
	void UpdateFromController();
	void Render()									const;

	static bool Command_Quit( EventArgs& args );
	void SetIsQuitting();
	bool IsQuitting()								const;
	void HardReset();

public:
	Game*			m_game = nullptr;

	double			m_lastFrameStartTime = 0.0;
	bool			m_isQuitting = false;
};