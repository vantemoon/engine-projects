#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Beetle.hpp"
#include "Game/Bullet.hpp"
#include "Game/Debris.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Wasp.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_playerShip = new PlayerShip( this, Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ), Vec2( 0.f, 0.f ) );
	
	m_worldCamera = new Camera();
	m_screenCamera = new Camera();

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	m_isAttractMode = true;
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

	for ( int waspIndex = 0; waspIndex < 2; ++ waspIndex )
	{
		delete m_wasps[waspIndex];
		m_wasps[waspIndex] = nullptr;
	}

	for ( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++ debrisIndex )
	{
		delete m_debris[debrisIndex];
		m_debris[debrisIndex] = nullptr;
	}

	delete m_worldCamera;
	m_worldCamera = nullptr;

	delete m_screenCamera;
	m_screenCamera = nullptr;
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

	// Debris
	for ( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++ debrisIndex )
	{
		if ( g_app->m_game->m_debris[debrisIndex] != nullptr && g_app->m_game->m_debris[debrisIndex]->m_isGarbage )
		{
			delete g_app->m_game->m_debris[debrisIndex];
			g_app->m_game->m_debris[debrisIndex] = nullptr;
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

	CheckPlayerLives();

	UpdateWaves();
	UpdateFromKeyboard();
	UpdateFromController();
	UpdateEntities( deltaSeconds );

	g_app->m_game->DeleteGarbageEntities();

	if ( m_isBackgroundMusicPlaying )
		g_engine->m_audioSystem->SetSoundPlaybackVolume( m_backgroundMusicSoundID, 0.25f );

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	if ( m_isScreenShaking )
	{
		float shakeElapsedTime = ( float ) GetCurrentTimeSeconds() - m_screenShakeStartTime;
		if ( shakeElapsedTime < m_screenShakeDuration )
		{
			ScreenShake( m_screenShakeIntensity );
			/*m_screenShakeIntensity *= 0.9f; // Dampen the shake intensity over time*/
		}
		else
		{
			m_isScreenShaking = false;
			m_screenShakeIntensity = 0.f;
			m_screenShakeDuration = 0.f;
			m_screenShakeStartTime = 0.f;
			m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::ScreenShake( float intensity )
{
	if ( m_worldCamera == nullptr || intensity <= 0.f )
		return;

	RandomNumberGenerator rng;
	float offsetX = rng.RollRandomFloatInRange( -intensity, intensity );
	float offsetY = rng.RollRandomFloatInRange( -intensity, intensity );
	m_worldCamera->Translate2D( Vec2( offsetX, offsetY ) );
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateWaves()
{
	if ( IsReadyToStartNextWave() )
	{
		if ( m_waveNumber == NUM_OF_WAVES )
		{
			// Return to attract mode
			m_isAttractMode = true;
			m_waveNumber = 0;

			SoundID gameWinSound = g_engine->m_audioSystem->CreateOrGetSound( "Data/Braam - Zone End.wav" );
			g_engine->m_audioSystem->StartSound( gameWinSound, false, 0.7f, 0.f, 0.8f );

			return;
		}

		StartNextWave();
	}
}


//-----------------------------------------------------------------------------------------------
bool Game::IsReadyToStartNextWave() const
{
	if ( m_playerShip == nullptr || m_playerShip->m_isDead )
		return false;

	// Asteroids
	for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++ asteroidIndex )
	{
		Asteroid* asteroid = m_asteroids[asteroidIndex];
		if ( asteroid != nullptr && asteroid->IsAlive() )
			return false;
	}

	// Beetles
	for ( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++ beetleIndex )
	{
		Beetle* beetle = m_beetles[beetleIndex];
		if ( beetle != nullptr && beetle->IsAlive() )
			return false;
	}

	// Wasps
	for ( int waspIndex = 0; waspIndex < MAX_WASPS; ++ waspIndex )
	{
		Wasp* wasp = m_wasps[waspIndex];
		if ( wasp != nullptr && wasp->IsAlive() )
			return false;
	}

	// Debris
	for ( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++ debrisIndex )
	{
		Debris* debris = m_debris[debrisIndex];
		if ( debris != nullptr && debris->IsAlive() )
			return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
void Game::StartNextWave()
{
	++ m_waveNumber;

	switch ( m_waveNumber )
	{
		default:
			break;

		case 1:
			SpawnRandomAsteroids( 4 );
			SpawnRandomBeetles( 1 );
			SpawnRandomWasps( 1 );
			break;

		case 2:
			SpawnRandomAsteroids( 6 );
			SpawnRandomBeetles( 2 );
			SpawnRandomWasps( 1 );
			break;

		case 3:
			SpawnRandomAsteroids( 8 );
			SpawnRandomBeetles( 2 );
			SpawnRandomWasps( 2 );
			break;

		case 4:
			SpawnRandomAsteroids( 10 );
			SpawnRandomBeetles( 5 );
			SpawnRandomWasps( 3 );
			break;

		case 5:
			SpawnRandomAsteroids( 12 );
			SpawnRandomBeetles( 5 );
			SpawnRandomWasps( 5 );
			break;
	}

	SoundID waveStartSound = g_engine->m_audioSystem->CreateOrGetSound( "Data/NewWave.wav" );
	g_engine->m_audioSystem->StartSound( waveStartSound, false, 0.7f, 0.f, 0.8f );
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateAttractMode( [[maybe_unused]] float deltaSeconds )
{
	UpdateFromKeyboard();
	UpdateFromController();
	// m_attractCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );

	if ( !m_isBackgroundMusicPlaying )
	{
		m_backgroundMusicSoundID = g_engine->m_audioSystem->CreateOrGetSound( "Data/12 Track 12.mp3" );
		g_engine->m_audioSystem->StartSound( m_backgroundMusicSoundID, true, 0.8f, 0.f, 1.f );
		m_isBackgroundMusicPlaying = true;
	}
	else
	{
		g_engine->m_audioSystem->SetSoundPlaybackVolume( m_backgroundMusicSoundID, 0.8f );
	}
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateFromKeyboard()
{
	if ( m_isAttractMode )
	{
		// Start the game
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) || g_engine->m_inputSystem->WasKeyJustPressed( 'N' ) )
		{
			Reset();
			m_isAttractMode = false;
		};

		// Quit the game
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
		{
			g_app->SetIsQuitting();
		}
	}
	else
	{
		// Return to attract mode
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
		{
			m_isAttractMode = true;
		}

		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'I' ) )
		{
			SpawnRandomAsteroids( 1 );
		}

		// Kill all enemies (for debugging)
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'K' ) )
		{
			KillAllEnemies();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateFromController()
{
	XboxController const& controller = g_engine->m_inputSystem->GetController( 0 );

	if ( m_isAttractMode )
	{
		// Start the game
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_A ) || controller.WasButtonJustPressed( XBOX_BUTTON_START ) )
		{
			Reset();
			m_isAttractMode = false;
		};
		// Quit the game
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
		{
			g_app->SetIsQuitting();
		}
	}
	else
	{
		// Return to attract mode
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
		{
			m_isAttractMode = true;
		}
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_X ) )
		{
			SpawnRandomAsteroids( 1 );
		}
		// Kill all enemies (for debugging)
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_Y ) )
		{
			KillAllEnemies();
		}
	}
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
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++ beetleIndex )
	{
		if( m_beetles[beetleIndex] != nullptr )
		{
			m_beetles[beetleIndex]->Update( deltaSeconds );

			for ( int otherBeetleIndex = beetleIndex + 1; otherBeetleIndex < MAX_BEETLES; ++otherBeetleIndex )
			{
				if ( m_beetles[otherBeetleIndex] != nullptr )
				{
					Beetle* beetleA = m_beetles[beetleIndex];
					Beetle* beetleB = m_beetles[otherBeetleIndex];
					PushDiscsOutOfEachOther2D( beetleA->m_position, beetleA->m_physicsRadius, beetleB->m_position, beetleB->m_physicsRadius );
				}
			}
		}
	}

	// Wasps
	for( int waspIndex = 0; waspIndex < MAX_WASPS; ++ waspIndex )
	{
		if( m_wasps[waspIndex] != nullptr )
		{
			m_wasps[waspIndex]->Update( deltaSeconds );
		}
	}

	// Debris
	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++ debrisIndex )
	{
		if( m_debris[debrisIndex] != nullptr )
		{
			m_debris[debrisIndex]->Update( deltaSeconds );
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

		// Debris
		for ( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex )
		{
			if ( m_debris[debrisIndex] != nullptr )
			{
				DebugDrawLine( Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ), m_debris[debrisIndex]->m_position, 0.2f, Rgba8( 50, 50, 50 ), Rgba8( 50, 50, 50 ) );
				DebugDrawRing( m_debris[debrisIndex]->m_position, m_debris[debrisIndex]->m_cosmeticRadius, 0.1f, Rgba8( 255, 0, 255 ) );
				DebugDrawRing( m_debris[debrisIndex]->m_position, m_debris[debrisIndex]->m_physicsRadius, 0.1f, Rgba8( 0, 255, 255 ) );

				Vec2 debrisForwardNormal = m_debris[debrisIndex]->GetForwardNormal();
				Vec2 debrisForwardLineEnd = m_debris[debrisIndex]->m_position + debrisForwardNormal * m_debris[debrisIndex]->m_cosmeticRadius;
				DebugDrawLine( m_debris[debrisIndex]->m_position, debrisForwardLineEnd, 0.2f, Rgba8( 255, 0, 0 ), Rgba8( 255, 0, 0 ) );

				Vec2 debrisLeftNormal = debrisForwardNormal.GetRotatedBy90Degrees();
				Vec2 debrisLeftLineEnd = m_debris[debrisIndex]->m_position + debrisLeftNormal * m_debris[debrisIndex]->m_cosmeticRadius;
				DebugDrawLine( m_debris[debrisIndex]->m_position, debrisLeftLineEnd, 0.2f, Rgba8( 0, 255, 0 ), Rgba8( 0, 255, 0 ) );

				Vec2 debrisVelocityLineEnd = m_debris[debrisIndex]->m_position + m_debris[debrisIndex]->m_velocity;
				DebugDrawLine( m_debris[debrisIndex]->m_position, debrisVelocityLineEnd, 0.2f, Rgba8( 255, 255, 0 ), Rgba8( 255, 255, 0 ) );
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

	g_engine->m_renderer->BeginCamera( *m_worldCamera );
	
	RenderEntities();
	RenderHUD();

	if ( g_app->m_debugDraw )
	{
		DebugDraw();
	}

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	float shipScale = SCREEN_SIZE_X * 0.04f;
	float shipOffsetX = SCREEN_SIZE_X * 0.2f;
	float centerX = SCREEN_SIZE_X * 0.5f;
	float centerY = SCREEN_SIZE_Y * 0.5f;

	Vertex shipVertexArray1[PlayerShip::NUM_SHIP_VERTS];
	Vertex shipVertexArray2[PlayerShip::NUM_SHIP_VERTS];
	m_playerShip->GetVertexArrayCopy( shipVertexArray1 );
	m_playerShip->GetVertexArrayCopy( shipVertexArray2 );
	TransformVertexArrayXY3D( PlayerShip::NUM_SHIP_VERTS, shipVertexArray1, shipScale, 0.f, Vec2( centerX - shipOffsetX, centerY ) );
	TransformVertexArrayXY3D( PlayerShip::NUM_SHIP_VERTS, shipVertexArray2, -shipScale, 0.f, Vec2( centerX + shipOffsetX, centerY ) );

	// Triangle scales and positions
	float triSize = SCREEN_SIZE_X * 0.05f;
	Vertex triangleVertexArray[3];
	triangleVertexArray[0].m_position = Vec3( centerX - triSize, centerY - triSize, 0.f );
	triangleVertexArray[1].m_position = Vec3( centerX - triSize, centerY + triSize, 0.f );
	triangleVertexArray[2].m_position = Vec3( centerX + triSize, centerY, 0.f );
	float alpha = 0.5f + 0.5f * SinDegrees( ( float ) GetCurrentTimeSeconds() * 180.f );
	Rgba8 triangleColor = Rgba8( 0, 153, 0, ( unsigned char ) ( alpha * 255.f ) );
	triangleVertexArray[0].m_color = triangleColor;
	triangleVertexArray[1].m_color = triangleColor;
	triangleVertexArray[2].m_color = triangleColor;

	g_engine->m_renderer->DrawVertexArray( PlayerShip::NUM_SHIP_VERTS, shipVertexArray1 );
	g_engine->m_renderer->DrawVertexArray( PlayerShip::NUM_SHIP_VERTS, shipVertexArray2 );
	g_engine->m_renderer->DrawVertexArray( 3, triangleVertexArray );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
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
	for ( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++ beetleIndex )
	{
		if ( m_beetles[beetleIndex] != nullptr )
		{
			m_beetles[beetleIndex]->Render();
		}
	}

	// Wasps
	for ( int waspIndex = 0; waspIndex < MAX_WASPS; ++ waspIndex )
	{
		if ( m_wasps[waspIndex] != nullptr )
		{
			m_wasps[waspIndex]->Render();
		}
	}

	// Debris
	for ( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++ debrisIndex )
	{
		if ( m_debris[debrisIndex] != nullptr )
		{
			m_debris[debrisIndex]->Render();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::RenderHUD() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	float spacing = SCREEN_SIZE_X * 0.025f;
	float rotation = 90.f;
	float scale = SCREEN_SIZE_X * 0.005f;
	float startX = spacing;
	float startY = SCREEN_SIZE_Y - spacing;

	for ( int lifeIndex = 0; lifeIndex < m_playerSpareLives; ++lifeIndex )
	{
		Vertex shipVertexArray[PlayerShip::NUM_SHIP_VERTS];
		m_playerShip->GetVertexArrayCopy( shipVertexArray );

		for ( int vertIndex = 0; vertIndex < PlayerShip::NUM_SHIP_VERTS; ++vertIndex )
		{
			shipVertexArray[vertIndex].m_color.a = 204;
		}

		Vec2 position = Vec2( startX + lifeIndex * spacing, startY );
		TransformVertexArrayXY3D( PlayerShip::NUM_SHIP_VERTS, shipVertexArray, scale, rotation, position );
		g_engine->m_renderer->DrawVertexArray( PlayerShip::NUM_SHIP_VERTS, shipVertexArray );
	}

	g_engine->m_renderer->EndCamera( *m_screenCamera );
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

			SoundID shootSound = g_engine->m_audioSystem->CreateOrGetSound( "Data/PlayershipShoot.wav" );
			g_engine->m_audioSystem->StartSound( shootSound );

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


//-----------------------------------------------------------------------------------------------
void Game::SpawnDebrisCluster( int numOfDebris, Vec2 const& position, Vec2 momentum, Rgba8 const& color, float minRadius, float maxRadius )
{
	for ( int debrisSpawned = 0; debrisSpawned < numOfDebris; ++debrisSpawned )
	{
		bool hasFreeSlot = false;
		for ( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex )
		{
			if ( m_debris[debrisIndex] == nullptr )
			{
				hasFreeSlot = true;
				RandomNumberGenerator rng;
				Vec2 randomDirection = Vec2::MakeFromPolarDegrees( rng.RollRandomFloatInRange( 0.f, 360.f ), 1.f );
				Vec2 randomVelocity = randomDirection * rng.RollRandomFloatInRange( 5.f, 15.f ) + momentum;
				float randomAngularVelocity = rng.RollRandomFloatInRange( -360.f, 360.f );

				m_debris[debrisIndex] = new Debris( this, position, randomVelocity, randomAngularVelocity, color, minRadius, maxRadius );
				break;
			}
		}
		if ( !hasFreeSlot )
		{
			ERROR_RECOVERABLE( "No available debris slots!" );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::KillAllEnemies()
{
	// Asteroids
	for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++ asteroidIndex )
	{
		if ( m_asteroids[asteroidIndex] != nullptr && m_asteroids[asteroidIndex]->IsAlive() )
		{
			m_asteroids[asteroidIndex]->Die();
		}
	}
	// Beetles
	for ( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++ beetleIndex )
	{
		if ( m_beetles[beetleIndex] != nullptr && m_beetles[beetleIndex]->IsAlive() )
		{
			m_beetles[beetleIndex]->Die();
		}
	}
	// Wasps
	for ( int waspIndex = 0; waspIndex < MAX_WASPS; ++ waspIndex )
	{
		if ( m_wasps[waspIndex] != nullptr && m_wasps[waspIndex]->IsAlive() )
		{
			m_wasps[waspIndex]->Die();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::CheckPlayerLives()
{
	static float timeOfDeath = 0.f;

	if ( m_playerShip == nullptr || !m_playerShip->m_isDead )
		return;

	if ( m_playerSpareLives == 0 && !m_playerShip->IsAlive() )
	{
		if ( timeOfDeath == 0.f )
		{
			timeOfDeath = ( float ) GetCurrentTimeSeconds();
		}

		// Return to attract mode after 3 seconds
		if ( ( float ) GetCurrentTimeSeconds() - timeOfDeath >= 3.f )
		{
			m_isAttractMode = true;
			m_waveNumber = 0;
			m_playerSpareLives = NUM_PLAYER_LIVES - 1;
			timeOfDeath = 0.f;

			SoundID gameOverSound = g_engine->m_audioSystem->CreateOrGetSound( "Data/Braam - Retro Pulse.wav" );
			g_engine->m_audioSystem->StartSound( gameOverSound, false, 0.7f, 0.f, 0.8f );
		}
	}
	else
	{
		timeOfDeath = 0.f;
	}
}


//-----------------------------------------------------------------------------------------------
void Game::Reset()
{
	m_waveNumber = 0;
	m_playerSpareLives = NUM_PLAYER_LIVES - 1;

	// Delete player ship and recreate
	delete m_playerShip;
	m_playerShip = new PlayerShip( this, Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ), Vec2( 0.f, 0.f ) );

	// Delete all entities
	for ( int i = 0; i < MAX_BULLETS; ++i ) {
		delete m_bullets[i];
		m_bullets[i] = nullptr;
	}
	for ( int i = 0; i < MAX_ASTEROIDS; ++i ) {
		delete m_asteroids[i];
		m_asteroids[i] = nullptr;
	}
	for ( int i = 0; i < MAX_BEETLES; ++i ) {
		delete m_beetles[i];
		m_beetles[i] = nullptr;
	}
	for ( int i = 0; i < MAX_WASPS; ++i ) {
		delete m_wasps[i];
		m_wasps[i] = nullptr;
	}
	for ( int i = 0; i < MAX_DEBRIS; ++i ) {
		delete m_debris[i];
		m_debris[i] = nullptr;
	}
}