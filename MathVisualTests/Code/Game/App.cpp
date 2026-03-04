#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameNearestPoint.hpp"
#include "Game/GameRaycastVsAABB.hpp"
#include "Game/GameRaycastVsDiscs.hpp"
#include "Game/GameRaycastVsLineSegment.hpp"
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

	double currentTime = GetCurrentTimeSeconds();
	float deltaSeconds = static_cast<float> ( currentTime - m_lastFrameStartTime );
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

		case NUM_GAME_MODES:
			break;

		default:
			break;
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
	g_engine->m_renderer->BeginCamera( *m_game->m_screenCamera );

	std::vector<Vertex> verts;
	AddVertsForTextTriangles2D( verts, "Mode (F7 for next): ", Vec2( 20.f, 760.f ), 20.f, Rgba8::YELLOW );
	switch ( m_gameMode )
	{
		case GAMEMODE_NEAREST_POINT:
			AddVertsForTextTriangles2D( verts, "Nearest Point (2D)", Vec2( 280.f, 760.f ), 20.f, Rgba8::YELLOW );
			break;
		case GAMEMODE_RAYCAST_VS_DISCS:
			AddVertsForTextTriangles2D( verts, "Raycast Vs. Disc (2D)", Vec2( 280.f, 760.f ), 20.f, Rgba8::YELLOW );
			break;
		case GAMEMODE_RAYCAST_VS_LINE_SEGMENTS:
			AddVertsForTextTriangles2D( verts, "Raycast Vs. Line Segments (2D)", Vec2( 280.f, 760.f ), 20.f, Rgba8::YELLOW );
			break;
		case GAMEMODE_RAYCAST_VS_AABBS:
			AddVertsForTextTriangles2D( verts, "Raycast Vs. AABBs (2D)", Vec2( 280.f, 760.f ), 20.f, Rgba8::YELLOW );
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
			AddVertsForTextTriangles2D( verts, "LMB/RMB set ray start/end; WASD move start, IJKL move end, arrows move ray, hold T: slow", Vec2( 240.f, 730.f ), 20.f, Rgba8::CYAN );
			break;
		case GAMEMODE_RAYCAST_VS_LINE_SEGMENTS:
			AddVertsForTextTriangles2D( verts, "LMB/RMB set ray start/end; WASD move start, IJKL move end, arrows move ray, hold T: slow", Vec2( 240.f, 730.f ), 20.f, Rgba8::CYAN );
			break;
		case GAMEMODE_RAYCAST_VS_AABBS:
			AddVertsForTextTriangles2D( verts, "LMB/RMB set ray start/end; WASD move start, IJKL move end, arrows move ray, hold T: slow", Vec2( 240.f, 730.f ), 20.f, Rgba8::CYAN );
			break;
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