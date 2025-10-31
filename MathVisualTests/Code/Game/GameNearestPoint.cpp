#include "Game/GameNearestPoint.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
GameNearestPoint::GameNearestPoint()
{
}


//-----------------------------------------------------------------------------------------------
GameNearestPoint::~GameNearestPoint()
{
}


//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	Render();
}


//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	std::vector<Vertex> verts;

	// Add verts to the vertex vector

	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}