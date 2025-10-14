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
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/StringUtils.hpp"
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

	m_currentGameState = ATTRACT_MODE;

	InitializeTargersArray( m_scanTargets, MAX_TARGETS );
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
void Game::InitializeTargersArray(Entity** out_targetsArray, int maxTargets)
{
	// Example implementation: set all pointers to nullptr
    for (int i = 0; i < maxTargets; ++i) {
        out_targetsArray[i] = nullptr;
    }
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
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		UpdateAttractMode( deltaSeconds );
		return;
	};

	if ( m_currentGameState == GameState::PAUSED )
	{
		UpdateFromKeyboard();
		UpdateFromController();
		return;
	};

	if ( m_isScanModeOn )
	{
		printf( "SCAN MODE ON\n" );
		deltaSeconds *= 0.1f;
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
			float t = shakeElapsedTime / m_screenShakeDuration;
			float currentIntensity = m_screenShakeIntensity * ( 1.f - t );
			ScreenShake( currentIntensity );
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
	
	UpdateScanMode( deltaSeconds );

	if ( m_isPausedAfterNextUpdate )
	{
		m_currentGameState = GameState::PAUSED;
		m_isPausedAfterNextUpdate = false;
	}
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateScanMode( [[maybe_unused]] float deltaSeconds )
{
	BuildScanTargets();
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
			m_currentGameState = GameState::VICTORY;
			m_waveNumber = 0;

			SoundID gameWinSound = g_engine->m_audioSystem->CreateOrGetSound( "Data/Braam - Zone End.wav" );
			g_engine->m_audioSystem->StartSound( gameWinSound, false, 0.7f, 0.f, 0.8f );

			m_currentGameState = GameState::ATTRACT_MODE;

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
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		// Start the game
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) || g_engine->m_inputSystem->WasKeyJustPressed( 'N' ) )
		{
			Reset();
			m_currentGameState = GameState::PLAYING;
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
			m_currentGameState = GameState::ATTRACT_MODE;
			m_isScanModeOn = false;
		}

		// Pause and resume the game
		if ( m_currentGameState != GameState::PAUSED && g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
		{
			m_currentGameState = GameState::PAUSED;
		}

		else if ( m_currentGameState == GameState::PAUSED && g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
		{
			m_currentGameState = GameState::PLAYING;
		}

		if ( m_isScanModeOn && g_engine->m_inputSystem->WasKeyJustPressed( 'F' ) )
		{
			// Perform scan action on the currently selected target
			if ( m_currentSelectedEntityIndex >= 0 && m_currentSelectedEntityIndex < MAX_TARGETS )
			{
				Entity* target = m_scanTargets[m_currentSelectedEntityIndex];
				if ( target != nullptr && target->IsAlive() )
				{
					if ( target->m_isAsteroid )
					{
						if ( m_playerShip->TrySpendEnergy( m_playerShip->m_costDetonate ) )
						{
							target->TakeDamage( target->m_health );
						}
					}
					else if ( target->m_isBeetle )
					{
						if ( m_playerShip->TrySpendEnergy( m_playerShip->m_costTelefrag ) )
						{
							Vec2 targetPosition = target->m_position;
							target->TakeDamage( target->m_health );
							m_playerShip->m_position = targetPosition;
						}
					}
					else if ( target->m_isWasp )
					{
						if ( m_playerShip->TrySpendEnergy( m_playerShip->m_costTelefrag ) )
						{
							Vec2 targetPosition = target->m_position;
							target->TakeDamage( target->m_health );
							m_playerShip->m_position = targetPosition;
						}
					}
					BuildScanTargets();
					m_currentSelectedEntityIndex = GetEnemyClosestToPlayer();
				}
			}
		}

		// Toggle debug features
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F1 ) )
		{
			m_isDebugFeaturesOn = !m_isDebugFeaturesOn;
		}

		// Pause the game after the next update (for debugging)
		if ( m_isDebugFeaturesOn && g_engine->m_inputSystem->WasKeyJustPressed( 'O' ) )
		{
			m_currentGameState = GameState::PLAYING;
			m_isPausedAfterNextUpdate = true;
		}

		// Spawn an asteroid (for debugging)
		if ( m_isDebugFeaturesOn && g_engine->m_inputSystem->WasKeyJustPressed( 'I' ) )
		{
			SpawnRandomAsteroids( 1 );
		}

		// Kill all enemies (for debugging)
		if ( m_isDebugFeaturesOn && g_engine->m_inputSystem->WasKeyJustPressed( 'K' ) )
		{
			KillAllEnemies();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateFromController()
{
	XboxController const& controller = g_engine->m_inputSystem->GetController( 0 );

	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		// Start the game
		if ( controller.WasButtonJustPressed( XBOX_BUTTON_A ) || controller.WasButtonJustPressed( XBOX_BUTTON_START ) )
		{
			Reset();
			m_currentGameState = GameState::PLAYING;
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
			m_currentGameState = GameState::ATTRACT_MODE;
			m_isScanModeOn = false;
		}

		if ( m_isScanModeOn && controller.WasButtonJustPressed( XBOX_BUTTON_RIGHT_SHOULDER ) )
		{
			// Perform scan action on the currently selected target
			if ( m_currentSelectedEntityIndex >= 0 && m_currentSelectedEntityIndex < MAX_TARGETS )
			{
				Entity* target = m_scanTargets[m_currentSelectedEntityIndex];
				if ( target != nullptr && target->IsAlive() )
				{
					if ( target->m_isAsteroid )
					{
						if ( m_playerShip->TrySpendEnergy( m_playerShip->m_costDetonate ) )
						{
							target->TakeDamage( target->m_health );
						}
					}
					else if ( target->m_isBeetle )
					{
						if ( m_playerShip->TrySpendEnergy( m_playerShip->m_costTelefrag ) )
						{
							Vec2 targetPosition = target->m_position;
							target->TakeDamage( target->m_health );
							m_playerShip->m_position = targetPosition;
						}
					}
					else if ( target->m_isWasp )
					{
						if ( m_playerShip->TrySpendEnergy( m_playerShip->m_costTelefrag ) )
						{
							Vec2 targetPosition = target->m_position;
							target->TakeDamage( target->m_health );
							m_playerShip->m_position = targetPosition;
						}
					}
					BuildScanTargets();
					m_currentSelectedEntityIndex = GetEnemyClosestToPlayer();
				}
			}
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
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

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

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		RenderAttractMode();
		return;
	};

	g_engine->m_renderer->BeginCamera( *m_worldCamera );
	
	RenderParallaxBackground();
	RenderEntities();

	if ( m_isScanModeOn )
	{
		RenderScanMode();
	}

	RenderHUD();

	if ( m_isDebugFeaturesOn )
	{
		DebugDraw();
	}

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	// Colors
	const Rgba8 brightYellow = Rgba8( 253, 239, 3 );
	const Rgba8 brightCyan = Rgba8( 87, 231, 239 );
	const Rgba8 dimCyan = Rgba8( 87, 231, 239, 80 );
	const Rgba8 dimYellow = Rgba8( 253, 239, 3, 80 );
	const Rgba8 neonPink = Rgba8( 248, 21, 98 );

	float flicker = 0.9f + 0.1f * SinDegrees( ( float ) GetCurrentTimeSeconds() * 720.f );
	Rgba8 flickerCol = Rgba8(
		( unsigned char ) ( brightCyan.r * flicker ),
		( unsigned char ) ( brightCyan.g * flicker ),
		( unsigned char ) ( brightCyan.b * flicker )
	);

	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	const float currentTime = ( float ) GetCurrentTimeSeconds();

	// Ships
	const float shipScale = SCREEN_SIZE_X * 0.04f;
	const float shipOffsetX = SCREEN_SIZE_X * 0.2f;
	const float centerX = SCREEN_SIZE_X * 0.5f;
	const float centerY = SCREEN_SIZE_Y * 0.5f;

	const float wobbleRot = 5.f * SinDegrees( currentTime * 45.f );
	const float bob = 4.f * SinDegrees( currentTime * 60.f );

	Vertex shipVertexArray1[PlayerShip::NUM_SHIP_VERTS];
	Vertex shipVertexArray2[PlayerShip::NUM_SHIP_VERTS];
	m_playerShip->GetVertexArrayCopy( shipVertexArray1 );
	m_playerShip->GetVertexArrayCopy( shipVertexArray2 );

	TransformVertexArrayXY3D(
		PlayerShip::NUM_SHIP_VERTS, shipVertexArray1, shipScale,
		wobbleRot, Vec2( centerX - shipOffsetX, centerY + bob ) );

	TransformVertexArrayXY3D(
		PlayerShip::NUM_SHIP_VERTS, shipVertexArray2, -shipScale,
		-wobbleRot, Vec2( centerX + shipOffsetX, centerY - bob ) );

	// Play triangle
	float triBase = SCREEN_SIZE_X * 0.05f;
	float triPulse = 1.f + 0.06f * SinDegrees( currentTime * 180.f );
	float triSize = triBase * triPulse;

	Vertex triangleVertexArray[3];
	triangleVertexArray[0].m_position = Vec3( centerX - triSize, centerY - triSize, 0.f );
	triangleVertexArray[1].m_position = Vec3( centerX - triSize, centerY + triSize, 0.f );
	triangleVertexArray[2].m_position = Vec3( centerX + triSize, centerY, 0.f );

	float triAlpha = 0.55f + 0.45f * SinDegrees( currentTime * 180.f );
	Rgba8 triangleColor = Rgba8( 0, 153, 0, ( unsigned char ) ( triAlpha * 255.f ) );
	triangleVertexArray[0].m_color = triangleColor;
	triangleVertexArray[1].m_color = triangleColor;
	triangleVertexArray[2].m_color = triangleColor;

	// Text
	std::vector<Vertex> textVerts;
	textVerts.reserve( 4096 );

	float starshipStartX = SCREEN_SIZE_X / 2.f - 250.f;
	float starshipStartY = 600.f;
	float goldStartX = SCREEN_SIZE_X / 2.f + 60.f;
	float goldStartY = 540.f;
	float glowOffset = 4.f;
	float jitterX = SinDegrees( currentTime * 720.f );
	AddVertsForTextTriangles2D( textVerts, "Starship", Vec2( starshipStartX + jitterX + glowOffset, starshipStartY + glowOffset ), 80.f, dimCyan );
	AddVertsForTextTriangles2D( textVerts, "Starship", Vec2( starshipStartX + jitterX, starshipStartY), 80.f, flickerCol );
	AddVertsForTextTriangles2D( textVerts, "Gold", Vec2( goldStartX + glowOffset, goldStartY + glowOffset ), 60.f, dimYellow );
	AddVertsForTextTriangles2D( textVerts, "Gold", Vec2( goldStartX, goldStartY ), 60.f, brightYellow );

	float promptStartX = SCREEN_SIZE_X / 2.f - 270.f;
	float promptStartY = 150.f;
	float promptAlpha = 0.35f + 0.65f * ( 0.5f * ( 1.f + SinDegrees( currentTime * 240.f ) ) );
	Rgba8 promptCol = Rgba8( neonPink.r, neonPink.g, neonPink.b, ( unsigned char ) ( promptAlpha * 255.f ) );
	AddVertsForTextTriangles2D( textVerts, "Press Start or A to Play", Vec2( promptStartX, promptStartY ), 30.f, promptCol );


	// Render 
	RenderParallaxBackground();
	g_engine->m_renderer->DrawVertexArray( PlayerShip::NUM_SHIP_VERTS, shipVertexArray2 );
	g_engine->m_renderer->DrawVertexArray( PlayerShip::NUM_SHIP_VERTS, shipVertexArray1 );
	g_engine->m_renderer->DrawVertexArray( 3, triangleVertexArray );
	g_engine->m_renderer->DrawVertexArray( ( int ) textVerts.size(), textVerts.data() );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderParallaxBackground() const 
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	const int   numOfColumns = 20;
	const int   tailLength = 12;
	const float glyphCellHeight = 18.0f;
	const float columnSpacingPixels = ( float ) SCREEN_SIZE_X / ( float ) numOfColumns;
	const float minSpeed = 50.0f;
	const float maxSpeed = 110.0f;

	const Rgba8 headColor = Rgba8( 140, 255, 200, 200 );
	const Rgba8 tailColor = Rgba8( 60, 200, 170, 150 );

	static const char* const glyphSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const int glyphSetLength = 36;

	static bool  isInitialised = false;
	static float columnHeadY[numOfColumns];
	static float columnSpeed[numOfColumns];
	static float lastUpdateTime = 0.0f;
	
	RandomNumberGenerator rng;

	if ( !isInitialised )
	{
		for ( int columnIndex = 0; columnIndex < numOfColumns; ++columnIndex )
		{
			columnSpeed[columnIndex] = rng.RollRandomFloatInRange( minSpeed, maxSpeed );

			float randomStartRatio = rng.RollRandomFloatZeroToOne();
			float maxStartY = ( float ) SCREEN_SIZE_Y + ( float ) tailLength * glyphCellHeight;
			columnHeadY[columnIndex] = randomStartRatio * maxStartY;
		}

		lastUpdateTime = ( float ) GetCurrentTimeSeconds();
		isInitialised = true;
	}

	float currentTime = ( float ) GetCurrentTimeSeconds();
	float deltaSeconds = currentTime - lastUpdateTime;
	if ( deltaSeconds < 0.0f ) deltaSeconds = 0.0f;
	if ( deltaSeconds > 0.1f ) deltaSeconds = 0.1f;
	lastUpdateTime = currentTime;

	std::vector<Vertex> textVertices;
	textVertices.reserve( numOfColumns * tailLength * 12 );

	for ( int columnIndex = 0; columnIndex < numOfColumns; ++ columnIndex )
	{
		columnHeadY[columnIndex] -= columnSpeed[columnIndex] * deltaSeconds;

		float minY = -glyphCellHeight * ( float ) tailLength;
		float maxY = ( float ) SCREEN_SIZE_Y + glyphCellHeight * ( float ) tailLength;

		if ( columnHeadY[columnIndex] < minY )
		{
			columnSpeed[columnIndex] = rng.RollRandomFloatInRange( minSpeed, maxSpeed );
			columnHeadY[columnIndex] = maxY;
		}

		float columnCenterX = ( columnIndex + 0.5f ) * columnSpacingPixels;
		float glyphWidth = glyphCellHeight * 0.6f;
		float glyphStartX = columnCenterX - glyphWidth * 0.5f;

		for ( int tailIndex = 0; tailIndex < tailLength; ++tailIndex )
		{
			float glyphYPosition = columnHeadY[columnIndex] - ( float ) tailIndex * glyphCellHeight;
			if ( glyphYPosition < -glyphCellHeight || glyphYPosition >( float )SCREEN_SIZE_Y + glyphCellHeight )
			{
				continue;
			}

			int frameTicker = ( int ) ( currentTime * 10.0f );
			int glyphIndex = ( columnIndex * 131 + tailIndex * 17 + frameTicker ) % glyphSetLength;
			if ( glyphIndex < 0 )
			{
				glyphIndex += glyphSetLength;
			}

			char glyphText[2];
			glyphText[0] = glyphSet[glyphIndex];
			glyphText[1] = '\0';

			Rgba8 glyphColour;
			if ( tailIndex == 0 )
			{
				glyphColour = headColor;
			}
			else
			{
				float t = ( float ) tailIndex / ( float ) tailLength;
				glyphColour.r = ( unsigned char ) Interpolate( ( float ) headColor.r, ( float ) tailColor.r, t );
				glyphColour.g = ( unsigned char ) Interpolate( ( float ) headColor.g, ( float ) tailColor.g, t );
				glyphColour.b = ( unsigned char ) Interpolate( ( float ) headColor.b, ( float ) tailColor.b, t );
				glyphColour.a = ( unsigned char ) Interpolate( ( float ) headColor.a, ( float ) tailColor.a, t );
			}
			if ( tailIndex > 0 )
			{
				float fadeRatio = 1.0f - ( ( float ) tailIndex / ( float ) tailLength );
				glyphColour.a = ( unsigned char ) ( ( float ) glyphColour.a * fadeRatio );
			}

			AddVertsForTextTriangles2D( textVertices, glyphText, Vec2( glyphStartX, glyphYPosition ), glyphCellHeight, glyphColour );
		}
	}

	if ( !textVertices.empty() )
	{
		g_engine->m_renderer->DrawVertexArray( ( int ) textVertices.size(), textVertices.data() );
	}

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderEntities() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

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

	// Player ship
	m_playerShip->Render();

	g_engine->m_renderer->EndCamera( *m_worldCamera );
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

	const float barX = 24.f;
	const float barY = ( float ) SCREEN_SIZE_Y - 120.f;
	const float barWidth = 260.f;
	const float barHeight = 18.f;

	const float energyRatio = m_playerShip->GetEnergyFraction();
	const float energyVal = m_playerShip->m_currentEnergy;
	const float energyMax = m_playerShip->m_maxEnergy;
	const float detCost = m_playerShip->m_costDetonate;
	const float telCost = m_playerShip->m_costTelefrag;

	Rgba8 bgColour = Rgba8( 20, 24, 32, 200 );
	Rgba8 borderColour = Rgba8( 0, 255, 220, 180 );
	Rgba8 fillColour = Rgba8( 253, 239, 3, 230 );
	Rgba8 lowColour = Rgba8( 255, 60, 180, 230 );

	bool belowBothCost = ( energyVal < detCost ) && ( energyVal < telCost );
	if ( belowBothCost )
	{
		fillColour = lowColour;
	}

	Vertex bg[6];
	bg[0] = Vertex( Vec3( barX, barY, 0.f ), bgColour, Vec2( 0, 0) );
	bg[1] = Vertex( Vec3( barX + barWidth, barY, 0.f ), bgColour, Vec2( 0, 0 ) );
	bg[2] = Vertex( Vec3( barX + barWidth, barY + barHeight, 0.f ), bgColour, Vec2( 0, 0 ) );
	bg[3] = Vertex( Vec3( barX, barY, 0.f ), bgColour, Vec2( 0, 0 ) );
	bg[4] = Vertex( Vec3( barX + barWidth, barY + barHeight, 0.f ), bgColour, Vec2( 0, 0 ) );
	bg[5] = Vertex( Vec3( barX, barY + barHeight, 0.f ), bgColour, Vec2( 0, 0 ) );
	g_engine->m_renderer->DrawVertexArray( 6, bg );

	float fillW = barWidth * energyRatio;
	Vertex fill[6];
	fill[0] = Vertex( Vec3( barX, barY, 0.f ), fillColour, Vec2( 0, 0 ) );
	fill[1] = Vertex( Vec3( barX + fillW, barY, 0.f ), fillColour, Vec2( 0, 0 ) );
	fill[2] = Vertex( Vec3( barX + fillW, barY + barHeight, 0.f ), fillColour, Vec2( 0, 0 ) );
	fill[3] = Vertex( Vec3( barX, barY, 0.f ), fillColour, Vec2( 0, 0 ) );
	fill[4] = Vertex( Vec3( barX + fillW, barY + barHeight, 0.f ), fillColour, Vec2( 0, 0 ) );
	fill[5] = Vertex( Vec3( barX, barY + barHeight, 0.f ), fillColour, Vec2( 0, 0 ) );
	g_engine->m_renderer->DrawVertexArray( 6, fill );

	DebugDrawLine( Vec2( barX, barY ), Vec2( barX + barWidth, barY ), 4.f, borderColour, borderColour );
	DebugDrawLine( Vec2( barX + barWidth, barY ), Vec2( barX + barWidth, barY + barHeight ), 4.f, borderColour, borderColour );
	DebugDrawLine( Vec2( barX + barWidth, barY + barHeight ), Vec2( barX, barY + barHeight ), 4.f, borderColour, borderColour );
	DebugDrawLine( Vec2( barX, barY + barHeight ), Vec2( barX, barY ), 4.f, borderColour, borderColour );

	Rgba8 tickColour = Rgba8( 180, 230, 255, 200 );
	float detX = barX + barWidth * ( detCost / energyMax );
	float telX = barX + barWidth * ( telCost / energyMax );
	DebugDrawLine( Vec2( detX, barY - 3.f ), Vec2( detX, barY + barHeight + 3.f ), 4.f, tickColour, tickColour );
	DebugDrawLine( Vec2( telX, barY - 3.f ), Vec2( telX, barY + barHeight + 3.f ), 4.f, tickColour, tickColour );

	std::vector<Vertex> textVerts;
	std::string labelEnergy = Stringf( "ENERGY: %d / %d", ( int ) energyVal, ( int ) energyMax );
	AddVertsForTextTriangles2D( textVerts, labelEnergy, Vec2( barX + 1.5f, barY + 30.f + 1.5f ), 16.f, Rgba8( 253, 239, 3, 60 ) );
	AddVertsForTextTriangles2D( textVerts, labelEnergy, Vec2( barX, barY + 30.f ), 16.f, Rgba8( 253, 239, 3, 230 ) );

	g_engine->m_renderer->DrawVertexArray( ( int ) textVerts.size(), textVerts.data() );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderScanMode() const
{	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	// Semi-transparent green overlay
	Rgba8 overlayColor = Rgba8( 0, 255, 0, 30 );
	Vertex overlayVerts[6];
	overlayVerts[0].m_position = Vec3( 0.f, 0.f, 0.f );
	overlayVerts[0].m_color = overlayColor;
	overlayVerts[1].m_position = Vec3( ( float ) SCREEN_SIZE_X, 0.f, 0.f );
	overlayVerts[1].m_color = overlayColor;
	overlayVerts[2].m_position = Vec3( ( float ) SCREEN_SIZE_X, ( float ) SCREEN_SIZE_Y, 0.f );
	overlayVerts[2].m_color = overlayColor;
	overlayVerts[3].m_position = Vec3( 0.f, 0.f, 0.f );
	overlayVerts[3].m_color = overlayColor;
	overlayVerts[4].m_position = Vec3( ( float ) SCREEN_SIZE_X, ( float ) SCREEN_SIZE_Y, 0.f );
	overlayVerts[4].m_color = overlayColor;
	overlayVerts[5].m_position = Vec3( 0.f, ( float ) SCREEN_SIZE_Y, 0.f );
	overlayVerts[5].m_color = overlayColor;
	g_engine->m_renderer->DrawVertexArray( 6, overlayVerts );

	// Highlight selected enemy
	Entity* m_currentSelectedEntity = nullptr;
	if ( m_currentSelectedEntityIndex >= 0 && m_currentSelectedEntityIndex < MAX_TARGETS ) {
		m_currentSelectedEntity = m_scanTargets[m_currentSelectedEntityIndex];
	}
	if ( m_currentSelectedEntity != nullptr && m_currentSelectedEntity->IsAlive() ) {
		// Convert world position to screen position
		Vec3 worldPos = Vec3( m_currentSelectedEntity->m_position.x, m_currentSelectedEntity->m_position.y, 0.f );
		Vec3 screenPos = TransformWorldToScreen( worldPos );
		float highlightRadius = m_currentSelectedEntity->m_cosmeticRadius * 1.5f * ( ( float ) SCREEN_SIZE_X / ( float ) WORLD_SIZE_X );
		DebugDrawRing( Vec2( screenPos.x, screenPos.y ), highlightRadius, 1.5f, Rgba8( 0, 255, 0 ) );

		Vec3 panelPos;
		const float panelWidth = 260.f;
		const float panelHeight = 130.f;
		if ( screenPos.x < SCREEN_SIZE_X * 0.5f ) // Left side of screen
		{
			panelPos.x = screenPos.x + highlightRadius + 10.f;
		}
		else // Right side of screen
		{
			panelPos.x = screenPos.x - highlightRadius - 10.f - panelWidth;
		}
		panelPos.y = screenPos.y - panelHeight * 0.5f;

		char* entityTypeString = nullptr;
		char* entityActionString = nullptr;

		if ( m_currentSelectedEntity )
		{
			m_currentSelectedEntity->GetEnemyTypeAndAction(
				&entityTypeString,
				&entityActionString );
		}

		float distanceToPlayer = GetDistance2D( m_playerShip->m_position, m_currentSelectedEntity->m_position );
		float speed = m_currentSelectedEntity->m_velocity.GetLength();

		std::string positionString = Stringf( "Position: (%.2f, %.2f)", m_currentSelectedEntity->m_position.x, m_currentSelectedEntity->m_position.y );
		std::string speedString = Stringf( "Speed: %.2f units/s", speed );
		std::string distanceString = Stringf( "Distance: %.2f units", distanceToPlayer );
		std::string healthString = Stringf( "Health: %d", m_currentSelectedEntity->m_health );
		std::string typeString = Stringf( "Type: %s", entityTypeString );
		std::string actionString = Stringf( "Action: %s", entityActionString );

		std::vector<Vertex> textVerts;
		AddVertsForTextTriangles2D( textVerts, actionString, Vec2( panelPos.x + 10.f, panelPos.y + 110.f ), 14.f, Rgba8( 255, 255, 0 ) );
		AddVertsForTextTriangles2D( textVerts, typeString, Vec2( panelPos.x + 10.f, panelPos.y + 90.f ), 14.f, Rgba8( 255, 255, 255 ) );
		AddVertsForTextTriangles2D( textVerts, healthString, Vec2( panelPos.x + 10.f, panelPos.y + 70.f ), 14.f, Rgba8( 255, 255, 255 ) );
		AddVertsForTextTriangles2D( textVerts, distanceString, Vec2( panelPos.x + 10.f, panelPos.y + 50.f ), 14.f, Rgba8( 255, 255, 255 ) );
		AddVertsForTextTriangles2D( textVerts, speedString, Vec2( panelPos.x + 10.f, panelPos.y + 30.f ), 14.f, Rgba8( 255, 255, 255 ) );
		AddVertsForTextTriangles2D( textVerts, positionString, Vec2( panelPos.x + 10.f, panelPos.y + 10.f ), 14.f, Rgba8( 255, 255, 255 ) );
		g_engine->m_renderer->DrawVertexArray( ( int ) textVerts.size(), textVerts.data() );
	}

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
Vec3 Game::TransformWorldToScreen( Vec3 const& worldPosition ) const
{
	Vec2 worldBottomLeft = m_worldCamera->GetOrthoBottomLeft();
	Vec2 worldTopRight = m_worldCamera->GetOrthoTopRight();

	Vec2 screenBottomLeft = m_screenCamera->GetOrthoBottomLeft();
	Vec2 screenTopRight = m_screenCamera->GetOrthoTopRight();

	float worldWidth = worldTopRight.x - worldBottomLeft.x;
	float worldHeight = worldTopRight.y - worldBottomLeft.y;
	float normX = ( worldPosition.x - worldBottomLeft.x ) / worldWidth;
	float normY = ( worldPosition.y - worldBottomLeft.y ) / worldHeight;

	float screenWidth = screenTopRight.x - screenBottomLeft.x;
	float screenHeight = screenTopRight.y - screenBottomLeft.y;
	float screenX = screenBottomLeft.x + normX * screenWidth;
	float screenY = screenBottomLeft.y + normY * screenHeight;

	return Vec3( screenX, screenY, worldPosition.z );
}


//-----------------------------------------------------------------------------------------------
int Game::BuildScanTargets()
{
	int m_numScanTargets = 0;

	// Asteroids
	for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex )
	{
		if ( m_asteroids[asteroidIndex] != nullptr && !m_asteroids[asteroidIndex]->m_isDead )
		{
			m_scanTargets[m_numScanTargets] = m_asteroids[asteroidIndex];
			++m_numScanTargets;
			if ( m_numScanTargets >= MAX_TARGETS )
				return m_numScanTargets;
		}
	}

	// Beetles
	for ( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex )
	{
		if ( m_beetles[beetleIndex] != nullptr && !m_beetles[beetleIndex]->m_isDead )
		{
			m_scanTargets[m_numScanTargets] = m_beetles[beetleIndex];
			++m_numScanTargets;
			if ( m_numScanTargets >= MAX_TARGETS )
				return m_numScanTargets;
		}
	}

	// Wasps
	for ( int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex )
	{
		if ( m_wasps[waspIndex] != nullptr && !m_wasps[waspIndex]->m_isDead )
		{
			m_scanTargets[m_numScanTargets] = m_wasps[waspIndex];
			++m_numScanTargets;
			if ( m_numScanTargets >= MAX_TARGETS )
				return m_numScanTargets;
		}
	}

	return m_numScanTargets;
}



//-----------------------------------------------------------------------------------------------
int Game::GetEnemyClosestToPlayer() const
{
	if ( m_playerShip == nullptr || m_playerShip->m_isDead )
		return -1;

	int closestTargetIndex = -1;
	float closestDistSq = 0.0f;

	for ( int targetIndex = 0; targetIndex < MAX_TARGETS; ++targetIndex )
	{
		Entity* target = m_scanTargets[targetIndex];
		if ( target == nullptr || target->m_isDead )
			continue;

		float distSq = GetDistanceSquared2D( m_playerShip->m_position, target->m_position );

		if ( closestTargetIndex == -1 || distSq < closestDistSq )
		{
			closestTargetIndex = targetIndex;
			closestDistSq = distSq;
		}
	}

	return closestTargetIndex;
}


//-----------------------------------------------------------------------------------------------
int Game::StepCurrentSelectedEntityIndex( int currentIndex, int step )
{
	if ( !m_isScanModeOn || m_playerShip == nullptr || currentIndex < 0 )
		return currentIndex;

	int numOfTargets = BuildScanTargets();
	if ( numOfTargets <= 0 || currentIndex >= numOfTargets )
		return -1;

	Entity* currentSelected = m_scanTargets[currentIndex];
	if ( currentSelected == nullptr || currentSelected->m_isDead )
		return -1;

	float baseAngle = ( currentSelected->m_position - m_playerShip->m_position ).GetOrientationDegrees();

	int   bestIndex = currentIndex;
	float bestMetric = 9999.f;
	bool  found = false;

	for ( int targetIndex = 0; targetIndex < numOfTargets; ++targetIndex )
	{
		if ( targetIndex == currentIndex ) continue;
		Entity* target = m_scanTargets[targetIndex];
		if ( target == nullptr || target->m_isDead ) continue;

		float targetAngle = ( target->m_position - m_playerShip->m_position ).GetOrientationDegrees();
		float delta = targetAngle - baseAngle;
		while ( delta <= -180.f ) delta += 360.f;
		while ( delta > 180.f ) delta -= 360.f;

		if ( step > 0 ) // Right = clockwise
		{
			if ( delta < 0.f )
			{
				float metric = -delta;
				if ( !found || metric < bestMetric )
				{
					found = true;
					bestMetric = metric;
					bestIndex = targetIndex;
				}
			}
		}
		else if ( step < 0 ) // Left = counter-clockwise
		{
			if ( delta > 0.f )
			{
				float metric = delta;
				if ( !found || metric < bestMetric )
				{
					found = true;
					bestMetric = metric;
					bestIndex = targetIndex;
				}
			}
		}
	}

	if ( !found )
	{
		float bestAbs = 9999.f;
		for ( int targetIndex = 0; targetIndex < numOfTargets; ++targetIndex )
		{
			if ( targetIndex == currentIndex ) continue;
			Entity* target = m_scanTargets[targetIndex];
			if ( target == nullptr || target->m_isDead ) continue;

			const float targetAngle = ( target->m_position - m_playerShip->m_position ).GetOrientationDegrees();
			float delta = targetAngle - baseAngle;
			while ( delta <= -180.f ) delta += 360.f;
			while ( delta > 180.f ) delta -= 360.f;
			const float ad = fabsf( delta );
			if ( ad < bestAbs )
			{
				bestAbs = ad;
				bestIndex = targetIndex;
			}
		}
	}

	m_currentSelectedEntityIndex = bestIndex;
	return m_currentSelectedEntityIndex;
}


//-----------------------------------------------------------------------------------------------
void Game::SpawnRandomAsteroids( int numOfAsteroid )
{
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
		m_currentGameState = GameState::GAME_OVER;

		if ( timeOfDeath == 0.f )
		{
			timeOfDeath = ( float ) GetCurrentTimeSeconds();
		}

		// Return to attract mode after 3 seconds
		if ( ( float ) GetCurrentTimeSeconds() - timeOfDeath >= 3.f )
		{
			m_currentGameState = GameState::ATTRACT_MODE;
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