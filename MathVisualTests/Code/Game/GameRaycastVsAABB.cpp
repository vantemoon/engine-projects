#include "Game/GameRaycastVsAABB.hpp"
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
GameRaycastVsAABB::GameRaycastVsAABB()
{
	GenerateRandomAABBs();
	for ( int raySegmentIndex = 0; raySegmentIndex < MAX_BOUNCES + 1; ++raySegmentIndex )
	{
		m_raySegmentDidHit[raySegmentIndex] = -1;
	}
}


//-----------------------------------------------------------------------------------------------
GameRaycastVsAABB::~GameRaycastVsAABB()
{
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsAABB::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	
	if ( m_isSlowMo )
	{
		deltaSeconds *= 0.1f;
	}
	
	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
	
	RaycastVsAABBs();
	UpdateFromKeyboard();
	Render();
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsAABB::UpdateFromKeyboard()
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
		GenerateRandomAABBs();
	}
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsAABB::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	Rgba8 darkBlue = Rgba8( 50, 80, 150, 255 );
	Rgba8 lightBlue = Rgba8( 100, 150, 255, 255 );

	std::vector<Vertex> verts;

	for ( int aabbIndex = 0; aabbIndex < MAX_AABBS; ++aabbIndex )
	{
		TestShapeAABB* aabb = m_testAABBs[aabbIndex];
		if ( !m_aabbWasImpacted[aabbIndex] )
		{
			AddVertsForAABB2D( verts, aabb->m_alignedBox, darkBlue, Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
		}
	}
	for ( int aabbIndex = 0; aabbIndex < MAX_AABBS; ++aabbIndex )
	{
		TestShapeAABB* aabb = m_testAABBs[aabbIndex];
		if ( m_aabbWasImpacted[aabbIndex] )
		{
			AddVertsForAABB2D( verts, aabb->m_alignedBox, lightBlue, Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
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
void GameRaycastVsAABB::GenerateRandomAABBs()
{
	RandomNumberGenerator rng;

	for ( int aabbIndex = 0; aabbIndex < MAX_AABBS; ++aabbIndex )
	{
		float boxWidth = rng.RollRandomFloatInRange( 10.f, 30.f );
		float boxHeight = rng.RollRandomFloatInRange( 10.f, 30.f );
		float boxLeft = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_X - boxWidth );
		float boxBottom = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_Y - boxHeight );
		m_testAABBs[aabbIndex] = new TestShapeAABB( AABB2( Vec2( boxLeft, boxBottom ), Vec2( boxLeft + boxWidth, boxBottom + boxHeight ) ) );
		m_aabbWasImpacted[aabbIndex] = false;
	}
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsAABB::RaycastVsAABBs()
{
	m_numRaySegments = 0;

	for ( int raySegmentIndex = 0; raySegmentIndex < MAX_BOUNCES + 1; ++raySegmentIndex )
	{
		m_raySegmentDidHit[raySegmentIndex] = -1;
	}

	for ( int aabbIndex = 0; aabbIndex < MAX_AABBS; ++aabbIndex )
	{
		m_aabbWasImpacted[aabbIndex] = false;
	}

	Vec2 initialRayVector = m_rayEndPos - m_rayStartPos;
	float totalMaxLength = initialRayVector.GetLength();
	if ( totalMaxLength <= 0.f )
	{
		return;
	}

	Vec2 currentRayStartPos = m_rayStartPos;
	Vec2 currentRayFwdNormal = initialRayVector / totalMaxLength;
	float remainingMaxLength = totalMaxLength;

	for ( int raySegmentIndex = 0; raySegmentIndex < MAX_BOUNCES + 1; ++raySegmentIndex )
	{
		m_raySegmentDidHit[raySegmentIndex] = -1;

		RaycastResult2D closestImpact;
		closestImpact.m_rayStartPos = currentRayStartPos;
		closestImpact.m_rayFwdNormal = currentRayFwdNormal;
		closestImpact.m_rayMaxLength = remainingMaxLength;
		closestImpact.m_didImpact = false;

		for ( int aabbIndex = 0; aabbIndex < MAX_AABBS; ++aabbIndex )
		{
			RaycastResult2D impact = RaycastVsAABB2D( currentRayStartPos, currentRayFwdNormal, remainingMaxLength, m_testAABBs[aabbIndex]->m_alignedBox );

			if ( !impact.m_didImpact )
			{
				continue;
			}

			if ( !closestImpact.m_didImpact || impact.m_impactDist < closestImpact.m_impactDist )
			{
				closestImpact = impact;
				m_raySegmentDidHit[raySegmentIndex] = aabbIndex;
			}
		}

		m_raySegmentResults[raySegmentIndex] = closestImpact;
		m_numRaySegments = raySegmentIndex + 1;

		if ( !closestImpact.m_didImpact )
		{
			break;
		}

		m_aabbWasImpacted[m_raySegmentDidHit[raySegmentIndex]] = true;

		if ( closestImpact.m_impactDist <= 0.f )
		{
			break;
		}

		remainingMaxLength -= closestImpact.m_impactDist;
		if ( remainingMaxLength <= 0.f )
		{
			break;
		}

		currentRayStartPos = closestImpact.m_impactPos + closestImpact.m_impactNormal * RAY_BOUNCE_RESTART_OFFSET;
		currentRayFwdNormal = currentRayFwdNormal.GetReflected( closestImpact.m_impactNormal ).GetNormalized();
	}
}


//-----------------------------------------------------------------------------------------------
Vec2 GameRaycastVsAABB::GetMouseWorldPos() const
{
	Vec2 mouseUV = g_engine->m_window->GetNormalizedMouseUV();
	Vec2 cameraBottomLeft = m_worldCamera->GetOrthoBottomLeft();
	Vec2 cameraTopRight = m_worldCamera->GetOrthoTopRight();
	AABB2 cameraBounds = AABB2( cameraBottomLeft, cameraTopRight );
	Vec2 cursorWorldPos = cameraBounds.GetPointAtUV( mouseUV );
	return cursorWorldPos;
}