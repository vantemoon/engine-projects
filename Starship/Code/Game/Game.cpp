#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/App.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_playerShip = new PlayerShip( this, Vec2(WORLD_CENTER_X, WORLD_CENTER_Y), Vec2(0.f, 0.f));
	
	// Spawn 6 asteroids in the game world
	RandomNumberGenerator rng;
	for ( int asteroidIndex = 0; asteroidIndex < NUM_STARTING_ASTEROIDS; ++asteroidIndex )
	{
		float randomX = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_X);
		float randomY = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_Y );
		float randomOrientation = rng.RollRandomFloatInRange( 0.f, 360.f );
		Vec2 randomDirection = Vec2::MakeFromPolarDegrees( rng.RollRandomFloatInRange( 0.f, 360.f ), 1.f );
		Vec2 randomVelocity = randomDirection * ASTEROID_SPEED;
		float randomAngularVelocity = rng.RollRandomFloatInRange( -200.f, 200.f );
		m_asteroids[asteroidIndex] = new Asteroid( this, Vec2( randomX, randomY ), randomOrientation, randomVelocity, randomAngularVelocity );
	}
	
	m_gameCamera = new Camera();
}


//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	delete m_playerShip;
	m_playerShip = nullptr;
	
	for ( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
	{
		delete m_bullets[bulletIndex];
		m_bullets[bulletIndex] = nullptr;
	}

	for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex )
	{
		delete m_asteroids[asteroidIndex];
		m_asteroids[asteroidIndex] = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
void Game::DeleteGarbageEntities()
{
	if(g_app->m_game == nullptr)
		return;

	if(g_app->m_game->m_playerShip != nullptr && g_app->m_game->m_playerShip->m_isGarbage)
	{
		delete g_app->m_game->m_playerShip;
		g_app->m_game->m_playerShip = nullptr;
	}

	for(int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		if(g_app->m_game->m_bullets[bulletIndex] != nullptr && g_app->m_game->m_bullets[bulletIndex]->m_isGarbage)
		{
			delete g_app->m_game->m_bullets[bulletIndex];
			g_app->m_game->m_bullets[bulletIndex] = nullptr;
		}
	}

	for(int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
	{
		if(g_app->m_game->m_asteroids[asteroidIndex] != nullptr && g_app->m_game->m_asteroids[asteroidIndex]->m_isGarbage)
		{
			delete g_app->m_game->m_asteroids[asteroidIndex];
			g_app->m_game->m_asteroids[asteroidIndex] = nullptr;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::Update( float deltaSeconds )
{
	UpdateEntities( deltaSeconds );

	g_app->m_game->DeleteGarbageEntities();
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateEntities( float deltaSeconds )
{
	m_playerShip->Update( deltaSeconds );

	for(int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		if(m_bullets[bulletIndex] != nullptr)
		{
			m_bullets[bulletIndex]->Update( deltaSeconds );
		}
	}

	for(int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
	{
		if(m_asteroids[asteroidIndex] != nullptr)
		{
			m_asteroids[asteroidIndex]->Update( deltaSeconds );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::DebugDraw() const
{
	if ( m_playerShip != nullptr && !m_playerShip->m_isDead )
	{
		// Draw lines from the player ship to all asteroids
		for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex )
		{
			if ( m_asteroids[asteroidIndex] != nullptr )
			{
				DebugDrawLine( m_playerShip->m_position, m_asteroids[asteroidIndex]->m_position.x, m_asteroids[asteroidIndex]->m_position.y, 0.3f, Rgba8( 50, 50, 50 ) );
			}
		}

		// Draw lines from the player ship to all bullets
		for ( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
		{
			if ( m_bullets[bulletIndex] != nullptr )
			{
				DebugDrawLine( m_playerShip->m_position, m_bullets[bulletIndex]->m_position.x, m_bullets[bulletIndex]->m_position.y, 0.3f, Rgba8( 50, 50, 50 ) );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_gameCamera );
	
	RenderEntities();

	if ( g_app->m_debugDraw )
	{
		DebugDraw();
	}

	g_engine->m_renderer->EndCamera( *m_gameCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderEntities() const
{
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
}


//-----------------------------------------------------------------------------------------------
void Game::SpawnRandomAsteroid()
{
	if ( m_playerShip == nullptr || m_playerShip->m_isDead )
		return;

	bool hasFreeSlot = false;
	for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex )
	{
		if ( m_asteroids[asteroidIndex] == nullptr )
		{
			hasFreeSlot = true;
			RandomNumberGenerator rng;
			float randomX = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_X );
			float randomY = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_Y );
			float randomOrientation = rng.RollRandomFloatInRange( 0.f, 360.f );
			Vec2 randomDirection = Vec2::MakeFromPolarDegrees( rng.RollRandomFloatInRange( 0.f, 360.f ), 1.f );
			Vec2 randomVelocity = randomDirection * ASTEROID_SPEED;
			float randomAngularVelocity = rng.RollRandomFloatInRange( -200.f, 200.f );
			m_asteroids[asteroidIndex] = new Asteroid( this, Vec2( randomX, randomY ), randomOrientation, randomVelocity, randomAngularVelocity );
			break;
		}
	}
	if ( !hasFreeSlot )
	{
		ERROR_RECOVERABLE( "No available asteroid slots!" );
	}
}


//-----------------------------------------------------------------------------------------------
void Game::SpawnBulletFromPlayerShip()
{
	if ( m_playerShip == nullptr || m_playerShip->m_isDead )
		return;

	bool hasFreeSlot = false;
	for ( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
	{
		if ( m_bullets[bulletIndex] == nullptr )
		{
			hasFreeSlot = true;
			m_bullets[bulletIndex] = new Bullet( this, m_playerShip );
			break;
		}
	}
	if ( !hasFreeSlot )
	{
		ERROR_RECOVERABLE( "No available bullet slots!" );
	}
}