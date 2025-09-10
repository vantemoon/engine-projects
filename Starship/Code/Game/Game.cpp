#include "Engine/Renderer/Camera.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_playerShip = new PlayerShip(this, Vec2(0.f, 50.f), Vec2(20.f, 0.f));
	// Add more initialization logic here (e.g. create asteroids, bullets, etc.)
	m_gameCamera = new Camera();
}


//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	delete m_playerShip;
	m_playerShip = nullptr;
	// Add more shutdown logic here (e.g. delete asteroids, bullets, etc.)
}


//-----------------------------------------------------------------------------------------------
void Game::Update( float deltaSeconds )
{
	m_playerShip->Update(deltaSeconds);
	// Add more update logic here (e.g. update all asteroids, bullets, etc.)
}


//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	m_playerShip->Render();
	// Add more render logic here (e.g. render all asteroids, bullets, etc.)
}