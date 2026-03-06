#include "Game/GameRaycastVsLineSegment.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
GameRaycastVsLineSegment::GameRaycastVsLineSegment()
{
	GenerateRandomLineSegments();
	for ( int raySegmentIndex = 0; raySegmentIndex < MAX_BOUNCES + 1; ++raySegmentIndex )
	{
		m_raySegmentDidHit[raySegmentIndex] = -1;
	}
}


//-----------------------------------------------------------------------------------------------
GameRaycastVsLineSegment::~GameRaycastVsLineSegment()
{
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsLineSegment::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if ( m_isSlowMo )
	{
		deltaSeconds *= 0.1f;
	}

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	RaycastVsLineSegments();
	UpdateFromKeyboard();
	Render();
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsLineSegment::UpdateFromKeyboard()
{
	float moveSpeed = 0.5f;
	if ( m_isSlowMo )
	{
		moveSpeed *= 0.1f;
	}

	// Move ray start and end positions with WASD and IJKL
	if ( g_engine->m_inputSystem->IsKeyDown( 'W' ) )
	{
		m_rayStartPos.y += moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'S' ) )
	{
		m_rayStartPos.y -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'A' ) )
	{
		m_rayStartPos.x -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'D' ) )
	{
		m_rayStartPos.x += moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'I' ) )
	{
		m_rayEndPos.y += moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'K' ) )
	{
		m_rayEndPos.y -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'J' ) )
	{
		m_rayEndPos.x -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'L' ) )
	{
		m_rayEndPos.x += moveSpeed;
	}

	// Use arrow keys to move both ray start and end positions together
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_UPARROW ) )
	{
		m_rayStartPos.y += moveSpeed;
		m_rayEndPos.y += moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_DOWNARROW ) )
	{
		m_rayStartPos.y -= moveSpeed;
		m_rayEndPos.y -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_LEFTARROW ) )
	{
		m_rayStartPos.x -= moveSpeed;
		m_rayEndPos.x -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_RIGHTARROW ) )
	{
		m_rayStartPos.x += moveSpeed;
		m_rayEndPos.x += moveSpeed;
	}

	// Use mouse to set ray start position with left mouse button
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_LBUTTON ) )
	{
		m_rayStartPos = GetMouseWorldPos();
	}

	// Use mouse to set ray end position with right mouse button
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_RBUTTON ) )
	{
		m_rayEndPos = GetMouseWorldPos();
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
		GenerateRandomLineSegments();
	}
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsLineSegment::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	Rgba8 darkBlue = Rgba8( 50, 80, 150, 255 );
	Rgba8 lightBlue = Rgba8( 100, 150, 255, 255 );

	std::vector<Vertex> verts;

	for ( int lineSegmentIndex = 0; lineSegmentIndex < MAX_LINE_SEGMENTS; ++lineSegmentIndex )
	{
		TestShapeLineSegment* lineSegment = m_testLineSegments[lineSegmentIndex];
		if ( !m_lineSegmentWasImpacted[lineSegmentIndex] )
		{
			AddVertsForLineSegment2D( verts, lineSegment->m_start, lineSegment->m_end, 0.3f, darkBlue );
		}
	}
	for ( int lineSegmentIndex = 0; lineSegmentIndex < MAX_LINE_SEGMENTS; ++lineSegmentIndex )
	{
		TestShapeLineSegment* lineSegment = m_testLineSegments[lineSegmentIndex];
		if ( m_lineSegmentWasImpacted[lineSegmentIndex] )
		{
			AddVertsForLineSegment2D( verts, lineSegment->m_start, lineSegment->m_end, 0.3f, lightBlue );
		}
	}

	if ( m_numRaySegments == 0 )
	{
		AddVertsForArrow2D( verts, m_rayStartPos, m_rayEndPos, 0.3f, 1.f, Rgba8::GREEN );
	}

	for ( int raySegmentIndex = 0; raySegmentIndex < m_numRaySegments; ++raySegmentIndex )
	{
		RaycastResult2D const& raySegment = m_raySegmentResults[raySegmentIndex];
		Vec2 segmentMaxEndPos = raySegment.m_rayStartPos + raySegment.m_rayFwdNormal * raySegment.m_rayMaxLength;

		if ( raySegment.m_didImpact )
		{
			AddVertsForArrow2D( verts, raySegment.m_rayStartPos, segmentMaxEndPos, 0.3f, 1.f, Rgba8( 190, 190, 190 ) );
			AddVertsForArrow2D( verts, raySegment.m_rayStartPos, raySegment.m_impactPos, 0.3f, 1.f, Rgba8::RED );
			AddVertsForArrow2D( verts, raySegment.m_impactPos, raySegment.m_impactPos + raySegment.m_impactNormal * 10.f, 0.3f, 1.f, Rgba8::YELLOW );
			AddVertsForDisc2D( verts, raySegment.m_impactPos, 0.5f, Rgba8::WHITE, 16 );
		}
		else
		{
			AddVertsForArrow2D( verts, raySegment.m_rayStartPos, segmentMaxEndPos, 0.3f, 1.f, Rgba8::GREEN );
		}
	}

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsLineSegment::GenerateRandomLineSegments()
{
	RandomNumberGenerator rng;

	for ( int lineSegmentIndex = 0; lineSegmentIndex < MAX_LINE_SEGMENTS; ++lineSegmentIndex )
	{
		Vec2 lineStart = Vec2( rng.RollRandomFloatInRange( 15.f, 185.f ), rng.RollRandomFloatInRange( 15.f, 85.f ) );
		float directionDegrees = rng.RollRandomFloatInRange( 0.f, 360.f );
		Vec2 lineDirection = Vec2::MakeFromPolarDegrees( directionDegrees );
		float lineLength = rng.RollRandomFloatInRange( 10.f, 30.f );
		Vec2 lineEnd = lineStart + lineDirection * lineLength;
		TestShapeLineSegment* lineSegment = new TestShapeLineSegment( lineStart, lineEnd, 0.3f, false );
		m_testLineSegments[lineSegmentIndex] = lineSegment;
	}
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsLineSegment::RaycastVsLineSegments()
{
	m_numRaySegments = 0;

	for ( int lineSegmentIndex = 0; lineSegmentIndex < MAX_LINE_SEGMENTS; ++ lineSegmentIndex )
	{
		m_lineSegmentWasImpacted[lineSegmentIndex] = false;
	}

	Vec2 initialRayVector = m_rayEndPos - m_rayStartPos;
	float totalMaxLength = initialRayVector.GetLength();
	if ( totalMaxLength <= 0.f )
	{
		return;
	}

	Vec2 currentRayStartPos = m_rayStartPos;
	Vec2 currentRayFwdNormal = initialRayVector.GetNormalized();
	float remainingLength = totalMaxLength;

	for ( int bounceIndex = 0; bounceIndex < MAX_BOUNCES && remainingLength > 0.f; ++ bounceIndex )
	{
		m_raySegmentDidHit[m_numRaySegments] = -1;

		RaycastResult2D closestImpactResult;
		closestImpactResult.m_rayStartPos = currentRayStartPos;
		closestImpactResult.m_rayFwdNormal = currentRayFwdNormal;
		closestImpactResult.m_rayMaxLength = remainingLength;
		closestImpactResult.m_didImpact = false;
		closestImpactResult.m_impactDist = FLT_MAX;

		for ( int lineSegmentIndex = 0; lineSegmentIndex < MAX_LINE_SEGMENTS; ++ lineSegmentIndex )
		{
			TestShapeLineSegment* lineSegment = m_testLineSegments[lineSegmentIndex];
			RaycastResult2D result = RaycastVsLineSegment2D( currentRayStartPos, currentRayFwdNormal, remainingLength, lineSegment->m_start, lineSegment->m_end );
			if ( result.m_didImpact && ( !closestImpactResult.m_didImpact || result.m_impactDist < closestImpactResult.m_impactDist ) )
			{
				closestImpactResult = result;
				m_raySegmentDidHit[m_numRaySegments] = lineSegmentIndex;
			}
		}

		m_raySegmentResults[m_numRaySegments] = closestImpactResult;
		++ m_numRaySegments;

		if ( !closestImpactResult.m_didImpact )
		{
			break;
		}

		m_lineSegmentWasImpacted[m_raySegmentDidHit[m_numRaySegments - 1]] = true;

		remainingLength -= closestImpactResult.m_impactDist;
		if ( remainingLength <= 0.f )
		{
			break;
		}

		Vec2 reflectedDirection = currentRayFwdNormal.GetReflected( closestImpactResult.m_impactNormal ).GetNormalized();
		currentRayStartPos = closestImpactResult.m_impactPos - currentRayFwdNormal * RAY_BOUNCE_RESTART_OFFSET;
		currentRayFwdNormal = reflectedDirection;
	}
}


//-----------------------------------------------------------------------------------------------
Vec2 GameRaycastVsLineSegment::GetMouseWorldPos() const
{
	Vec2 mouseUV = g_engine->m_window->GetNormalizedMouseUV();
	Vec2 cameraBottomLeft = m_worldCamera->GetOrthoBottomLeft();
	Vec2 cameraTopRight = m_worldCamera->GetOrthoTopRight();
	AABB2 cameraBounds = AABB2( cameraBottomLeft, cameraTopRight );
	Vec2 cursorWorldPos = cameraBounds.GetPointAtUV( mouseUV );
	return cursorWorldPos;
}