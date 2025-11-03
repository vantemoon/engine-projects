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
#include "Engine/Math/RandomNumberGenerator.hpp"
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
	CheckIfPointIsInsideShapes();
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

	// Randomize all shape positions with F8
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		RandomizeAllShapes();
	}
}


//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	Rgba8 darkBlue = Rgba8( 50, 80, 150, 255 );
	Rgba8 lightBlue = Rgba8( 100, 150, 255, 255 );
	Rgba8 white = Rgba8( 255, 255, 255, 255 );
	Rgba8 orange = Rgba8( 255, 165, 0, 255 );
	Rgba8 translucentWhite = Rgba8( 255, 255, 255, 30 );

	std::vector<Vertex> verts;

	// Draw shapes
	if ( m_isPointInTriangle )
	{
		AddVertsForTriangle2D( verts, m_triangle->m_ccw0, m_triangle->m_ccw1, m_triangle->m_ccw2, lightBlue );
	}
	else
	{
		AddVertsForTriangle2D( verts, m_triangle->m_ccw0, m_triangle->m_ccw1, m_triangle->m_ccw2, darkBlue );
	}
	if ( m_isPointInDisc )
	{
		AddVertsForDisc2D( verts, m_disc->m_center, m_disc->m_radius, lightBlue, m_disc->m_numSides );
	}
	else
	{
		AddVertsForDisc2D( verts, m_disc->m_center, m_disc->m_radius, darkBlue, m_disc->m_numSides );
	}
	if ( m_isPointInAABB )
	{
		AddVertsForAABB2D( verts, m_aabb->m_alignedBox, lightBlue );
	}
	else
	{
		AddVertsForAABB2D( verts, m_aabb->m_alignedBox, darkBlue );
	}
	if ( m_isPointInOBB )
	{
		AddVertsForOBB2D( verts, m_obb->m_orientedBox, lightBlue );
	}
	else
	{
		AddVertsForOBB2D( verts, m_obb->m_orientedBox, darkBlue );
	}
	if ( m_isPointInCapsule )
	{
		AddVertsForCapsule2D( verts, m_capsule->m_boneStart, m_capsule->m_boneEnd, m_capsule->m_radius, lightBlue, m_capsule->m_numSides );
	}
	else
	{
		AddVertsForCapsule2D( verts, m_capsule->m_boneStart, m_capsule->m_boneEnd, m_capsule->m_radius, darkBlue, m_capsule->m_numSides );
	}
	if ( m_isPointOnLineSegment )
	{
		AddVertsForLineSegment2D( verts, m_lineSegment->m_start, m_lineSegment->m_end, m_lineSegment->m_thickness, lightBlue );
	}
	else
	{
		AddVertsForLineSegment2D( verts, m_lineSegment->m_start, m_lineSegment->m_end, m_lineSegment->m_thickness, darkBlue );
	}
	if ( m_isPointOnInfiniteLine )
	{
		AddVertsForLineSegment2D( verts, m_infiniteLine->m_start, m_infiniteLine->m_end, m_infiniteLine->m_thickness, lightBlue );
	}
	else
	{
		AddVertsForLineSegment2D( verts, m_infiniteLine->m_start, m_infiniteLine->m_end, m_infiniteLine->m_thickness, darkBlue );
	}

	// Draw lines from reference point to nearest points
	AddVertsForLineSegment2D( verts, m_referencePoint->m_center, m_nearestPointOnTriangle->m_center, 0.2f, translucentWhite );
	AddVertsForLineSegment2D( verts, m_referencePoint->m_center, m_nearestPointOnDisc->m_center, 0.2f, translucentWhite );
	AddVertsForLineSegment2D( verts, m_referencePoint->m_center, m_nearestPointOnAABB->m_center, 0.2f, translucentWhite );
	AddVertsForLineSegment2D( verts, m_referencePoint->m_center, m_nearestPointOnOBB->m_center, 0.2f, translucentWhite );
	AddVertsForLineSegment2D( verts, m_referencePoint->m_center, m_nearestPointOnCapsule->m_center, 0.2f, translucentWhite );
	AddVertsForLineSegment2D( verts, m_referencePoint->m_center, m_nearestPointOnLineSegment->m_center, 0.2f, translucentWhite );
	AddVertsForLineSegment2D( verts, m_referencePoint->m_center, m_nearestPointOnInfiniteLine->m_center, 0.2f, translucentWhite );

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


//-----------------------------------------------------------------------------------------------
void GameNearestPoint::CheckIfPointIsInsideShapes()
{
	m_isPointInTriangle = IsPointInsideTriangle2D( m_referencePoint->m_center, m_triangle->m_ccw0, m_triangle->m_ccw1, m_triangle->m_ccw2 );
	m_isPointInDisc = IsPointInsideDisc2D( m_referencePoint->m_center, m_disc->m_center, m_disc->m_radius );
	m_isPointInAABB = IsPointInsideAABB2D( m_referencePoint->m_center, m_aabb->m_alignedBox );
	m_isPointInOBB = IsPointInsideOBB2D( m_referencePoint->m_center, m_obb->m_orientedBox );
	m_isPointInCapsule = IsPointInsideCapsule2D( m_referencePoint->m_center, m_capsule->m_boneStart, m_capsule->m_boneEnd, m_capsule->m_radius );
	m_isPointOnLineSegment = ( GetDistanceSquared2D( m_referencePoint->m_center, m_nearestPointOnLineSegment->m_center ) < 0.001f );
	m_isPointOnInfiniteLine = ( GetDistanceSquared2D( m_referencePoint->m_center, m_nearestPointOnInfiniteLine->m_center ) < 0.001f );
}


//-----------------------------------------------------------------------------------------------
void GameNearestPoint::RandomizeAllShapes()
{
	RandomNumberGenerator rng;

	// Randomize triangle
	Vec2 triangleCenter = Vec2( rng.RollRandomFloatInRange( 15.f, 185.f ), rng.RollRandomFloatInRange( 15.f, 85.f ) );
	float minRadius = 10.f;
	float maxRadius = 20.f;

	float baseAngle = rng.RollRandomFloatInRange( 0.f, 360.f );
	float angle1 = baseAngle;
	float angle2 = baseAngle + rng.RollRandomFloatInRange( 100.f, 140.f );
	float angle3 = baseAngle + rng.RollRandomFloatInRange( 220.f, 260.f );

	Vec2 ccw0 = triangleCenter + Vec2::MakeFromPolarDegrees( angle1, rng.RollRandomFloatInRange( minRadius, maxRadius ) );
	Vec2 ccw1 = triangleCenter + Vec2::MakeFromPolarDegrees( angle2, rng.RollRandomFloatInRange( minRadius, maxRadius ) );
	Vec2 ccw2 = triangleCenter + Vec2::MakeFromPolarDegrees( angle3, rng.RollRandomFloatInRange( minRadius, maxRadius ) );

	m_triangle->m_ccw0 = ccw0;
	m_triangle->m_ccw1 = ccw1;
	m_triangle->m_ccw2 = ccw2;

	// Randomize disc
	Vec2 discCenter = Vec2( rng.RollRandomFloatInRange( 15.f, 185.f ), rng.RollRandomFloatInRange( 15.f, 85.f ) );
	float discRadius = rng.RollRandomFloatInRange( 5.f, 15.f );

	m_disc->m_center = discCenter;
	m_disc->m_radius = discRadius;

	// Randomize AABB
	Vec2 aabbCenter = Vec2( rng.RollRandomFloatInRange( 15.f, 185.f ), rng.RollRandomFloatInRange( 15.f, 85.f ) );
	Vec2 aabbHalfDimensions = Vec2( rng.RollRandomFloatInRange( 5.f, 15.f ), rng.RollRandomFloatInRange( 5.f, 10.f ) );

	m_aabb->m_alignedBox = AABB2( aabbCenter - aabbHalfDimensions, aabbCenter + aabbHalfDimensions );

	// Randomize OBB
	Vec2 obbCenter = Vec2( rng.RollRandomFloatInRange( 15.f, 185.f ), rng.RollRandomFloatInRange( 15.f, 85.f ) );
	Vec2 obbHalfDimensions = Vec2( rng.RollRandomFloatInRange( 5.f, 15.f ), rng.RollRandomFloatInRange( 5.f, 10.f ) );
	float obbRotationDegrees = rng.RollRandomFloatInRange( 0.f, 360.f );
	Vec2 iBasisNormal = Vec2::MakeFromPolarDegrees( obbRotationDegrees, 1.f );

	m_obb->m_orientedBox = OBB2( obbCenter, iBasisNormal, obbHalfDimensions );

	// Randomize capsule
	Vec2 capsuleBoneStart = Vec2( rng.RollRandomFloatInRange( 15.f, 185.f ), rng.RollRandomFloatInRange( 15.f, 85.f ) );
	float capsuleBoneAngle = rng.RollRandomFloatInRange( 0.f, 360.f );
	float capsuleBoneLength = rng.RollRandomFloatInRange( 10.f, 30.f );
	Vec2 capsuleBoneEnd = capsuleBoneStart + Vec2::MakeFromPolarDegrees( capsuleBoneAngle, capsuleBoneLength );
	float capsuleRadius = rng.RollRandomFloatInRange( 2.f, 5.f );

	m_capsule->m_boneStart = capsuleBoneStart;
	m_capsule->m_boneEnd = capsuleBoneEnd;
	m_capsule->m_radius = capsuleRadius;

	// Randomize line segment
	Vec2 lineSegmentStart = Vec2( rng.RollRandomFloatInRange( 15.f, 185.f ), rng.RollRandomFloatInRange( 15.f, 85.f ) );
	float lineSegmentAngle = rng.RollRandomFloatInRange( 0.f, 360.f );
	float lineSegmentLength = rng.RollRandomFloatInRange( 10.f, 30.f );
	Vec2 lineSegmentEnd = lineSegmentStart + Vec2::MakeFromPolarDegrees( lineSegmentAngle, lineSegmentLength );

	m_lineSegment->m_start = lineSegmentStart;
	m_lineSegment->m_end = lineSegmentEnd;

	// Randomize infinite line
	Vec2 infiniteLinePoint = Vec2( rng.RollRandomFloatInRange( 15.f, 185.f ), rng.RollRandomFloatInRange( 15.f, 85.f ) );
	float infiniteLineAngle = rng.RollRandomFloatInRange( 0.f, 360.f );
	Vec2 infiniteLineDirection = Vec2::MakeFromPolarDegrees( infiniteLineAngle, 1.f );
	Vec2 infiniteLinePerp = infiniteLineDirection.GetRotatedBy90Degrees();

	m_infiniteLine->m_start = infiniteLinePoint - infiniteLinePerp * 200.f;
	m_infiniteLine->m_end = infiniteLinePoint + infiniteLinePerp * 200.f;
}