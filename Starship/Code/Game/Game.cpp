#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/App.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_playerShip = new PlayerShip( this, Vec2(100.f, 50.f), Vec2(0.f, 0.f));
	m_asteroids[0] = new Asteroid( this, Vec2( 20.f, 50.f ), Vec2( 10.f, 0.f ), 15.f );
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
void DeleteGarbageEntities()
{
	if(g_app->m_game == nullptr)
		return;

	if(g_app->m_game->m_playerShip != nullptr && g_app->m_game->m_playerShip->m_isGarbage)
	{
		delete g_app->m_game->m_playerShip;
		g_app->m_game->m_playerShip = nullptr;
	}

	for(int bulletIndex = 0; bulletIndex < Game::MAX_BULLETS; ++bulletIndex)
	{
		if(g_app->m_game->m_bullets[bulletIndex] != nullptr && g_app->m_game->m_bullets[bulletIndex]->m_isGarbage)
		{
			delete g_app->m_game->m_bullets[bulletIndex];
			g_app->m_game->m_bullets[bulletIndex] = nullptr;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::Update( float deltaSeconds )
{
	m_playerShip->Update(deltaSeconds);
	
	for ( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
	{
		if ( m_bullets[bulletIndex] != nullptr )
		{
			m_bullets[bulletIndex]->Update(deltaSeconds);
		}
	}

	for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex )
	{
		if ( m_asteroids[asteroidIndex] != nullptr )
		{
			m_asteroids[asteroidIndex]->Update(deltaSeconds);
		}
	}

	DeleteGarbageEntities();
}


//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_gameCamera );
	
	m_playerShip->Render();
	
	for ( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
	{
		if ( m_bullets[bulletIndex] != nullptr )
		{
			m_bullets[bulletIndex]->Render();
		}
	}

	for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex )
	{
		if ( m_asteroids[asteroidIndex] != nullptr )
		{
			m_asteroids[asteroidIndex]->Render();
		}
	}

	g_engine->m_renderer->EndCamera( *m_gameCamera );
}