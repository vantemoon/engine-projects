#include "Game/Game2DCurves.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
Game2DCurves::Game2DCurves()
{
	float const panelPadding = 2.f;

	AABB2 usableSpace = AABB2( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y - 10.f ) );
	AABB2 topHalf = usableSpace;

	m_bezierPanel = topHalf.ChopTop( 0.5f );
	m_splinePanel = topHalf;
	m_easingPanel = m_bezierPanel.ChopLeft( 0.5f );

	m_splinePanel.PadOnAllSides( panelPadding );
	m_bezierPanel.PadOnAllSides( panelPadding );
	m_easingPanel.PadOnAllSides( panelPadding );
}


//-----------------------------------------------------------------------------------------------
Game2DCurves::~Game2DCurves()
{
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::Update( float deltaSeconds )
{
	UpdateFromKeyboard( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::UpdateFromKeyboard( [[maybe_unused]] float deltaSeconds )
{
	// DO NOTHING
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	g_engine->m_renderer->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
	g_engine->m_renderer->SetBlendMode( BlendMode::OPAQUE );

	std::vector<Vertex> verts;

	AddVertsForAABB2D( verts, m_splinePanel, Rgba8::RED, Vec2::ZERO, Vec2::ONE );
	AddVertsForAABB2D( verts, m_bezierPanel, Rgba8::GREEN, Vec2::ZERO, Vec2::ONE );
	AddVertsForAABB2D( verts, m_easingPanel, Rgba8::BLUE, Vec2::ZERO, Vec2::ONE );

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}