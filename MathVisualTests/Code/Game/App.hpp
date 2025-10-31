#pragma once
#include "Game/GameCommon.hpp"
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
	void Update( float deltaSeconds );
	void UpdateFromKeyboard();
	void UpdateFromController();
	void Render()									const;

	void SetIsQuitting();
	bool IsQuitting()								const;
	void HardReset( GameMode newGameMode );

public:
	Game*			m_game = nullptr;
	GameMode		m_gameMode = INVALID_MODE;

	double			m_lastFrameStartTime = 0.0;
	bool			m_isQuitting = false;
};