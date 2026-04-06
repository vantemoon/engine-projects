#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Camera.hpp"


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_worldCamera = new Camera();
	m_screenCamera = new Camera();

	m_gameClock = new Clock();

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
}


//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	delete m_worldCamera;
	m_worldCamera = nullptr;

	delete m_screenCamera;
	m_screenCamera = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Game::Update( [[maybe_unused]] float deltaSeconds )
{
	// DO NOTHING
}


//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	// DO NOTHING
}