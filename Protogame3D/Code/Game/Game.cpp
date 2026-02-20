#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_worldCamera = new Camera();
	m_screenCamera = new Camera();

	m_worldCamera->SetOrthoView( Vec2( -1.f, -1.f ), Vec2( 1.f, 1.f ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	m_currentGameState = ATTRACT_MODE;

	m_gameClock = new Clock();
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
void Game::DeleteGarbageEntities()
{
	if ( g_app->m_game == nullptr )
		return;

	// #ToDo: Delete entities that are marked as garbage
}


//-----------------------------------------------------------------------------------------------
void Game::Update()
{
	if ( !m_hasControlsBeenShown )
	{
		g_engine->m_devConsole->ToggleMode( DevConsoleMode::OPEN_FULL );
		AddInstructionsToDevConsole();
		m_hasControlsBeenShown = true;
	}

	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		UpdateAttractMode();
		return;
	};

	if ( m_currentGameState == GameState::PAUSED )
	{
		UpdateFromKeyboard();
		UpdateFromController();
		return;
	};

	UpdateFromKeyboard();
	UpdateFromController();
	UpdateEntities();

	g_app->m_game->DeleteGarbageEntities();

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	if ( m_isScreenShaking )
	{
		float shakeElapsedTime = ( float ) m_gameClock->GetTotalSeconds() - m_screenShakeStartTime;
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
void Game::UpdateAttractMode()
{
	UpdateFromKeyboard();
	UpdateFromController();
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateFromKeyboard()
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		// Start the game
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) || g_engine->m_inputSystem->WasKeyJustPressed( 'N' ) || g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) )
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
		}

		// Pause and resume the game
		if ( m_currentGameState != GameState::PAUSED && g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
		{
			m_currentGameState = GameState::PAUSED;
			m_gameClock->Pause();
		}

		else if ( m_currentGameState == GameState::PAUSED && g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
		{
			m_currentGameState = GameState::PLAYING;
			m_gameClock->Unpause();
		}

		// Slow the time scale to 1/10
		if ( g_engine->m_inputSystem->IsKeyDown( 'T' ) )
		{
			m_gameClock->SetTimeScale( 0.1 );
		}
		if ( g_engine->m_inputSystem->WasKeyJustReleased( 'T' ) )
		{
			m_gameClock->SetTimeScale( 1.0 );
		}

		// Toggle debug features
		if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F1 ) )
		{
			m_isDebugFeaturesOn = !m_isDebugFeaturesOn;
		}

		// Pause the game after the next update (for debugging)
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'O' ) )
		{
			m_currentGameState = GameState::PLAYING;
			m_gameClock->StepSingleFrame();
			m_currentGameState = GameState::PAUSED;
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
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateEntities()
{
	// #ToDo: Update all entities in the game world
}


//-----------------------------------------------------------------------------------------------
void Game::DebugDraw() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	// #ToDo: Draw debug information about entities, collisions, etc.

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	if ( m_currentGameState == GameState::ATTRACT_MODE )
	{
		RenderAttractMode();
		RenderDevConsole();
		return;
	};

	// Clear screen
	g_engine->m_renderer->ClearScreen( Rgba8::BLUE );

	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	RenderEntities();
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
	// Clear screen
	g_engine->m_renderer->ClearScreen( Rgba8::BLACK );

	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	// Colors
	const Rgba8 brightYellow = Rgba8( 253, 239, 3 );
	const Rgba8 brightCyan = Rgba8( 87, 231, 239 );
	const Rgba8 dimCyan = Rgba8( 87, 231, 239, 80 );
	const Rgba8 dimYellow = Rgba8( 253, 239, 3, 80 );
	const Rgba8 neonPink = Rgba8( 248, 21, 98 );

	float flicker = 0.9f + 0.1f * SinDegrees( ( float ) m_gameClock->GetTotalSeconds() * 720.f );
	Rgba8 flickerCol = Rgba8(
		( unsigned char ) ( brightCyan.r * flicker ),
		( unsigned char ) ( brightCyan.g * flicker ),
		( unsigned char ) ( brightCyan.b * flicker )
	);

	const float currentTime = ( float ) m_gameClock->GetTotalSeconds();
	const float centerX = SCREEN_SIZE_X * 0.5f;
	const float centerY = SCREEN_SIZE_Y * 0.5f;

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
	std::vector<Vertex> verts;
	AddVertsForTextTriangles2D( verts, "PROTOGAME2D", Vec2( 10.f, SCREEN_SIZE_Y - 30.f ), 24.f, Rgba8( 255, 255, 255 ) );

	// Texture
	Texture* testTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Image/Test_StbiFlippedAndOpenGL.png" );
	std::vector<Vertex> testTextureVerts;
	AABB2 texturedAABB2( 300.f, 100.f, 800.f, 600.f );
	AddVertsForAABB2D( testTextureVerts, texturedAABB2, Rgba8( 255, 255, 255, 255 ) ); // This should now set UVs on each Vertex!!
	g_engine->m_renderer->BindTexture( testTexture );
	g_engine->m_renderer->DrawVertexArray( testTextureVerts );

	std::vector<Vertex> ringVerts;
	AddVertsForRing2D( ringVerts, Vec2( SCREEN_CENTER_X, SCREEN_CENTER_Y ), 200.f, 20.f, Rgba8( 255, 0, 255 ), 32 );
	g_engine->m_renderer->BindTexture( nullptr ); // NOTE: We now have to do this before rendering anything UN-textured!
	g_engine->m_renderer->DrawVertexArray( ringVerts );

	// Render 
	g_engine->m_renderer->DrawVertexArray( 3, triangleVertexArray );
	g_engine->m_renderer->DrawVertexArray( ( int ) verts.size(), verts.data() );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderEntities() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	// #ToDo: Render all entities in the game world

	for ( Entity* entity : m_entities )
	{
		entity->Render();
	}

	Vertex squareVertexArray[6];
	float squareSize = 10.f;
	float time = ( float ) m_gameClock->GetTotalSeconds();
	float squareX = WORLD_SIZE_X * 0.5f + ( WORLD_SIZE_X * 0.1f ) * SinDegrees( time * 60.f );
	squareVertexArray[0].m_position = Vec3( squareX - squareSize, 50.f - squareSize, 0.f );
	squareVertexArray[1].m_position = Vec3( squareX + squareSize, 50.f - squareSize, 0.f );
	squareVertexArray[2].m_position = Vec3( squareX + squareSize, 50.f + squareSize, 0.f );
	squareVertexArray[3].m_position = Vec3( squareX - squareSize, 50.f - squareSize, 0.f );
	squareVertexArray[4].m_position = Vec3( squareX + squareSize, 50.f + squareSize, 0.f );
	squareVertexArray[5].m_position = Vec3( squareX - squareSize, 50.f + squareSize, 0.f );
	Rgba8 squareColor = Rgba8( 200, 50, 50 );
	squareVertexArray[0].m_color = squareColor;
	squareVertexArray[1].m_color = squareColor;
	squareVertexArray[2].m_color = squareColor;
	squareVertexArray[3].m_color = squareColor;
	squareVertexArray[4].m_color = squareColor;
	squareVertexArray[5].m_color = squareColor;

	g_engine->m_renderer->DrawVertexArray( 6, squareVertexArray );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderHUD() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	// #ToDo: Render HUD elements (health, score, etc.)

	std::vector<Vertex> verts;
	AddVertsForTextTriangles2D( verts, "GAME MODE", Vec2( 10.f, SCREEN_SIZE_Y - 30.f ), 24.f, Rgba8( 255, 255, 255 ) );

	g_engine->m_renderer->DrawVertexArray( ( int ) verts.size(), verts.data() );

	g_engine->m_renderer->EndCamera( *m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Game::RenderDevConsole() const
{
	g_engine->m_renderer->BeginCamera( *m_screenCamera );

	float screenWidth = g_gameConfigBlackboard.GetValue( "windowWidth", 1600.f );
	float screenHeight = g_gameConfigBlackboard.GetValue( "windowHeight", 800.f );
	AABB2 devConsoleBounds = AABB2( 0.f, 0.f, screenWidth, screenHeight );
	g_engine->m_devConsole->Render( devConsoleBounds );

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
bool Game::IsOnScreen( Vec2 const& worldPosition, float cosmeticRadius ) const
{
	Vec2 worldBottomLeft = m_worldCamera->GetOrthoBottomLeft();
	Vec2 worldTopRight = m_worldCamera->GetOrthoTopRight();
	if ( worldPosition.x + cosmeticRadius < worldBottomLeft.x ) return false;
	if ( worldPosition.x - cosmeticRadius > worldTopRight.x ) return false;
	if ( worldPosition.y + cosmeticRadius < worldBottomLeft.y ) return false;
	if ( worldPosition.y - cosmeticRadius > worldTopRight.y ) return false;
	return true;
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
void Game::KillAllEnemies()
{
	// #ToDo: Kill all enemy entities in the game world
}


//-----------------------------------------------------------------------------------------------
void Game::Reset()
{
	// #ToDo: Delete all entities and reset game state
	m_isScreenShaking = false;
	m_isDebugFeaturesOn = false;
}


//-----------------------------------------------------------------------------------------------
void Game::AddInstructionsToDevConsole() const
{
	g_engine->m_devConsole->AddLineWithoutTimestamp( DevConsole::INFO_MAJOR, "Keyboard" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( DevConsole::INFO_MINOR, "" );

	g_engine->m_devConsole->AddLineWithoutTimestamp( DevConsole::INFO_MINOR, "Space (Attract Mode):      Switch to Game Mode" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( DevConsole::INFO_MINOR, "P     (Attract Mode):      Switch to Game Mode" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( DevConsole::INFO_MINOR, "Esc   (Attract Mode):      Exit the game" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( DevConsole::INFO_MINOR, "" );

	g_engine->m_devConsole->AddLineWithoutTimestamp( DevConsole::INFO_MINOR, "P        (Game Mode):      Pause or resume" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( DevConsole::INFO_MINOR, "O        (Game Mode):      Advance one frame, then pause" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( DevConsole::INFO_MINOR, "T        (Game Mode):      Hold to slow time to 1/10th speed" );
	g_engine->m_devConsole->AddLineWithoutTimestamp( DevConsole::INFO_MINOR, "Esc      (Game Mode):      Switch to Attract Mode" );
}