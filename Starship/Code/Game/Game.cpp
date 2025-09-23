#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/App.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Beetle.hpp"
#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Wasp.hpp"


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_playerShip = new PlayerShip( this, Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ), Vec2( 0.f, 0.f ) );
	
	SpawnRandomAsteroids(4);
	SpawnRandomBeetles(2);
	SpawnRandomWasps(1);
	
	m_gameCamera = new Camera();
	m_attractCamera = new Camera();
}


//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	delete m_playerShip;
	m_playerShip = nullptr;
	
	for ( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++ bulletIndex )
	{
		delete m_bullets[bulletIndex];
		m_bullets[bulletIndex] = nullptr;
	}

	for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++ asteroidIndex )
	{
		delete m_asteroids[asteroidIndex];
		m_asteroids[asteroidIndex] = nullptr;
	}

	for ( int beetleIndex = 0; beetleIndex < 2; ++ beetleIndex )
	{
		delete m_beetles[beetleIndex];
		m_beetles[beetleIndex] = nullptr;
	}

	delete m_gameCamera;
	m_gameCamera = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Game::DeleteGarbageEntities()
{
	if ( g_app->m_game == nullptr )
		return;

	// Player ship
	if ( g_app->m_game->m_playerShip != nullptr && g_app->m_game->m_playerShip->m_isGarbage )
	{
		delete g_app->m_game->m_playerShip;
		g_app->m_game->m_playerShip = nullptr;
	}

	// Bullets
	for ( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++ bulletIndex )
	{
		if ( g_app->m_game->m_bullets[bulletIndex] != nullptr && g_app->m_game->m_bullets[bulletIndex]->m_isGarbage )
		{
			delete g_app->m_game->m_bullets[bulletIndex];
			g_app->m_game->m_bullets[bulletIndex] = nullptr;
		}
	}

	// Asteroids
	for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++ asteroidIndex )
	{
		if ( g_app->m_game->m_asteroids[asteroidIndex] != nullptr && g_app->m_game->m_asteroids[asteroidIndex]->m_isGarbage )
		{
			delete g_app->m_game->m_asteroids[asteroidIndex];
			g_app->m_game->m_asteroids[asteroidIndex] = nullptr;
		}
	}

	// Beetles
	for ( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++ beetleIndex )
	{
		if ( g_app->m_game->m_beetles[beetleIndex] != nullptr && g_app->m_game->m_beetles[beetleIndex]->m_isGarbage )
		{
			delete g_app->m_game->m_beetles[beetleIndex];
			g_app->m_game->m_beetles[beetleIndex] = nullptr;
		}
	}

	// Wasp
	for ( int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex )
	{
		if ( g_app->m_game->m_wasps[waspIndex] != nullptr && g_app->m_game->m_wasps[waspIndex]->m_isGarbage )
		{
			delete g_app->m_game->m_wasps[waspIndex];
			g_app->m_game->m_wasps[waspIndex] = nullptr;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::Update( float deltaSeconds )
{
	if ( m_isAttractMode )
	{
		UpdateAttractMode( deltaSeconds );
		return;
	};

	if ( m_isAttractMode )
	{
		return;
	};

	UpdateEntities( deltaSeconds );

	m_gameCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );

	g_app->m_game->DeleteGarbageEntities();
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateAttractMode( [[maybe_unused]] float deltaSeconds )
{
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) || g_engine->m_inputSystem->WasKeyJustPressed( 'N' ) )
	{
		m_isAttractMode = false;
	};
	m_attractCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
}



//-----------------------------------------------------------------------------------------------
void Game::UpdateEntities( float deltaSeconds )
{
	// Player ship
	m_playerShip->Update( deltaSeconds );

	// Bullets
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++ bulletIndex )
	{
		if( m_bullets[bulletIndex] != nullptr )
		{
			m_bullets[bulletIndex]->Update( deltaSeconds );
		}
	}

	// Asteroids
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++ asteroidIndex )
	{
		if( m_asteroids[asteroidIndex] != nullptr )
		{
			m_asteroids[asteroidIndex]->Update( deltaSeconds );
		}
	}

	// Beetles
	for( int beetleIndex = 0; beetleIndex < 2; ++ beetleIndex )
	{
		if( m_beetles[beetleIndex] != nullptr )
		{
			m_beetles[beetleIndex]->Update( deltaSeconds );
		}
	}

	// Wasps
	for( int waspIndex = 0; waspIndex < 2; ++ waspIndex )
	{
		if( m_wasps[waspIndex] != nullptr )
		{
			m_wasps[waspIndex]->Update( deltaSeconds );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::DebugDraw() const
{
	if ( m_playerShip != nullptr )
	{
		//Player ship
		DebugDrawLine( Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ), m_playerShip->m_position, 0.2f, Rgba8( 50, 50, 50 ), Rgba8( 50, 50, 50 ) );
		DebugDrawRing( m_playerShip->m_position, m_playerShip->m_cosmeticRadius, 0.1f, Rgba8( 255, 0, 255 ) );
		DebugDrawRing( m_playerShip->m_position, m_playerShip->m_physicsRadius, 0.1f, Rgba8( 0, 255, 255 ) );
		
		Vec2 playershipForwardNormal = m_playerShip->GetForwardNormal();
		Vec2 playershipForwardLineEnd = m_playerShip->m_position + playershipForwardNormal * m_playerShip->m_cosmeticRadius;
		DebugDrawLine( m_playerShip->m_position, playershipForwardLineEnd, 0.2f, Rgba8( 255, 0, 0 ), Rgba8( 255, 0, 0 ) );

		Vec2 playershipLeftNormal = playershipForwardNormal.GetRotatedBy90Degrees();
		Vec2 playershipLeftLineEnd = m_playerShip->m_position + playershipLeftNormal * m_playerShip->m_cosmeticRadius;
		DebugDrawLine( m_playerShip->m_position, playershipLeftLineEnd, 0.2f, Rgba8( 0, 255, 0 ), Rgba8( 0, 255, 0 ) );

		Vec2 playershipVelocityLineEnd = m_playerShip->m_position + m_playerShip->m_velocity;
		DebugDrawLine( m_playerShip->m_position, playershipVelocityLineEnd, 0.2f, Rgba8( 255, 255, 0 ), Rgba8( 255, 255, 0 ) );

		// Asteroids
		for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++ asteroidIndex )
		{
			if ( m_asteroids[asteroidIndex] != nullptr )
			{
				DebugDrawLine( Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ), m_asteroids[asteroidIndex]->m_position, 0.2f, Rgba8( 50, 50, 50 ), Rgba8( 50, 50, 50 ) );
				DebugDrawRing( m_asteroids[asteroidIndex]->m_position, m_asteroids[asteroidIndex]->m_cosmeticRadius, 0.1f, Rgba8( 255, 0, 255 ) );
				DebugDrawRing( m_asteroids[asteroidIndex]->m_position, m_asteroids[asteroidIndex]->m_physicsRadius, 0.1f, Rgba8( 0, 255, 255 ) );
		    
				Vec2 asteroidForwardNormal = m_asteroids[asteroidIndex]->GetForwardNormal();
				Vec2 asteroidForwardLineEnd = m_asteroids[asteroidIndex]->m_position + asteroidForwardNormal * m_asteroids[asteroidIndex]->m_cosmeticRadius;
				DebugDrawLine( m_asteroids[asteroidIndex]->m_position, asteroidForwardLineEnd, 0.2f, Rgba8( 255, 0, 0 ), Rgba8( 255, 0, 0 ) );
			
				Vec2 asteroidLeftNormal = asteroidForwardNormal.GetRotatedBy90Degrees();
				Vec2 asteroidLeftLineEnd = m_asteroids[asteroidIndex]->m_position + asteroidLeftNormal * m_asteroids[asteroidIndex]->m_cosmeticRadius;
				DebugDrawLine( m_asteroids[asteroidIndex]->m_position, asteroidLeftLineEnd, 0.2f, Rgba8( 0, 255, 0 ), Rgba8( 0, 255, 0 ) );
			
				Vec2 asteroidVelocityLineEnd = m_asteroids[asteroidIndex]->m_position + m_asteroids[asteroidIndex]->m_velocity;
				DebugDrawLine( m_asteroids[asteroidIndex]->m_position, asteroidVelocityLineEnd, 0.2f, Rgba8( 255, 255, 0 ), Rgba8( 255, 255, 0 ) );
			}
		}

		// Bullets
		for ( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++ bulletIndex )
		{
			if ( m_bullets[bulletIndex] != nullptr )
			{
				DebugDrawLine( Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ), m_bullets[bulletIndex]->m_position, 0.2f, Rgba8( 50, 50, 50 ), Rgba8( 50, 50, 50 ) );
				DebugDrawRing( m_bullets[bulletIndex]->m_position, m_bullets[bulletIndex]->m_cosmeticRadius, 0.1f, Rgba8( 255, 0, 255 ) );
				DebugDrawRing( m_bullets[bulletIndex]->m_position, m_bullets[bulletIndex]->m_physicsRadius, 0.1f, Rgba8( 0, 255, 255 ) );
			
				Vec2 bulletForwardNormal = m_bullets[bulletIndex]->GetForwardNormal();
				Vec2 bulletForwardLineEnd = m_bullets[bulletIndex]->m_position + bulletForwardNormal * m_bullets[bulletIndex]->m_cosmeticRadius;
				DebugDrawLine( m_bullets[bulletIndex]->m_position, bulletForwardLineEnd, 0.2f, Rgba8( 255, 0, 0 ), Rgba8( 255, 0, 0 ) );
			
				Vec2 bulletLeftNormal = bulletForwardNormal.GetRotatedBy90Degrees();
				Vec2 bulletLeftLineEnd = m_bullets[bulletIndex]->m_position + bulletLeftNormal * m_bullets[bulletIndex]->m_cosmeticRadius;
				DebugDrawLine( m_bullets[bulletIndex]->m_position, bulletLeftLineEnd, 0.2f, Rgba8( 0, 255, 0 ), Rgba8( 0, 255, 0 ) );
			
				Vec2 bulletVelocityLineEnd = m_bullets[bulletIndex]->m_position + m_bullets[bulletIndex]->m_velocity;
				DebugDrawLine( m_bullets[bulletIndex]->m_position, bulletVelocityLineEnd, 0.2f, Rgba8( 255, 255, 0 ), Rgba8( 255, 255, 0 ) );
			}
		}

		// Beetles
		for ( int beetleIndex = 0; beetleIndex < 2; ++beetleIndex )
		{
			if ( m_beetles[beetleIndex] != nullptr )
			{
				DebugDrawLine( Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ), m_beetles[beetleIndex]->m_position, 0.2f, Rgba8( 50, 50, 50 ), Rgba8( 50, 50, 50 ) );
				DebugDrawRing( m_beetles[beetleIndex]->m_position, m_beetles[beetleIndex]->m_cosmeticRadius, 0.1f, Rgba8( 255, 0, 255 ) );
				DebugDrawRing( m_beetles[beetleIndex]->m_position, m_beetles[beetleIndex]->m_physicsRadius, 0.1f, Rgba8( 0, 255, 255 ) );

				Vec2 beetleForwardNormal = m_beetles[beetleIndex]->GetForwardNormal();
				Vec2 beetleForwardLineEnd = m_beetles[beetleIndex]->m_position + beetleForwardNormal * m_beetles[beetleIndex]->m_cosmeticRadius;
				DebugDrawLine( m_beetles[beetleIndex]->m_position, beetleForwardLineEnd, 0.2f, Rgba8( 255, 0, 0 ), Rgba8( 255, 0, 0 ) );

				Vec2 beetleLeftNormal = beetleForwardNormal.GetRotatedBy90Degrees();
				Vec2 beetleLeftLineEnd = m_beetles[beetleIndex]->m_position + beetleLeftNormal * m_beetles[beetleIndex]->m_cosmeticRadius;
				DebugDrawLine( m_beetles[beetleIndex]->m_position, beetleLeftLineEnd, 0.2f, Rgba8( 0, 255, 0 ), Rgba8( 0, 255, 0 ) );

				Vec2 beetleVelocityLineEnd = m_beetles[beetleIndex]->m_position + m_beetles[beetleIndex]->m_velocity;
				DebugDrawLine( m_beetles[beetleIndex]->m_position, beetleVelocityLineEnd, 0.2f, Rgba8( 255, 255, 0 ), Rgba8( 255, 255, 0 ) );
			}
		}

		// Wasps
		for ( int waspIndex = 0; waspIndex < 2; ++waspIndex )
		{
			if ( m_wasps[waspIndex] != nullptr )
			{
				DebugDrawLine( Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ), m_wasps[waspIndex]->m_position, 0.2f, Rgba8( 50, 50, 50 ), Rgba8( 50, 50, 50 ) );
				DebugDrawRing( m_wasps[waspIndex]->m_position, m_wasps[waspIndex]->m_cosmeticRadius, 0.1f, Rgba8( 255, 0, 255 ) );
				DebugDrawRing( m_wasps[waspIndex]->m_position, m_wasps[waspIndex]->m_physicsRadius, 0.1f, Rgba8( 0, 255, 255 ) );

				Vec2 waspForwardNormal = m_wasps[waspIndex]->GetForwardNormal();
				Vec2 waspForwardLineEnd = m_wasps[waspIndex]->m_position + waspForwardNormal * m_wasps[waspIndex]->m_cosmeticRadius;
				DebugDrawLine( m_wasps[waspIndex]->m_position, waspForwardLineEnd, 0.2f, Rgba8( 255, 0, 0 ), Rgba8( 255, 0, 0 ) );

				Vec2 waspLeftNormal = waspForwardNormal.GetRotatedBy90Degrees();
				Vec2 waspLeftLineEnd = m_wasps[waspIndex]->m_position + waspLeftNormal * m_wasps[waspIndex]->m_cosmeticRadius;
				DebugDrawLine( m_wasps[waspIndex]->m_position, waspLeftLineEnd, 0.2f, Rgba8( 0, 255, 0 ), Rgba8( 0, 255, 0 ) );

				Vec2 waspVelocityLineEnd = m_wasps[waspIndex]->m_position + m_wasps[waspIndex]->m_velocity;
				DebugDrawLine( m_wasps[waspIndex]->m_position, waspVelocityLineEnd, 0.2f, Rgba8( 255, 255, 0 ), Rgba8( 255, 255, 0 ) );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	if ( m_isAttractMode )
	{
		RenderAttractMode();
		return;
	};

	g_engine->m_renderer->BeginCamera( *m_gameCamera );
	
	RenderEntities();

	if ( g_app->m_debugDraw )
	{
		DebugDraw();
	}

	g_engine->m_renderer->EndCamera( *m_gameCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	g_engine->m_renderer->BeginCamera( *m_attractCamera );

	DebugDrawLine( Vec2( 10.f, 10.f ), Vec2( 190.f, 90.f ), 1.f, Rgba8( 255, 0, 0 ), Rgba8( 0, 255, 0 ) );

	g_engine->m_renderer->EndCamera( *m_attractCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderEntities() const
{
	// Player ship
	m_playerShip->Render();

	// Bullets
	for ( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++ bulletIndex )
	{
		if ( m_bullets[bulletIndex] != nullptr )
		{
			m_bullets[bulletIndex]->Render();
		}
	}

	// Asteroids
	for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++ asteroidIndex )
	{
		if ( m_asteroids[asteroidIndex] != nullptr )
		{
			m_asteroids[asteroidIndex]->Render();
		}
	}

	// Beetles
	for ( int beetleIndex = 0; beetleIndex < 2; ++ beetleIndex )
	{
		if ( m_beetles[beetleIndex] != nullptr )
		{
			m_beetles[beetleIndex]->Render();
		}
	}

	// Wasps
	for ( int waspIndex = 0; waspIndex < 2; ++ waspIndex )
	{
		if ( m_wasps[waspIndex] != nullptr )
		{
			m_wasps[waspIndex]->Render();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::SpawnRandomAsteroids( int numOfAsteroid )
{
	if ( m_playerShip == nullptr || m_playerShip->m_isDead )
		return;

	for ( int asteroidSpawned = 0; asteroidSpawned < numOfAsteroid; ++asteroidSpawned )
	{
		bool hasFreeSlot = false;
		for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex )
		{
			if ( m_asteroids[asteroidIndex] == nullptr )
			{
				hasFreeSlot = true;
				RandomNumberGenerator rng;
				Vec2 randomOffscreenPosition = GetRandomOffscreenPosition( ASTEROID_COSMETIC_RADIUS );
				float randomOrientation = rng.RollRandomFloatInRange( 0.f, 360.f );
				Vec2 randomDirection = Vec2::MakeFromPolarDegrees( rng.RollRandomFloatInRange( 0.f, 360.f ), 1.f );
				Vec2 randomVelocity = randomDirection * ASTEROID_SPEED;
				float randomAngularVelocity = rng.RollRandomFloatInRange( -200.f, 200.f );
				m_asteroids[asteroidIndex] = new Asteroid( this, randomOffscreenPosition, randomOrientation, randomVelocity, randomAngularVelocity );
				break;
			}
		}
		if ( !hasFreeSlot )
		{
			ERROR_RECOVERABLE( "No available asteroid slots!" );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::SpawnBulletFromPlayerShip()
{
	if ( m_playerShip == nullptr || m_playerShip->m_isDead )
		return;

	bool hasFreeSlot = false;
	for ( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++ bulletIndex )
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


//-----------------------------------------------------------------------------------------------
void Game::SpawnRandomBeetles( int numOfBeetles )
{
	if ( m_playerShip == nullptr || m_playerShip->m_isDead )
		return;

	for ( int beetlesSpawned = 0; beetlesSpawned < numOfBeetles; ++beetlesSpawned )
	{
		bool hasFreeSlot = false;
		for ( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex )
		{
			if ( m_beetles[beetleIndex] == nullptr )
			{
				hasFreeSlot = true;
				Vec2 randomOffscreenPosition = GetRandomOffscreenPosition( BEETLE_COSMETIC_RADIUS );

				m_beetles[beetleIndex] = new Beetle( this, randomOffscreenPosition );
				break;
			}
		}
		if ( !hasFreeSlot )
		{
			ERROR_RECOVERABLE( "No available beetle slots!" );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::SpawnRandomWasps(int numOfWasps)
{
	if ( m_playerShip == nullptr || m_playerShip->m_isDead )
		return;
	
	for ( int waspsSpawned = 0; waspsSpawned < numOfWasps; ++waspsSpawned )
	{
		bool hasFreeSlot = false;
		for ( int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex )
		{
			if ( m_wasps[waspIndex] == nullptr )
			{
				hasFreeSlot = true;
				Vec2 randomOffscreenPosition = GetRandomOffscreenPosition( WASP_COSMETIC_RADIUS );

				m_wasps[waspIndex] = new Wasp( this, randomOffscreenPosition );
				break;
			}
		}
		if ( !hasFreeSlot )
		{
			ERROR_RECOVERABLE( "No available wasp slots!" );
		}
	}
}


//-----------------------------------------------------------------------------------------------
Vec2 Game::GetRandomOffscreenPosition( float cosmeticRadius ) const
{
	RandomNumberGenerator rng;
	float randomX = rng.RollRandomFloatInRange( -cosmeticRadius, WORLD_SIZE_X + cosmeticRadius );
	float randomY = rng.RollRandomFloatInRange( -cosmeticRadius, WORLD_SIZE_Y + cosmeticRadius );
	int side = rng.RollRandomIntLessThan( 4 ); // 0: left, 1: right, 2: bottom, 3: top
	switch ( side )
	{
		case 0: // left
			randomX = -cosmeticRadius;
			break;
		case 1: // right
			randomX = WORLD_SIZE_X + cosmeticRadius;
			break;
		case 2: // bottom
			randomY = -cosmeticRadius;
			break;
		case 3: // top
			randomY = WORLD_SIZE_Y + cosmeticRadius;
			break;
		default:
			break;
	}
	return Vec2( randomX, randomY );
}