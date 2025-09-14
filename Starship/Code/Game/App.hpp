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
	void Render() const;

	void SetIsQuitting();
	bool IsQuitting() const;
	void HardReset();
	void OnKeyDown( unsigned char keyCode );
	void OnKeyUp( unsigned char keyCode );
	bool IsKeyDown( unsigned char keyCode ) const;
	bool WasKeyJustPressed( unsigned char keyCode ) const;
	bool WasKeyJustReleased( unsigned char keyCode ) const;

public:
	Game*			m_game = nullptr;
	Camera			m_gameCamera;

	bool			m_isQuitting = false;
	bool			m_isPaused = false;
	bool            m_isSlowMo = false;
	bool			m_pauseAfterNextUpdate = false;
	bool            m_debugDraw = false;

private:
	bool            m_isKeyDown[256] = { false };
	bool			m_wasKeyJustPressed[256] = { false };
};