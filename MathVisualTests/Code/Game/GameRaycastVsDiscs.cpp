#include "Game/GameRaycastVsDiscs.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
GameRaycastVsDiscs::GameRaycastVsDiscs()
{
	GenerateRandomDiscs();
}


//-----------------------------------------------------------------------------------------------
GameRaycastVsDiscs::~GameRaycastVsDiscs()
{
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if ( m_isSlowMo )
	{
		deltaSeconds *= 0.1f;
	}

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	UpdateFromKeyboard();
	Render();
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::UpdateFromKeyboard()
{
	// Hold down T to enable slow motion
	if ( g_engine->m_inputSystem->IsKeyDown( 'T' ) )
	{
		m_isSlowMo = true;
	}
	else
	{
		m_isSlowMo = false;
	}

	// Randomize all shape positions with F8
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		GenerateRandomDiscs();
	}
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	Rgba8 darkBlue = Rgba8( 50, 80, 150, 255 );

	std::vector<Vertex> verts;

	for ( int discIndex = 0; discIndex < MAX_DISCS; ++discIndex )
	{
		TestShapeDisc* disc = m_testDiscs[discIndex];
		Rgba8 discColor = darkBlue;
		AddVertsForDisc2D( verts, disc->m_center, disc->m_radius, discColor, disc->m_numSides );
	}

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::GenerateRandomDiscs()
{
	RandomNumberGenerator rng;

	for ( int discIndex = 0; discIndex < MAX_DISCS; ++discIndex )
	{
		Vec2 discCenter = Vec2( rng.RollRandomFloatInRange( 15.f, 185.f ), rng.RollRandomFloatInRange( 15.f, 85.f ) );
		float discRadius = rng.RollRandomFloatInRange( 5.f, 15.f );

		TestShapeDisc* disc = new TestShapeDisc( discCenter, discRadius, 32 );
		m_testDiscs[discIndex] = disc;
	}
}