#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/Game2DCurves.hpp"
#include "Game/Game3DShapes.hpp"
#include "Game/GameNearestPoint.hpp"
#include "Game/GamePachinko2D.hpp"
#include "Game/GameRaycastVsAABB.hpp"
#include "Game/GameRaycastVsDiscs.hpp"
#include "Game/GameRaycastVsLineSegment.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
App* g_app = nullptr;


//-----------------------------------------------------------------------------------------------
App::App()
{
	EngineConfig engineConfig;
	engineConfig.m_windowConfig.m_clientAspect = 2.0f;
	engineConfig.m_windowConfig.m_windowTitle = "Math Visual Tests";

	g_engine = new Engine( engineConfig );

	m_game = new GameNearestPoint();
	m_gameMode = GAMEMODE_NEAREST_POINT;

	m_lastFrameStartTime = GetCurrentTimeSeconds();

	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "Quit", Command_Quit );
}


//-----------------------------------------------------------------------------------------------
App::~App()
{
	delete m_game;
	m_game = nullptr;

	delete g_engine;
	g_engine = nullptr;
}


//-----------------------------------------------------------------------------------------------
void App::RunMainLoop()
{
	while ( !IsQuitting() )
	{
		RunFrame();
	}
}


//-----------------------------------------------------------------------------------------------
void App::RunFrame()
{
	g_engine->BeginFrame();

	Clock::TickSystemClock();

	double currentTime = GetCurrentTimeSeconds();
	float deltaSeconds = static_cast< float > ( currentTime - m_lastFrameStartTime );
	deltaSeconds = GetClamped( deltaSeconds, 0.f, 0.1f );

	m_lastFrameStartTime = currentTime;

	Update( deltaSeconds );
	Render(); // Draw the current state of the game
	g_engine->EndFrame(); // Allow engine subsystems to do post-frame stuff
}


//-----------------------------------------------------------------------------------------------
void App::UpdateFromKeyboard()
{
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
	{
		SetIsQuitting();
	}

	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F6 ) )
	{
		GameMode prevGameMode = static_cast< GameMode >( ( m_gameMode - 1 ) % NUM_GAME_MODES );
		if ( prevGameMode == INVALID_MODE )
		{
			prevGameMode = static_cast<GameMode>( NUM_GAME_MODES - 1 );
		}
		else if ( prevGameMode == NUM_GAME_MODES )
		{
			prevGameMode = static_cast<GameMode>( NUM_GAME_MODES - 1 );
		}
		m_gameMode = prevGameMode;
		HardReset( prevGameMode );
	}

	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F7 ) )
	{
		GameMode nextMode = static_cast<GameMode>( ( m_gameMode + 1 ) % NUM_GAME_MODES );
		if ( nextMode == INVALID_MODE )
		{
			nextMode = GAMEMODE_NEAREST_POINT;
		}
		else if ( nextMode == NUM_GAME_MODES )
		{
			nextMode = GAMEMODE_NEAREST_POINT;
		}
		m_gameMode = nextMode;
		HardReset( nextMode );
	}
}


//-----------------------------------------------------------------------------------------------
void App::UpdateFromController()
{
	XboxController const& controller = g_engine->m_inputSystem->GetController( 0 );

	if ( controller.WasButtonJustPressed( XBOX_BUTTON_BACK ) )
	{
		SetIsQuitting();
	}
}


//-----------------------------------------------------------------------------------------------
void App::HardReset( GameMode newGameMode )
{
	if ( m_game != nullptr )
	{
		delete m_game;
		m_game = nullptr;
	}

	switch ( newGameMode )
	{
		case INVALID_MODE:
			break;

		case GAMEMODE_NEAREST_POINT:
			m_game = new GameNearestPoint();
			break;

		case GAMEMODE_RAYCAST_VS_DISCS:
			m_game = new GameRaycastVsDiscs();
			break;

		case GAMEMODE_RAYCAST_VS_LINE_SEGMENTS:
			m_game = new GameRaycastVsLineSegment();
			break;

		case GAMEMODE_RAYCAST_VS_AABBS:
			m_game = new GameRaycastVsAABB();
			break;

		case GAMEMODE_3D_SHAPES:
			m_game = new Game3DShapes();
			break;

		case GAMEMODE_2D_CURVES:
			m_game = new Game2DCurves();
			break;

		case GAMEMODE_PACHINKO_2D:
			m_game = new GamePachinko2D();
			break;

		case NUM_GAME_MODES:
			break;

		default:
			break;
	}

	if ( newGameMode == GAMEMODE_3D_SHAPES )
	{
		g_engine->m_inputSystem->SetCursorMode( CursorMode::FPS );
	}
	else
	{
		g_engine->m_inputSystem->SetCursorMode( CursorMode::POINTER );
	}
}


//-----------------------------------------------------------------------------------------------
void App::Update( float deltaSeconds )
{
	UpdateFromKeyboard();

	float timeScale = 1.f;

	m_game->Update( deltaSeconds * timeScale );
}


//-----------------------------------------------------------------------------------------------
void App::Render() const
{
	g_engine->m_renderer->ClearScreen( Rgba8( 0, 0, 0 ) );
	m_game->Render();
	RenderHUD();
}


//-----------------------------------------------------------------------------------------------
void App::RenderHUD() const
{
	if ( m_game == nullptr || m_game->m_screenCamera == nullptr )
	{
		return;
	}

	g_engine->m_renderer->BeginCamera( *m_game->m_screenCamera );

	std::vector<Vertex> verts;
	AddVertsForTextTriangles2D( verts, "Mode (F6/F7 for prev/next): ", Vec2( 20.f, 760.f ), 20.f, Rgba8::YELLOW );
	switch ( m_gameMode )
	{
		case GAMEMODE_NEAREST_POINT:
			AddVertsForTextTriangles2D( verts, "Nearest Point (2D)", Vec2( 400.f, 760.f ), 20.f, Rgba8::YELLOW );
			break;
		case GAMEMODE_RAYCAST_VS_DISCS:
			AddVertsForTextTriangles2D( verts, "Raycast Vs. Disc (2D)", Vec2( 400.f, 760.f ), 20.f, Rgba8::YELLOW );
			break;
		case GAMEMODE_RAYCAST_VS_LINE_SEGMENTS:
			AddVertsForTextTriangles2D( verts, "Raycast Vs. Line Segments (2D)", Vec2( 400.f, 760.f ), 20.f, Rgba8::YELLOW );
			break;
		case GAMEMODE_RAYCAST_VS_AABBS:
			AddVertsForTextTriangles2D( verts, "Raycast Vs. AABBs (2D)", Vec2( 400.f, 760.f ), 20.f, Rgba8::YELLOW );
			break;
		case GAMEMODE_3D_SHAPES:
			AddVertsForTextTriangles2D( verts, "Test Shapes (3D)", Vec2( 400.f, 760.f ), 20.f, Rgba8::YELLOW );
			break;
		case GAMEMODE_2D_CURVES:
			AddVertsForTextTriangles2D( verts, "Easing, Curves, Splines (2D)", Vec2( 400.f, 760.f ), 20.f, Rgba8::YELLOW );
			break;
		case GAMEMODE_PACHINKO_2D:
			AddVertsForTextTriangles2D( verts, "Pachinko Machine (2D)", Vec2( 400.f, 760.f ), 20.f, Rgba8::YELLOW );
			break;
		default:
			break;
	}
	AddVertsForTextTriangles2D( verts, "F8 to randomize; ", Vec2( 20.f, 730.f ), 20.f, Rgba8::CYAN );
	switch ( m_gameMode )
	{
		case GAMEMODE_NEAREST_POINT:
			AddVertsForTextTriangles2D( verts, "WASD/Arrow: move reference point, hold T: slow", Vec2( 240.f, 730.f ), 20.f, Rgba8::CYAN );
			break;
		case GAMEMODE_RAYCAST_VS_DISCS:
			AddVertsForTextTriangles2D( verts, "LMB/RMB: set ray start/end; WASD: move start, IJKL: move end, arrows move ray, hold T: slow", Vec2( 240.f, 730.f ), 20.f, Rgba8::CYAN );
			break;
		case GAMEMODE_RAYCAST_VS_LINE_SEGMENTS:
			AddVertsForTextTriangles2D( verts, "LMB/RMB: set ray start/end; WASD: move start, IJKL: move end, arrows move ray, hold T: slow", Vec2( 240.f, 730.f ), 20.f, Rgba8::CYAN );
			break;
		case GAMEMODE_RAYCAST_VS_AABBS:
			AddVertsForTextTriangles2D( verts, "LMB/RMB: set ray start/end; WASD: move start, IJKL: move end, arrows move ray, hold T: slow", Vec2( 240.f, 730.f ), 20.f, Rgba8::CYAN );
			break;
		case GAMEMODE_3D_SHAPES:
		{
			Game3DShapes const* currGame = dynamic_cast< Game3DShapes const* >( m_game );
			std::string controlsText = "WASD: fly horizontal, QE: fly vertical, space: lock raycast, hold T: slow";
			if ( currGame != nullptr && currGame->m_nearestRaycastResult.m_didImpact )
			{
				controlsText += ", LMB: grab object";
			}
			AddVertsForTextTriangles2D( verts, controlsText, Vec2( 240.f, 730.f ), 20.f, Rgba8::CYAN );

			if ( currGame != nullptr && currGame->m_isShapeGrabbed && currGame->m_grabbedShapeType == 3 )
			{
				TestShapeOBB3D const* shape = currGame->m_testOBBs[currGame->m_grabbedShapeIndex];
				EulerAngles const& orientation = shape->m_orientation;
				std::string orientationText = Stringf( "I/O: yaw (%.1f), J/K: pitch (%.1f), N/M: roll (%.1f), U: reset",
					orientation.m_yawDegrees,
					orientation.m_pitchDegrees,
					orientation.m_rollDegrees );
				AddVertsForTextTriangles2D( verts, orientationText, Vec2( 20.f, 705.f ), 20.f, Rgba8::WHITE );
			}
			break;
		}
		case GAMEMODE_2D_CURVES:
		{
			Game2DCurves const* currGame = dynamic_cast<Game2DCurves const*>( m_game );
			int const numSubdivisions = ( currGame != nullptr ) ? currGame->GetNumSubdivisions() : 0;
			std::string controlsText = "W/E: prev/next easing function, N/M: curve subdivisions (" + std::to_string( numSubdivisions ) + "), hold T: slow";
			AddVertsForTextTriangles2D( verts, controlsText, Vec2( 240.f, 730.f ), 20.f, Rgba8::CYAN );
			break;
		}
		case GAMEMODE_PACHINKO_2D:
		{
			GamePachinko2D const* currGame = dynamic_cast< GamePachinko2D const* >( m_game );
			int const numBalls = ( currGame != nullptr ) ? static_cast< int >( currGame->m_balls.size() ) : 0;
			float const ballElasticity = ( currGame != nullptr ) ? currGame->m_ballElasticity : 0.f;
			bool isGravityOn = ( currGame != nullptr ) ? currGame->m_isGravityOn : false;
			bool isFloorOn = ( currGame != nullptr ) ? currGame->m_isFloorOn : false;
			bool isFixedTimestep = ( currGame != nullptr ) ? currGame->m_usingFixedTimestep : false;
			float physicsTimestep = ( currGame != nullptr ) ? currGame->m_physicsTimestep : 0.f;
			float deltaSeconds = ( currGame != nullptr ) ? ( float ) currGame->m_gameClock->GetDeltaSeconds() : 0.f;

			std::string const gravityText = isGravityOn ? "On" : "Off";
			std::string const floorText = isFloorOn ? "On" : "Off";
			std::string const timestepText = isFixedTimestep ? "Fixed" : "Variable";
			std::string const controlsTextLine1 = Stringf( "LMB/RMB move, T:slow, space/N:ball (%d), e=%.2f (Z,X)",
				numBalls,
				ballElasticity );
			std::string const controlsTextLine2 = Stringf( "G:gravity %s, B:bottom warp %s, %s timestep=%.2fms (P,[,]), dt=%.2fms", 
				gravityText.c_str(), 
				floorText.c_str(), 
				timestepText.c_str(), 
				physicsTimestep * 1000.f, 
				deltaSeconds * 1000.f );

			AddVertsForTextTriangles2D( verts, controlsTextLine1, Vec2( 240.f, 730.f ), 20.f, Rgba8::CYAN );
			AddVertsForTextTriangles2D( verts, controlsTextLine2, Vec2( 240.f, 705.f ), 20.f, Rgba8::CYAN );
			break;
		}
		default:
			break;
	}
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_game->m_screenCamera );
}


//-----------------------------------------------------------------------------------------------
bool App::Command_Quit( EventArgs& args )
{
	UNUSED( args );
	if ( g_app && !g_app->m_isQuitting )
	{
		g_app->SetIsQuitting();
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
void App::SetIsQuitting()
{
	m_isQuitting = true;
}


//-----------------------------------------------------------------------------------------------
bool App::IsQuitting() const
{
	return m_isQuitting;
}