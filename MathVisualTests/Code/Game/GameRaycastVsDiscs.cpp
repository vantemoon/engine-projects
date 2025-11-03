#include "Game/GameRaycastVsDiscs.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
GameRaycastVsDiscs::GameRaycastVsDiscs()
{
}


//-----------------------------------------------------------------------------------------------
GameRaycastVsDiscs::~GameRaycastVsDiscs()
{
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	Render();
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	std::vector<Vertex> verts;

	Vec2 start = Vec2( 50.f, 25.f );
	Vec2 end = Vec2( 150.f, 75.f );
	float thickness = 0.5f;
	Rgba8 green = Rgba8( 0, 255, 0, 255 );
	AddVertsForLineSegment2D( verts, start, end, thickness, green );

	Vec2 discCenter = Vec2( WORLD_CENTER_X, WORLD_CENTER_Y );
	float discRadius = 10.f;
	Rgba8 blue = Rgba8( 0, 0, 255, 255 );
	AddVertsForDisc2D( verts, discCenter, discRadius, blue, 32 );

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}