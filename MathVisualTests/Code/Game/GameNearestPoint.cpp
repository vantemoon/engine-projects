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
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
GameNearestPoint::GameNearestPoint()
{
	m_referencePoint = new TestShapeDisc( Vec2( 100.f, 50.f ), 0.5f, 16 );

	m_triangle = new TestShapeTriangle( Vec2( 25.f, 15.f ), Vec2( 45.f, 35.f ), Vec2( 20.f, 40.f ) );
	m_disc = new TestShapeDisc( Vec2( 150.f, 40.f ), 5.f, 32 );
	m_aabb = new TestShapeAABB( AABB2( Vec2( 80.f, 20.f ), Vec2( 100.f, 35.f ) ) );
	m_obb = new TestShapeOBB( Vec2( 90.f, 70.f ), Vec2( 0.866f, 0.5f ), Vec2( 10.f, 10.f ) );
	m_capsule = new TestShapeCapsule( Vec2( 180.f, 50.f ), Vec2( 170.f, 60.f ), 2.5f, 16 );
	m_lineSegment = new TestShapeLineSegment( Vec2( 80.f, 50.f ), Vec2( 60.f, 30.f ), 0.5f, false );
	m_infiniteLine = new TestShapeLineSegment( Vec2( -100.f, 10.f ), Vec2( 220.f, 160.f ), 0.5f, true );

	float nearestPointRadius = 0.8f;
	int nearestPointSides = 16;
	m_nearestPointOnTriangle = new TestShapeDisc( Vec2( 0, 0 ), nearestPointRadius, nearestPointSides );
	m_nearestPointOnDisc = new TestShapeDisc( Vec2( 0, 0 ), nearestPointRadius, nearestPointSides );
	m_nearestPointOnAABB = new TestShapeDisc( Vec2( 0, 0 ), nearestPointRadius, nearestPointSides );
	m_nearestPointOnOBB = new TestShapeDisc( Vec2( 0, 0 ), nearestPointRadius, nearestPointSides );
	m_nearestPointOnCapsule = new TestShapeDisc( Vec2( 0, 0 ), nearestPointRadius, nearestPointSides );
	m_nearestPointOnLineSegment = new TestShapeDisc( Vec2( 0, 0 ), nearestPointRadius, nearestPointSides );
	m_nearestPointOnInfiniteLine = new TestShapeDisc( Vec2( 0, 0 ), nearestPointRadius, nearestPointSides );
}


//-----------------------------------------------------------------------------------------------
GameNearestPoint::~GameNearestPoint()
{
}


//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if ( m_isSlowMo )
	{
		deltaSeconds *= 0.1f;
	}

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	GetNearestPoints();
	UpdateFromKeyboard();
	Render();
}


//-----------------------------------------------------------------------------------------------
void GameNearestPoint::UpdateFromKeyboard()
{
	// Move reference point with WASD or arrow keys
	float moveSpeed = 0.5f;
	if ( m_isSlowMo )
	{
		moveSpeed *= 0.1f;
	}

	if ( g_engine->m_inputSystem->IsKeyDown( 'W' ) || g_engine->m_inputSystem->IsKeyDown( KEYCODE_UPARROW ) )
	{
		m_referencePoint->m_center.y += moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'S' ) || g_engine->m_inputSystem->IsKeyDown( KEYCODE_DOWNARROW ) )
	{
		m_referencePoint->m_center.y -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'A' ) || g_engine->m_inputSystem->IsKeyDown( KEYCODE_LEFTARROW ) )
	{
		m_referencePoint->m_center.x -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'D' ) || g_engine->m_inputSystem->IsKeyDown( KEYCODE_RIGHTARROW ) )
	{
		m_referencePoint->m_center.x += moveSpeed;
	}

	// Hold down T to enable slow motion
	if ( g_engine->m_inputSystem->IsKeyDown( 'T' ) )
	{
		m_isSlowMo = true;
	}
	else
	{
		m_isSlowMo = false;
	}
}


//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	Rgba8 darkBlue = Rgba8( 50, 80, 150, 255 );
	Rgba8 white = Rgba8( 255, 255, 255, 255 );
	Rgba8 orange = Rgba8( 255, 165, 0, 255 );

	std::vector<Vertex> verts;

	// Draw shapes
	AddVertsForTriangle2D( verts, m_triangle->m_ccw0, m_triangle->m_ccw1, m_triangle->m_ccw2, darkBlue );
	AddVertsForDisc2D( verts, m_disc->m_center, m_disc->m_radius, darkBlue, m_disc->m_numSides );
	AddVertsForAABB2D( verts, m_aabb->m_alignedBox, darkBlue );
	AddVertsForOBB2D( verts, m_obb->m_orientedBox, darkBlue );
	AddVertsForCapsule2D( verts, m_capsule->m_boneStart, m_capsule->m_boneEnd, m_capsule->m_radius, darkBlue, m_capsule->m_numSides );
	AddVertsForLineSegment2D( verts, m_lineSegment->m_start, m_lineSegment->m_end, m_lineSegment->m_thickness, darkBlue );
	AddVertsForLineSegment2D( verts, m_infiniteLine->m_start, m_infiniteLine->m_end, m_infiniteLine->m_thickness, darkBlue );

	// Draw nearest points
	AddVertsForDisc2D( verts, m_nearestPointOnTriangle->m_center, m_nearestPointOnTriangle->m_radius, orange, m_nearestPointOnTriangle->m_numSides );
	AddVertsForDisc2D( verts, m_nearestPointOnDisc->m_center, m_nearestPointOnDisc->m_radius, orange, m_nearestPointOnDisc->m_numSides );
	AddVertsForDisc2D( verts, m_nearestPointOnAABB->m_center, m_nearestPointOnAABB->m_radius, orange, m_nearestPointOnAABB->m_numSides );
	AddVertsForDisc2D( verts, m_nearestPointOnOBB->m_center, m_nearestPointOnOBB->m_radius, orange, m_nearestPointOnOBB->m_numSides );
	AddVertsForDisc2D( verts, m_nearestPointOnCapsule->m_center, m_nearestPointOnCapsule->m_radius, orange, m_nearestPointOnCapsule->m_numSides );
	AddVertsForDisc2D( verts, m_nearestPointOnLineSegment->m_center, m_nearestPointOnLineSegment->m_radius, orange, m_nearestPointOnLineSegment->m_numSides );
	AddVertsForDisc2D( verts, m_nearestPointOnInfiniteLine->m_center, m_nearestPointOnInfiniteLine->m_radius, orange, m_nearestPointOnInfiniteLine->m_numSides );

	// Draw reference point
	AddVertsForDisc2D( verts, m_referencePoint->m_center, m_referencePoint->m_radius, white, m_referencePoint->m_numSides );

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void GameNearestPoint::GetNearestPoints()
{
	m_nearestPointOnTriangle->m_center = GetNearestPointOnTriangle2D( m_referencePoint->m_center, m_triangle->m_ccw0, m_triangle->m_ccw1, m_triangle->m_ccw2 );
	m_nearestPointOnDisc->m_center = GetNearestPointOnDisc2D( m_referencePoint->m_center, m_disc->m_center, m_disc->m_radius );
	m_nearestPointOnAABB->m_center = GetNearestPointOnAABB2D( m_referencePoint->m_center, m_aabb->m_alignedBox );
	m_nearestPointOnOBB->m_center = GetNearestPointOnOBB2D( m_referencePoint->m_center, m_obb->m_orientedBox );
	m_nearestPointOnCapsule->m_center = GetNearestPointOnCapsule2D( m_referencePoint->m_center, m_capsule->m_boneStart, m_capsule->m_boneEnd, m_capsule->m_radius );
	m_nearestPointOnLineSegment->m_center = GetNearestPointOnLineSegment2D( m_referencePoint->m_center, m_lineSegment->m_start, m_lineSegment->m_end );
	m_nearestPointOnInfiniteLine->m_center = GetNearestPointOnInfiniteLine2D( m_referencePoint->m_center, m_infiniteLine->m_start, m_infiniteLine->m_end );
}