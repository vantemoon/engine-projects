#pragma once
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

	void RunFrame();
	void Update( float deltaSeconds );
	void UpdateFromKeyboard();
	void Render() const;

	void SetIsQuitting();
	bool IsQuitting() const;
	void HardReset();

public:
	Game*			m_game = nullptr;
	Camera			m_gameCamera;
	double			m_lastFrameStartTime = 0.0;

	bool			m_isQuitting = false;
	bool			m_isPaused = false;
	bool            m_isSlowMo = false;
	bool			m_pauseAfterNextUpdate = false;
	bool            m_debugDraw = false;
};