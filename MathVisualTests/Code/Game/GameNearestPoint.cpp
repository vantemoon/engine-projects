#include "Game/GameNearestPoint.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TestShapeAABB.hpp"
#include "Game/TestShapeCapsule.hpp"
#include "Game/TestShapeDisc.hpp"
#include "Game/TestShapeLineSegment.hpp"
#include "Game/TestShapeOBB.hpp"
#include "Game/TestShapeTriangle.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
GameNearestPoint::GameNearestPoint()
{
	m_triangle = new TestShapeTriangle( Vec2( 25.f, 15.f ), Vec2( 45.f, 35.f ), Vec2( 20.f, 40.f ) );
	m_disc = new TestShapeDisc( Vec2( 150.f, 40.f ), 5.f, 32 );
	m_aabb = new TestShapeAABB( AABB2( Vec2( 80.f, 20.f ), Vec2( 100.f, 35.f ) ) );
	m_obb = new TestShapeOBB( Vec2( 90.f, 70.f ), Vec2( 0.866f, 0.5f ), Vec2( 10.f, 10.f ) );
	m_capsule = new TestShapeCapsule( Vec2( 180.f, 50.f ), Vec2( 170.f, 60.f ), 2.5f, 16 );
	m_lineSegment = new TestShapeLineSegment( Vec2( 100.f, 50.f ), Vec2( 60.f, 55.f ), 0.5f, false );
	m_infiniteLine = new TestShapeLineSegment( Vec2( -100.f, 10.f ), Vec2( 220.f, 160.f ), 0.5f, true );
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

	Rgba8 darkBlue = Rgba8( 50, 80, 150, 255 );
	std::vector<Vertex> verts;

	// Draw shapes
	AddVertsForTriangle2D( verts, m_triangle->m_ccw0, m_triangle->m_ccw1, m_triangle->m_ccw2, darkBlue );
	AddVertsForDisc2D( verts, m_disc->m_center, m_disc->m_radius, darkBlue, m_disc->m_numSides );
	AddVertsForAABB2D( verts, m_aabb->m_alignedBox, darkBlue );
	AddVertsForOBB2D( verts, m_obb->m_orientedBox, darkBlue );
	AddVertsForCapsule2D( verts, m_capsule->m_boneStart, m_capsule->m_boneEnd, m_capsule->m_radius, darkBlue, m_capsule->m_numSides );
	AddVertsForLineSegment2D( verts, m_lineSegment->m_start, m_lineSegment->m_end, m_lineSegment->m_thickness, darkBlue );
	AddVertsForLineSegment2D( verts, m_infiniteLine->m_start, m_infiniteLine->m_end, m_infiniteLine->m_thickness, darkBlue );

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}